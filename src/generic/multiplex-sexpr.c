/*
 *  multiplex-sexpr.c
 *  atomic-libc
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

#include <atomic/multiplex.h>
#include <atomic/memory.h>

struct io_element {
    struct sexpr_io *io;
    void (*on_read)(struct sexpr *, struct sexpr_io *, void *);
    void *data;
};

static struct memory_pool list_pool = MEMORY_POOL_INITIALISER(sizeof (struct io_element));

void multiplex_sexpr () {
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_io();
        installed = (char)1;
    }
}

static void mx_on_read (struct io *r, void *d) {
    struct io_element *element = (struct io_element *)d;
    struct sexpr *sx = sx_read (element->io);

    while (sx != sx_nonexistent) {
        element->on_read (sx, element->io, element->data);
        sx = sx_read (element->io);
    }
}

void multiplex_add_sexpr (struct sexpr_io *io, void (*on_read)(struct sexpr *, struct sexpr_io *, void *), void *data) {
    struct io_element *element = get_pool_mem (&list_pool);

    element->io = io;
    element->on_read = on_read;
    element->data = data;

    multiplex_add_io (io->in, mx_on_read, (void *)element);
    multiplex_add_io_no_callback(io->out);
}
