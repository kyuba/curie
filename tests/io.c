/*
 *  io.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 08/06/2008.
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

#include "curie/io.h"
#include <errno.h>

#define TESTDATA "THIS IS SOME TEST DATA"
#define TESTDATA_LENGTH (unsigned int)(sizeof(TESTDATA) -1)

int a_main(void) {
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
