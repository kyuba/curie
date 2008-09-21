/*
 *  exec.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 03/06/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <curie/io-system.h>
#include <curie/memory.h>
#include <curie/io.h>
#include <curie/exec.h>
#include <curie/exec-system.h>
#include <curie/network.h>

#define MAXFD 1024

static struct memory_pool
  exec_context_pool = MEMORY_POOL_INITIALISER(sizeof(struct exec_context));

struct exec_context *execute(unsigned int options,
                             char **command,
                             char **environment)
{
    struct exec_context *context =
        (struct exec_context *)get_pool_mem(&exec_context_pool);
    int pid, i;
    struct io *proc_stdout_in, *proc_stdout_out,
              *proc_stdin_in,  *proc_stdin_out;

    context->in = (struct io *)0;
    context->out = (struct io *)0;

    if (!(options & EXEC_CALL_NO_IO)) {
        net_open_loop(&proc_stdout_in, &proc_stdout_out);
        net_open_loop(&proc_stdin_in, &proc_stdin_out);
    }

    pid = a_fork();
    context->pid = pid;

    switch (pid) {
        case -1:
            io_close (proc_stdout_in);
            io_close (proc_stdout_out);
            io_close (proc_stdin_in);
            io_close (proc_stdin_out);

            context->in = (struct io *)0;
            context->out = (struct io *)0;
            break;
        case 0:
            if ((command != (char **)0) || (options & EXEC_CALL_PURGE)) {
                if (!(options & EXEC_CALL_NO_IO)) {
                    a_dup (proc_stdin_in->fd, 0);
                    a_dup (proc_stdout_out->fd, 1);
                }

                for (i = 3; i < MAXFD; i++) {
                    a_close(i);
                }

                if (command != (char **)0) {
                    a_exec (command[0], command, environment);
                }
            } else if (!(options & EXEC_CALL_NO_IO)) {
                io_close (proc_stdout_in);
                io_close (proc_stdin_out);

                context->in = proc_stdin_in;
                context->out = proc_stdout_out;
            }

            break;
        default:
            if (!(options & EXEC_CALL_NO_IO)) {
                io_close (proc_stdin_in);
                io_close (proc_stdout_out);

                context->in = proc_stdout_in;
                context->out = proc_stdin_out;
            }
            break;
    }

    context->status = ps_running;

    return context;
}

void free_exec_context (struct exec_context *context) {
    free_pool_mem ((void *)context);
}

void check_exec_context (struct exec_context *context) {
    int i;

    switch (context->pid) {
        case 0:
        case -1:
            return;
        default:
            if ((context->status == ps_running) &&
                (a_wait (context->pid, &i) != wr_running)) {
                context->exitstatus = i;
                context->status = ps_terminated;
            }
            return;
    }
}
