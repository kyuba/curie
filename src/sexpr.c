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
#include <curie/sexpr-internal.h>
#include <curie/string.h>
#include <curie/gc.h>
#include <curie/tree.h>

static struct tree sx_cons_tree   = TREE_INITIALISER;
static struct tree sx_string_tree = TREE_INITIALISER;
static struct tree sx_symbol_tree = TREE_INITIALISER;
unsigned long gc_base_items       = 0;

sexpr cons(sexpr sx_car, sexpr sx_cdr) {
    static struct memory_pool pool =
            MEMORY_POOL_INITIALISER(sizeof (struct sexpr_cons));
    sexpr t[2] = { sx_car, sx_cdr };
    int_32 hash = bin_hash ((const char *)t, sizeof (t));
    struct sexpr_cons *rv;
    struct tree_node *n;

    if ((n = tree_get_node (&sx_cons_tree, (int_pointer)hash)))
    {
        if ((rv = (struct sexpr_cons *)node_get_value (n))
             != (struct sexpr_cons *)0)
        {
            return (sexpr)rv;
        }
    }

    rv = get_pool_mem (&pool);

    if (rv == (struct sexpr_cons *)0)
    {
        return sx_nonexistent;
    }

    rv->type = sxt_cons;
    rv->car  = sx_car;
    rv->cdr  = sx_cdr;

    tree_add_node_value (&sx_cons_tree, (int_pointer)hash, rv);

    gc_base_items++;

    return (sexpr)rv;
}

static sexpr make_string_or_symbol
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

    s->type = (symbol == (char)1) ? sxt_symbol : sxt_string;

    gc_base_items++;

    return (sexpr)s;
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

        unsigned long length = 0;
        int_32 hash;

        hash = str_hash (((struct sexpr_string_or_symbol *)sx)->character_data, &length);

        if (sx->type == sxt_string) {
            tree_remove_node(&sx_string_tree, (int_pointer)hash);
        } else {
            tree_remove_node(&sx_symbol_tree, (int_pointer)hash);
        }

        afree ((sizeof (struct sexpr_string_or_symbol) + length), sx);
        gc_base_items--;
    }
    else if (consp(sxx))
    {
        struct sexpr_cons *sx = (struct sexpr_cons *)sx_pointer(sxx);
        sexpr t[2] = { sx->car, sx->cdr };
        int_32 hash = bin_hash ((const char *)t, sizeof (t));

        tree_remove_node(&sx_cons_tree, (int_pointer)hash);

        free_pool_mem (sx);
        gc_base_items--;
    }
}

#if !defined(BOOTSTRAP)
static void sx_map_call (struct tree_node *node, void *u)
{
    sexpr sx = (sexpr)node_get_value(node);

    gc_call (sx);
}

void sx_call_all ()
{
    tree_map (&sx_cons_tree,   sx_map_call, (void *)0);
    tree_map (&sx_string_tree, sx_map_call, (void *)0);
    tree_map (&sx_symbol_tree, sx_map_call, (void *)0);
}

void sx_tag_sub (sexpr sx)
{
    if (consp (sx))
    {
        sexpr sxcar = car (sx), sxcdr = cdr (sx);

        if (pointerp (sxcar))
        {
            gc_tag (sxcar);
        }

        if (pointerp (sxcdr))
        {
            gc_tag (sxcdr);
        }
    }
}
#endif
