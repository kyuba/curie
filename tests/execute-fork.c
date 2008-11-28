/*
 *  execute-fork.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 07/08/2008.
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

#include "curie/exec.h"
#include "curie/sexpr.h"

#include <curie/sexpr-internal.h>

define_symbol(test_symbol_1, "hello-world!");
define_string(test_string_1, "hello \"world\"!");
define_symbol(sym_success, "success");
define_symbol(sym_failure, "failure");

int cmain(void) {
    struct exec_context *context;
    int rv = 0;
    sexpr i,
          t1 = test_symbol_1,
          t2 = test_string_1,
          t3 = sym_success;
    struct sexpr_io *io, *stdio = sx_open_stdio();

    context = execute(0, (char **)0, (char **)0);

    switch (context->pid) {
        case -1: /* only way this would fail ... */
            sx_write (stdio, sym_failure);
            return 1;
        case 0:
            {
                io = sx_open_io (context->in, context->out);

                do {
                    i = sx_read(io);
                } while (i == sx_nonexistent);

                rv |= (equalp(i, t1) ? 0 : 1) << 1;
                sx_destroy(i);

                do {
                    i = sx_read(io);
                } while (i == sx_nonexistent);

                rv |= (equalp(i, t2) ? 0 : 1) << 2;
                sx_destroy(i);

                i = sx_read(io);
                rv |= (equalp(i, sx_nonexistent) ? 0 : 1) << 3;
                sx_destroy(i);

                if (rv != 0) {
                    sx_write (io, sym_failure);
                } else {
                    sx_write (io, t3);
                }
            }

            break;
        default:
            {
                io = sx_open_io (context->in, context->out);

                sx_write (io, t1);
                sx_write (io, t2);

                while (io_commit (io->out) == io_incomplete);

                do {
                    i = sx_read(io);
                } while (i == sx_nonexistent);

                rv |= (equalp(i, t3) ? 0 : 1) << 4;
                sx_destroy(i);
            }

            break;
    }

    if (rv != 0) {
        sx_write (stdio, sym_failure);
        sx_write (stdio, make_integer (rv));
    } else {
        sx_write (stdio, t3);
    }

    sx_destroy (t1);
    sx_destroy (t2);
    sx_destroy (t3);

    sx_close_io (io);
    sx_close_io (stdio);

    if (context->pid > 0) {
        while (context->status == ps_running)
            check_exec_context (context);
    }
    free_exec_context (context);

    return rv;
}
