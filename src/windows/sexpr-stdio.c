/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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

#include <curie/sexpr.h>
#include <curie/io.h>
#include <curie/io-system.h>

#include <curie/sexpr-internal.h>

#include <windows.h>

struct sexpr_io *sx_open_stdio() {
    struct io *in, *out;

    if ((in = io_open (GetStdHandle(STD_INPUT_HANDLE))) == (struct io *)0)
    {
        return (struct sexpr_io *)0;
    }

    if ((out = io_open (GetStdHandle(STD_OUTPUT_HANDLE))) == (struct io *)0)
    {
        io_close (in);
        return (struct sexpr_io *)0;
    }

    return sx_open_io (in, out);
}

struct sexpr_io *sx_open_stdout() {
    struct io *out;

    if ((out = io_open (GetStdHandle(STD_OUTPUT_HANDLE))) == (struct io *)0)
    {
        return (struct sexpr_io *)0;
    }

    return sx_open_io (io_open_null, out);
}

struct sexpr_io *sx_open_stdin() {
    struct io *in;

    if ((in = io_open (GetStdHandle(STD_INPUT_HANDLE))) == (struct io *)0)
    {
        return (struct sexpr_io *)0;
    }

    return sx_open_io (in, io_open_null);
}
