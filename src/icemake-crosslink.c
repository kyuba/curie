/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#include <curie/tree.h>

#include <stdlib.h>

#include <icemake/icemake.h>

static void do_cross_link (struct target *target, struct target *source)
{
    sexpr cur = source->code;

    while (consp (cur))
    {
        sexpr ccar = car (cur);
        sexpr cccdr = cdr (ccar);

        target->code = cons (cons (sym_link, cccdr), target->code);

        cur = cdr (cur);
    }
}

static void target_map_cross_link (struct tree_node *node, void *u)
{
    struct target *target = (struct target *) node_get_value (node);

    if (!eolp (target->use_objects))
    {
        sexpr cur = target->use_objects;

        while (consp (cur))
        {
            sexpr o = car (cur);
            struct tree_node *n1
                    = tree_get_node_string (&targets, (char *)sx_string (o));
            struct target *s;

            if (n1 == (struct tree_node *)0)
            {
                exit (68);
            }

            s = (struct target *) node_get_value (n1);

            do_cross_link (target, s);

            cur = cdr(cur);
        }
    }
}

void crosslink_objects ()
{
    sx_write (stdio, cons (sym_cross_link, sx_end_of_list));
    tree_map (&targets, target_map_cross_link, (void *)0);
}
