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
#include <atomic/string.h>
#include <atomic/tree.h>

/*@-nullinit@*/
/* need some sentinel values... */

/*@-mustfreeonly@*/
/* somehow can't seem to write initialisers without this... */

/*@-temptrans@*/

/*@-compmempass@*/
/* this is actually intentional. */

/*@-dependenttrans@*/
/* and so is this. */

/*@-freshtrans@*/
/*@-mustfreefresh@*/
/*@-onlytrans@*/

static const struct sexpr const _sx_nil = { .type = sxt_nil, .references = -1 };
static const struct sexpr const _sx_false = { .type = sxt_false, .references = -1 };
static const struct sexpr const _sx_true = { .type = sxt_true, .references = -1 };
static const struct sexpr const _sx_empty_list = { .type = sxt_empty_list, .references = -1 };
static const struct sexpr const _sx_end_of_list = { .type = sxt_end_of_list, .references = -1 };
static const struct sexpr const _sx_end_of_file = { .type = sxt_end_of_file, .references = -1 };
static const struct sexpr const _sx_not_a_number = { .type = sxt_not_a_number, .references = -1 };
static const struct sexpr const _sx_nonexistent = { .type = sxt_nonexistent, .references = -1 };

const struct sexpr * const sx_nil = &_sx_nil;
const struct sexpr * const sx_false = &_sx_false;
const struct sexpr * const sx_true = &_sx_true;
const struct sexpr * const sx_empty_list = &_sx_empty_list;
const struct sexpr * const sx_end_of_list = &_sx_end_of_list;
const struct sexpr * const sx_end_of_file = &_sx_end_of_file;
const struct sexpr * const sx_not_a_number = &_sx_not_a_number;
const struct sexpr * const sx_nonexistent = &_sx_nonexistent;

/*@notnull@*/ /*@only@*/ static struct memory_pool *sx_cons_pool = (struct memory_pool *)0;
/*@notnull@*/ /*@only@*/ static struct memory_pool *sx_int_pool = (struct memory_pool *)0;

/*@notnull@*/ /*@only@*/ static struct tree *sx_string_tree = (struct tree *)0;
/*@notnull@*/ /*@only@*/ static struct tree *sx_symbol_tree = (struct tree *)0;

struct sexpr *cons(struct sexpr *sx_car, struct sexpr *sx_cdr) {
    struct sexpr_cons *rv;

    if (sx_cons_pool == (struct memory_pool *)0) {
        sx_cons_pool = create_memory_pool (sizeof (struct sexpr_cons));
    }

    rv = get_pool_mem (sx_cons_pool);

    rv->type = sxt_cons;
    rv->car = sx_car;
    rv->cdr = sx_cdr;

    rv->references = 1;

    return (struct sexpr*)rv;
}

struct sexpr *make_integer(signed long long number) {
    struct sexpr_integer *rv;

    if (sx_int_pool == (struct memory_pool *)0) {
        sx_int_pool = create_memory_pool (sizeof (struct sexpr_integer));
    }

    rv = get_pool_mem (sx_int_pool);

    rv->references = 1;
    rv->type = sxt_integer;
    rv->integer = number;

    return (struct sexpr*)rv;
}

static struct sexpr *make_string_or_symbol (const char *string, char symbol) {
    struct sexpr_string_or_symbol *s;
    unsigned long len;
    unsigned int i;
    int_32 hash = str_hash_unaligned (string, &len);
    struct tree_node *n;

    if (sx_string_tree == (struct tree *)0) {
        sx_string_tree = tree_create();
        sx_symbol_tree = tree_create();
    }

    if ((n = tree_get_node ((symbol == (char)1) ? sx_symbol_tree : sx_string_tree, (int_pointer)hash))) {
        s = (struct sexpr_string_or_symbol *)node_get_value (n);
        (s->references)++;
        return (struct sexpr *)s;
    }

    s = aalloc (sizeof (struct sexpr_string_or_symbol) + len + 1);

    for (i = 0; string[i] != (char)0; i++) {
        s->character_data[i] = string[i];
    }

    s->references = 1;
    s->type = (symbol == (char)1) ? sxt_symbol : sxt_string;
    return (struct sexpr *)s;
}

struct sexpr *make_string(const char *string) {
    return make_string_or_symbol (string, (char)0);
}

struct sexpr *make_symbol(const char *symbol) {
    return make_string_or_symbol (symbol, (char)1);
}

void sx_destroy(struct sexpr *sexpr) {
    if (sexpr->references == -1) {
        return;
    } else if (sexpr->references > 1) {
        if (sexpr->type == sxt_cons) {
            sx_destroy ((struct sexpr *)car (sexpr));
            sx_destroy ((struct sexpr *)cdr (sexpr));
        }
        (sexpr->references)--;
        return;
    }

    switch (sexpr->type) {
        case sxt_integer:
            free_pool_mem (sexpr);
            return;
        case sxt_cons:
            sx_destroy ((struct sexpr *)car (sexpr));
            sx_destroy ((struct sexpr *)cdr (sexpr));
            free_pool_mem (sexpr);
            return;
        case sxt_string:
        case sxt_symbol:
            {
                unsigned int length = 0;
                while (((struct sexpr_string_or_symbol *)sexpr)->character_data[length] != (char)0) length++;
                length++;
                afree ((sizeof (struct sexpr_string_or_symbol) + length), sexpr);
            }
            return;
        default:
            return;
    }
}
