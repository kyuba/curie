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

#include "curie/io.h"
#include "curie/sexpr.h"

define_string(str_hello_world, "hello world!");

#define SX_TEST_INTEGER (signed long int)1337
#define SX_TEST_INTEGER2 (signed long int)-23

int cmain(void) {
    struct io *out = io_open_write ("build/to-sexpr-write"), *in = io_open (0);
    struct sexpr_io *io = sx_open_io (in, out);
    sexpr s  = str_hello_world;
    sexpr s1 = str_hello_world;
    sexpr s2 = make_integer(SX_TEST_INTEGER);
    sexpr list;

    sx_write (io, s);

    s = make_integer (SX_TEST_INTEGER);

    sx_write (io, s);

    s = make_integer (SX_TEST_INTEGER2);

    sx_write (io, s);

    list = cons(s1, s2);

    sx_write(io, list);

    list = cons(s1, cons(s2, sx_end_of_list));

    sx_write(io, list);

    sx_close_io (io);

    return 0;
}
