/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#include "curie/multiplex.h"
#include "curie/network.h"
#include "curie/exec.h"
#include "curie/main.h"

static void mx_on_death(struct exec_context *cx, void *d) {
    cexit (2);
}

static void sx_read_child(sexpr sx, struct sexpr_io *io, void *d)
{
    if (sx == sx_end_of_file) {
        sx_close_io (io);
        cexit (4);
    }

    sx_write (io, sx);
}

static void sx_read_parent_sexpr(sexpr sx, struct sexpr_io *io, void *d)
{
    if (sx == sx_end_of_file) {
        sx_close_io (io);
        cexit (8);
    }

    sx_write (io, sx);
}

static void sx_read_parent(sexpr sx, struct sexpr_io *io, sexpr sx_tests[4])
{
    static int num = 0;

    if (truep(equalp(sx, sx_tests[num]))) {
        if (num == 3) {
            sx_close_io (io);
            cexit (0);
        } else {
            num++;
        }
    } else {
        cexit (10+num);
    }
}

static void sx_on_connect(struct sexpr_io *io, sexpr sx_tests[4]) {
    sx_write (io, sx_tests[0]);
    sx_write (io, sx_tests[1]);
    sx_write (io, sx_tests[2]);
    sx_write (io, sx_tests[3]);

    multiplex_add_sexpr (io, (void (*)(sexpr, struct sexpr_io *, void *))
                             sx_read_parent, (void *)sx_tests);
}

static void sx_read_child_confirmation(sexpr sx, struct sexpr_io *io, void *d)
{
    if (truep(equalp(sx, (sexpr)d))) {
        multiplex_add_socket_client_sx ("build/to-socket-sexpr-io",
                                        sx_read_child, (void *)0);
    }
}

int cmain(void) {
    define_string (str_hello_world, "hello world!");
    define_symbol (sym_meow, "meow");
    define_symbol (commence, "commence");

    struct exec_context *context;

    sexpr sx_tests[4] = {
        str_hello_world,
        make_integer (1337),
        sym_meow,
        make_integer (-23)
    };

    struct sexpr_io *io;

    context = execute(EXEC_CALL_PURGE, (char **)0, (char **)0);

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
            multiplex_add_socket_sx ("build/to-socket-sexpr-io",
                                     (void (*)(struct sexpr_io *, void *))
                                     sx_on_connect, (void *)sx_tests);

            multiplex_add_sexpr(io, sx_read_parent_sexpr, (void *)0);

            sx_write (io, commence);

            while (multiplex() == mx_ok);

            return 7;
    }
}
