/*
 *  multiplex-network.c
 *  curie-libc
 *
 *  Created by Magnus Deininger on 10/08/2008.
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

#include "curie/multiplex.h"
#include "curie/network.h"
#include "curie/exec.h"
#include "curie/main.h"

static void mx_on_death(struct exec_context *cx, void *d) {
    a_exit (2);
}

static void sx_read_child(struct sexpr *sx, struct sexpr_io *io, void *d)
{
    if (sx == sx_end_of_file) {
        sx_close_io (io);
        a_exit (4);
    }

    sx_write (io, sx);
}

static void sx_read_parent_sexpr(struct sexpr *sx, struct sexpr_io *io, void *d)
{
    if (sx == sx_end_of_file) {
        sx_close_io (io);
        a_exit (8);
    }

    sx_write (io, sx);
}

static void sx_read_parent(struct sexpr *sx, struct sexpr_io *io, struct sexpr *sx_tests[4])
{
    static int num = 0;

    if (equalp(sx, sx_tests[num])) {
        if (num == 3) {
            sx_close_io (io);
            a_exit (0);
        } else {
            num++;
        }
    } else {
        a_exit (10+num);
    }
}

static void sx_on_connect(struct sexpr_io *io, struct sexpr *sx_tests[4]) {
    sx_write (io, sx_tests[0]);
    sx_write (io, sx_tests[1]);
    sx_write (io, sx_tests[2]);
    sx_write (io, sx_tests[3]);

    multiplex_add_sexpr (io, (void (*)(struct sexpr *, struct sexpr_io *, void *))sx_read_parent, (void *)sx_tests);
}

static void sx_read_child_confirmation(struct sexpr *sx, struct sexpr_io *io, void *d)
{
    if ((void *)sx == d) {
        multiplex_add_socket_sx ("test-socket-sexpr-io", sx_read_child, (void *)0);
    }
}

int a_main(void) {
    struct exec_context *context;
    struct exec_call *call;
    struct sexpr *commence = make_symbol ("commence");

    struct sexpr *sx_tests[4] = {
        make_string ("hello world!"),
        make_integer (1337),
        make_symbol ("meow"),
        make_integer (-23)
    };

    struct sexpr_io *io;

    call = create_exec_call();
    call->options |= EXEC_CALL_PURGE;
    context = execute(call);

    multiplex_network();
    multiplex_sexpr();

    switch (context->pid) {
        case -1:
            return 3;
        case 0:
            multiplex_add_sexpr (sx_open_stdio (),
                                 sx_read_child_confirmation, (void *)commence);

            while (multiplex() == mx_ok);

            return 5;
        default:
            multiplex_process();

            io = sx_open_io (context->in, context->out);

            multiplex_add_process(context, mx_on_death, (void *)0);
            multiplex_add_socket_listener_sx ("test-socket-sexpr-io", (void (*)(struct sexpr_io *, void *))sx_on_connect, (void *)sx_tests);

            multiplex_add_sexpr(io, sx_read_parent_sexpr, (void *)0);

            sx_write (io, commence);

            while (multiplex() == mx_ok);

            return 7;
    }
}
