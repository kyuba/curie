/**\file
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
*/

#include "curie/io.h"
#include "curie/multiplex.h"

static void mx_on_read(struct io *io, void *wx) {
    struct io *w = (struct io *)wx;

    io_write (w, io->buffer, io->length);
    io->position = io->length;
}

static void mx_on_close(struct io *io, void *wx) {
    struct io *w = (struct io *)wx;

    multiplex_del_io (w);
}

int cmain(void) {
    struct io *r = io_open_read("multiplexer-test-data.sx"),
              *w = io_open_write("to-multiplexer-test-data.sx");

    multiplex_io();

    multiplex_add_io (r, mx_on_read, mx_on_close, (void *)w);
    multiplex_add_io_no_callback (w);

    while (multiplex() == mx_ok);

    return 0;
}
