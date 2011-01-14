/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include <syscall/syscall.h>
#include <curie/io-system.h>
#include <curie/memory.h>
#include <curie/io.h>
#include <curie/exec.h>
#include <curie/exec-system.h>
#include <curie/network.h>

#define MAXFD 1024

struct exec_context *execute(unsigned int options,
                             char **command,
                             char **environment)
{
    static struct memory_pool
            pool = MEMORY_POOL_INITIALISER(sizeof(struct exec_context));

    struct exec_context *context =
            (struct exec_context *)get_pool_mem(&pool);
    int pid, i, retries = 0;
    struct io *proc_stdout_in, *proc_stdout_out,
              *proc_stdin_in,  *proc_stdin_out;

    context->in = (struct io *)0;
    context->out = (struct io *)0;

    if ((options & EXEC_CALL_NO_IO) == 0) {
        net_open_loop(&proc_stdout_in, &proc_stdout_out);
        net_open_loop(&proc_stdin_in, &proc_stdin_out);
    }

    while (((pid = sys_fork()) == -1) && (retries < 10))
    {
        retries++;
    }

    context->pid = pid;

    switch (pid) {
        case -1:
            if ((options & EXEC_CALL_NO_IO) == 0) {
                io_close (proc_stdout_in);
                io_close (proc_stdout_out);
                io_close (proc_stdin_in);
                io_close (proc_stdin_out);
            }

            context->in = (struct io *)0;
            context->out = (struct io *)0;
            context->status = ps_terminated;
            return context;
        case 0:
            if (options & EXEC_CALL_CREATE_SESSION)
            {
                sys_setsid();
            }

            if ((command != (char **)0) ||
                ((options & EXEC_CALL_PURGE) != 0))
            {
                if ((options & EXEC_CALL_NO_IO) == 0) {
                    (void)a_dup (proc_stdin_in->fd, 0);
                    (void)a_dup (proc_stdout_out->fd, 1);
                }

                for (i = 3; i < MAXFD; i++) {
                    (void)a_close(i);
                }

                if (command != (char **)0) {
                    sys_execve (command[0], command, environment);
                }
            } else if ((options & EXEC_CALL_NO_IO) == 0) {
                io_close (proc_stdout_in);
                io_close (proc_stdin_out);

                context->in = proc_stdin_in;
                context->out = proc_stdout_out;
            }

            break;
        default:
            if ((options & EXEC_CALL_NO_IO) == 0) {
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
