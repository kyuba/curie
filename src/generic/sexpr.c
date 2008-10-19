/*
 *  sexpr.c
 *  libcurie
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

#include <curie/memory.h>
#include <curie/sexpr.h>
#include <curie/string.h>
#include <curie/tree.h>

static const struct sexpr_generic _sx_nil =
  { .type = sxt_nil, .references = -1 };
static const struct sexpr_generic _sx_false =
  { .type = sxt_false, .references = -1 };
static const struct sexpr_generic _sx_true =
  { .type = sxt_true, .references = -1 };
static const struct sexpr_generic _sx_empty_list =
  { .type = sxt_empty_list, .references = -1 };
static const struct sexpr_generic _sx_end_of_list =
  { .type = sxt_end_of_list, .references = -1 };
static const struct sexpr_generic _sx_end_of_file =
  { .type = sxt_end_of_file, .references = -1 };
static const struct sexpr_generic _sx_not_a_number =
  { .type = sxt_not_a_number, .references = -1 };
static const struct sexpr_generic _sx_nonexistent =
  { .type = sxt_nonexistent, .references = -1 };
static const struct sexpr_generic _sx_dot =
  { .type = sxt_dot, .references = -1 };

/*@-compmempass@*/
sexpr const sx_nil =
        (sexpr const)&_sx_nil;
sexpr const sx_false =
        (sexpr const)&_sx_false;
sexpr const sx_true =
        (sexpr const)&_sx_true;
sexpr const sx_empty_list =
        (sexpr const)&_sx_empty_list;
sexpr const sx_end_of_list =
        (sexpr const)&_sx_end_of_list;
sexpr const sx_end_of_file =
        (sexpr const)&_sx_end_of_file;
sexpr const sx_not_a_number =
        (sexpr const)&_sx_not_a_number;
sexpr const sx_nonexistent =
        (sexpr const)&_sx_nonexistent;
sexpr const sx_dot =
        (sexpr const)&_sx_dot;
/*@=compmempass@*/

static struct memory_pool sx_cons_pool =
        MEMORY_POOL_INITIALISER(sizeof (struct sexpr_cons));
static struct memory_pool sx_int_pool =
        MEMORY_POOL_INITIALISER(sizeof (struct sexpr_integer));

static struct tree sx_string_tree = TREE_INITIALISER;
static struct tree sx_symbol_tree = TREE_INITIALISER;

sexpr cons(sexpr sx_car, sexpr sx_cdr) {
    struct sexpr_cons *rv = get_pool_mem (&sx_cons_pool);

    if (rv == (struct sexpr_cons *)0)
    {
        return sx_nonexistent;
    }

    rv->type = sxt_cons;
    rv->car = sx_car;
    rv->cdr = sx_cdr;

    rv->references = 1;

    /*@-memtrans -mustfree@*/
    return (sexpr)rv;
    /*@=memtrans =mustfree@*/
}

sexpr make_integer(signed long number) {
    struct sexpr_integer *rv = get_pool_mem (&sx_int_pool);

    if (rv == (struct sexpr_integer *)0)
    {
        return sx_nonexistent;
    }

    rv->references = 1;
    rv->type = sxt_integer;
    rv->integer = number;

    /*@-memtrans -mustfree@*/
    return (sexpr)rv;
    /*@=memtrans =mustfree@*/
}

/*@shared@*/ static sexpr make_string_or_symbol
        (const char *string, char symbol)
{
    struct sexpr_string_or_symbol *s;
    unsigned long len;
    unsigned int i;
    int_32 hash = str_hash_unaligned (string, &len);
    struct tree_node *n;

    if ((n = tree_get_node ((symbol == (char)1) ? &sx_symbol_tree : &sx_string_tree, (int_pointer)hash))) {
        if ((s = (struct sexpr_string_or_symbol *)node_get_value (n))
             != (struct sexpr_string_or_symbol *)0)
        {
            (s->references)++;
            return (sexpr )s;
        }
    }

    if ((s = aalloc (sizeof (struct sexpr_string_or_symbol) + len + 1))
         == (struct sexpr_string_or_symbol *)0)
    {
        return sx_nonexistent;
    }

    tree_add_node_value ((symbol == (char)1) ? &sx_symbol_tree : &sx_string_tree, (int_pointer)hash, s);

    for (i = 0; string[i] != (char)0; i++) {
        s->character_data[i] = string[i];
    }
    s->character_data[i] = (char)0;

    s->references = 1;
    s->type = (symbol == (char)1) ? sxt_symbol : sxt_string;

    /*@-memtrans -mustfree@*/
    return (sexpr )s;
    /*@=memtrans =mustfree@*/
}

sexpr make_string(const char *string) {
    return make_string_or_symbol (string, (char)0);
}

sexpr make_symbol(const char *symbol) {
    return make_string_or_symbol (symbol, (char)1);
}

void sx_destroy(sexpr sx) {
    if (sx->references == -1) {
        return;
    } else if (sx->references > 1) {
        if (sx->type == sxt_cons) {
            sx_destroy ((sexpr) car (sx));
            sx_destroy ((sexpr) cdr (sx));
        }
        (sx->references)--;
        return;
    }

    switch (sx->type) {
        case sxt_integer:
            free_pool_mem (sx);
            return;
        case sxt_cons:
            sx_destroy ((sexpr) car (sx));
            sx_destroy ((sexpr) cdr (sx));
            free_pool_mem (sx);
            return;
        case sxt_string:
        case sxt_symbol:
            {
                unsigned long length = 0;
                int_32 hash;

                hash = str_hash_unaligned (((struct sexpr_string_or_symbol *)sx)->character_data, &length);

                if (sx->type == sxt_string) {
                    tree_remove_node(&sx_string_tree, (int_pointer)hash);
                } else {
                    tree_remove_node(&sx_symbol_tree, (int_pointer)hash);
                }

                afree ((sizeof (struct sexpr_string_or_symbol) + length), sx);
            }
            return;
        default:
            return;
    }
}

void sx_xref(sexpr sx) {
    if (sx->references == -1) return;
    if (consp(sx)) {
        sx_xref(car(sx));
        sx_xref(cdr(sx));
    }

    sx->references += 1;
}
