/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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

#include <curie/memory.h>
#include <curie/sexpr.h>
#include <curie/string.h>
#include <curie/tree.h>

static struct memory_pool sx_cons_pool =
        MEMORY_POOL_INITIALISER(sizeof (struct sexpr_cons));

static struct tree sx_string_tree = TREE_INITIALISER;
static struct tree sx_symbol_tree = TREE_INITIALISER;

sexpr cons(sexpr sx_car, sexpr sx_cdr) {
    struct sexpr_cons *rv = get_pool_mem (&sx_cons_pool);

    if (rv == (struct sexpr_cons *)0)
    {
        return sx_nonexistent;
    }

    rv->header.type = sxt_cons;
    rv->car = sx_car;
    rv->cdr = sx_cdr;

    rv->header.references = 1;

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
    int_32 hash = str_hash (string, &len);
    struct tree_node *n;

    if ((n = tree_get_node ((symbol == (char)1) ? &sx_symbol_tree : &sx_string_tree, (int_pointer)hash))) {
        if ((s = (struct sexpr_string_or_symbol *)node_get_value (n))
             != (struct sexpr_string_or_symbol *)0)
        {
            (s->header.references)++;
            return (sexpr)s;
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

    s->header.references = 1;
    s->header.type = (symbol == (char)1) ? sxt_symbol : sxt_string;

    /*@-memtrans -mustfree@*/
    return (sexpr)s;
    /*@=memtrans =mustfree@*/
}

sexpr make_string(const char *string) {
    return make_string_or_symbol (string, (char)0);
}

sexpr make_symbol(const char *symbol) {
    return make_string_or_symbol (symbol, (char)1);
}

void sx_destroy(sexpr sxx) {
    if (!pointerp(sxx)) return;

    if (stringp(sxx) || symbolp(sxx))
    {
        struct sexpr_string_or_symbol *sx
                = (struct sexpr_string_or_symbol *)sx_pointer(sxx);

        if (sx->header.references == (unsigned short int)(~0)) return;

        if (sx->header.references > 1)
        {
            (sx->header.references)--;
        }
        else
        {
            unsigned long length = 0;
            int_32 hash;

            hash = str_hash (((struct sexpr_string_or_symbol *)sx)->character_data, &length);

            if (sx->header.type == sxt_string) {
                tree_remove_node(&sx_string_tree, (int_pointer)hash);
            } else {
                tree_remove_node(&sx_symbol_tree, (int_pointer)hash);
            }

            afree ((sizeof (struct sexpr_string_or_symbol) + length), sx);
        }
    }
    else if (consp(sxx))
    {
        struct sexpr_cons *sx
                = (struct sexpr_cons *)sx_pointer(sxx);

        if (sx->header.references == (unsigned short int)(~0)) return;

        sx_destroy ((sexpr) car (sx));
        sx_destroy ((sexpr) cdr (sx));

        if (sx->header.references > 1)
        {
            (sx->header.references)--;
        }
        else
        {
            free_pool_mem (sx);
        }
    }
}

void sx_xref(sexpr sxx) {
    if (!pointerp(sxx)) return;

    if (stringp(sxx) || symbolp(sxx))
    {
        struct sexpr_string_or_symbol *sx
                = (struct sexpr_string_or_symbol *)sx_pointer(sxx);

        if (sx->header.references == (unsigned short int)(~0)) return;

        (sx->header.references) += 1;
    }
    else if (consp(sxx))
    {
        struct sexpr_cons *sx
                = (struct sexpr_cons *)sx_pointer(sxx);

        sx_xref(car(sx));
        sx_xref(cdr(sx));

        if (sx->header.references == (unsigned short int)(~0)) return;

        (sx->header.references) += 1;
    }
}