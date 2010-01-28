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

#define TESTDATA "THIS IS SOME TEST DATA"
#define TESTDATA_LENGTH (unsigned int)(sizeof(TESTDATA) -1)

int cmain(void) {
    struct io *out = io_open_write ("temporary-io-test-file"), *in;
    char cont = (char)1;
    unsigned int i;
    int rv = 0;

    if (io_write (out, TESTDATA, TESTDATA_LENGTH) != io_complete) {
        io_close (out);
        return 11;
    }

    io_close (out);

    in = io_open_read ("temporary-io-test-file");

    do {
        enum io_result res = io_read (in);

        switch (res) {
            case io_changes:
            case io_no_change:
                cont = (char)1;
                break;
            case io_end_of_file:
                cont = (char)0;
                break;
            case io_unrecoverable_error:
                rv = 12;
                goto end;
            default:
                rv = res;
                goto end;
        }
    } while (cont == (char)1);

    if (in->length != TESTDATA_LENGTH) {
        rv = 13;
        goto end;
    }

    for (i = 0; i < in->length; i++) {
        if ((in->buffer)[i] != TESTDATA[i]) {
            rv = 14;
            goto end;
        }
    }

    end:

    io_close (in);

    return 0;
}
