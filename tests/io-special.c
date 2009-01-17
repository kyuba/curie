/*
 *  io-special.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 17/01/2009.
 *  Copyright 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2009, Magnus Deininger All rights reserved.
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

#include <curie/main.h>
#include <curie/sexpr.h>
#include <curie/memory.h>
#include <curie/multiplex.h>

static struct sexpr_io *queue;
static struct io *queue_io;

sexpr sxm1337 = make_integer (-1337);

static void *rm_recover(unsigned long int s, void *c, unsigned long int l)
{
    cexit(22);
    return (void *)0;
}

static void *gm_recover(unsigned long int s)
{
    cexit(23);
    return (void *)0;
}

static void mx_sx_queue_read (sexpr sx, struct sexpr_io *io, void *aux)
{
    if (truep (equalp (sx, sxm1337))) cexit (0);
    sx_destroy (sx);
}

int cmain()
{
    set_resize_mem_recovery_function(rm_recover);
    set_get_mem_recovery_function(gm_recover);

    queue_io = io_open_special();

    queue    = sx_open_io    (queue_io, queue_io);

    sx_write (queue, sxm1337);
    if (falsep(equalp(sx_read(queue), sxm1337))) return 1;

    multiplex_sexpr();

    multiplex_add_sexpr (queue, mx_sx_queue_read, (void *)0);

    sx_write (queue, sxm1337);

    while (multiplex() == mx_ok);

    return 2;
}
