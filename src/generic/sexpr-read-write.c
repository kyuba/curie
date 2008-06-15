/*
 *  sexpr-read-write.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 15/06/2008.
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

#include <atomic/memory.h>
#include <atomic/sexpr.h>

/*@-nullinit@*/
/* need some sentinel values... */

/*@-mustfreeonly@*/
/* somehow can't seem to write initialisers without this... */

/*@notnull@*/ struct memory_pool *sx_io_pool = (struct memory_pool *)0;

struct sexpr_io *sx_open_io(struct io *in, struct io *out) {
    struct sexpr_io *rv;

    if (sx_io_pool == (struct memory_pool *)0) {
        sx_io_pool = create_memory_pool (sizeof (struct sexpr_io));
    }

    rv = get_pool_mem (sx_io_pool);

    rv->in = in;
    rv->out = out;

    return rv;
}

struct sexpr_io *sx_open_io_fd(int in, int out) {
    return sx_open_io (io_open (in), io_open(out));
}

void sx_close_io (struct sexpr_io *io) {
    io_close (io->in);
    io_close (io->out);

    free_pool_mem (io);
}

struct sexpr *sx_read(struct sexpr_io *io);
char sx_write(struct sexpr_io *io, struct sexpr *sexpr);
