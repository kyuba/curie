/*
 *  sexpr.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 01/06/2008.
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

/*@-temptrans@*/

const struct sexpr const _sx_nil = { .type = sxt_nil };
const struct sexpr const _sx_false = { .type = sxt_false };
const struct sexpr const _sx_true = { .type = sxt_true };
const struct sexpr const _sx_empty_list = { .type = sxt_empty_list };
const struct sexpr const _sx_end_of_list = { .type = sxt_end_of_list };
const struct sexpr const _sx_end_of_file = { .type = sxt_end_of_file };
const struct sexpr const _sx_not_a_number = { .type = sxt_not_a_number };
const struct sexpr const _sx_nonexistent = { .type = sxt_nonexistent };

const struct sexpr * const sx_nil = &_sx_nil;
const struct sexpr * const sx_false = &_sx_false;
const struct sexpr * const sx_true = &_sx_true;
const struct sexpr * const sx_empty_list = &_sx_empty_list;
const struct sexpr * const sx_end_of_list = &_sx_end_of_list;
const struct sexpr * const sx_end_of_file = &_sx_end_of_file;
const struct sexpr * const sx_not_a_number = &_sx_not_a_number;
const struct sexpr * const sx_nonexistent = &_sx_nonexistent;

/*@notnull@*/ struct memory_pool *sx_io_pool = (struct memory_pool *)0;
/*@notnull@*/ struct memory_pool *sx_cons_pool = (struct memory_pool *)0;
/*@notnull@*/ struct memory_pool *sx_int_pool = (struct memory_pool *)0;

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

struct sexpr *cons(struct sexpr *sx_car, struct sexpr *sx_cdr) {
    struct sexpr_cons *rv;

    if (sx_cons_pool == (struct memory_pool *)0) {
        sx_cons_pool = create_memory_pool (sizeof (struct sexpr_cons));
    }

    rv = get_pool_mem (sx_cons_pool);

    rv->type = sxt_cons;
    rv->car = sx_car;
    rv->cdr = sx_cdr;

    return (struct sexpr*)rv;
}

struct sexpr *make_integer(signed long long number) {
    struct sexpr_integer *rv;

    if (sx_int_pool == (struct memory_pool *)0) {
        sx_int_pool = create_memory_pool (sizeof (struct sexpr_integer));
    }

    rv = get_pool_mem (sx_int_pool);

    rv->type = sxt_integer;
    rv->integer = number;

    return (struct sexpr*)rv;
}

struct sexpr *make_string(const char *string);
struct sexpr *make_symbol(const char *symbol);

void sx_destroy(struct sexpr *sexpr) {
    switch (sexpr->type) {
        case sxt_integer:
        case sxt_cons:
            free_pool_mem (sexpr);
            return;
        default:
            return;
    }
}

void *sx_list_map (struct sexpr *sexpr, void (*callback)(struct sexpr *, void *), void *p);
struct sexpr *sx_list_fold (struct sexpr *sexpr, void (*callback)(struct sexpr *));
