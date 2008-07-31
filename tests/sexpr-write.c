/*
 *  sexpr-write.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 29/06/2008.
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

#include "atomic/io.h"
#include "atomic/sexpr.h"

#define SX_TEST_STRING "hello world!"
#define SX_TEST_INTEGER (signed long long)1337
#define SX_TEST_INTEGER2 (signed long long)-23

int atomic_main(void) {
    struct io *out = io_open_write ("temporary-sexpr-write"), *in = io_open (0);
    struct sexpr_io *io = sx_open_io (in, out);
    struct sexpr *s = make_string (SX_TEST_STRING);
    struct sexpr *s1 = make_string (SX_TEST_STRING);
    struct sexpr *s2 = make_integer(SX_TEST_INTEGER);
    struct sexpr *list;

    sx_write (io, s);

    sx_destroy(s);

    s = make_integer (SX_TEST_INTEGER);

    sx_write (io, s);

    sx_destroy (s);

    s = make_integer (SX_TEST_INTEGER2);

    sx_write (io, s);

    sx_destroy (s);

    list = cons(s1, s2);

    sx_write(io, list);

    sx_destroy(list);

    list = cons(s1, cons(s2, sx_end_of_list));

    sx_write(io, list);

    sx_destroy(list);

    sx_close_io (io);

    return 0;
}
