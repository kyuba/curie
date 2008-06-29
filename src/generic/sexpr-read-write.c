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
#include <atomic/io.h>

/*@-nullinit@*/
/* need some sentinel values... */

/*@-mustfreeonly@*/
/* somehow can't seem to write initialisers without this... */

/*@notnull@*/ /*@only@*/ static struct memory_pool *sx_io_pool = (struct memory_pool *)0;

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

const struct sexpr *sx_read(struct sexpr_io *io) {
    enum io_result r;

    r = io_read(io->in);

    return sx_nonexistent;
}


static void sx_write_string_or_symbol (struct io *io, const struct sexpr_string_or_symbol *sexpr) {
    int i;

    for (i = 0; sexpr->character_data[i] != 0; i++);

    if (sexpr->type == sxt_string) {
      (void)io_write (io, "\"", 1);
      (void)io_write (io, sexpr->character_data, i);
      (void)io_write (io, "\"", 1);
    } else
      (void)io_write (io, sexpr->character_data, i);
}

static void sx_write_cons (struct sexpr_io *io, const struct sexpr_cons *sexpr) {
    (void)io_write (io->out, "(", 1);

  retry:

    sx_write (io, car(sexpr));
    sexpr = (const struct sexpr_cons *)cdr ((const struct sexpr *)sexpr);

    if (sexpr->type == sxt_cons) {
        (void)io_write (io->out, " ", 1);
        goto retry;
    }

    if (sexpr->type != sxt_end_of_list) {
        (void)io_write (io->out, ". ", 2);
        sx_write (io, (const struct sexpr *)sexpr);
    }

    (void)io_write (io->out, ")", 1);
}

static void sx_write_integer (struct io *io, const struct sexpr_integer *sexpr) {
    char num [33];
    signed long i = (signed long)sexpr->integer;
    unsigned int j = 0;

    do {
       char s = '0' + (i % 10);

       num[j] = s;

       i = i / 10;
       j++;
    } while ((i != 0) && (j < 32));

    num[j] = 0;

    (void)io_write (io, num, j);
}

void sx_write(struct sexpr_io *io, const struct sexpr *sexpr) {
    switch (sexpr->type) {
        case sxt_symbol:
        case sxt_string:
            sx_write_string_or_symbol (io->out, (const struct sexpr_string_or_symbol *)sexpr);
            break;

        case sxt_cons:
            sx_write_cons (io, (const struct sexpr_cons *)sexpr);
            break;

        case sxt_integer:
            sx_write_integer (io->out, (const struct sexpr_integer *)sexpr);
            break;

        case sxt_nil:
            (void)io_write (io->out, "#nil", 4);
            break;
        case sxt_false:
            (void)io_write (io->out, "#f", 2);
            break;
        case sxt_true:
            (void)io_write (io->out, "#t", 2);
            break;
        case sxt_empty_list:
            (void)io_write (io->out, "()", 2);
            break;
        case sxt_end_of_list:
            (void)io_write (io->out, "#eol", 4);
            break;
        case sxt_end_of_file:
            (void)io_write (io->out, "#eof", 4);
            break;
        case sxt_not_a_number:
            (void)io_write (io->out, "#nan", 4);
            break;
        case sxt_nonexistent:
            (void)io_write (io->out, "#ne", 3);
            break;
    }

    (void)io_write (io->out, "\n", 1);
}
