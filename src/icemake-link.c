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

#include <icemake/icemake.h>

#include <curie/multiplex.h>

int icemake_link (struct icemake *im)
{
    int rv = 0;
    sexpr cursor = im->buildtargets;

    im->workstack =
        cons (cons (sym_phase, cons (sym_link, sx_end_of_list)),
              im->workstack);

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string(sxcar);
        struct tree_node *node =
            tree_get_node_string (&(im->targets), (char *)target);

        if (node != (struct tree_node *)0)
        {
            struct target *t = (struct target *)node_get_value(node);
            if (t->toolchain->link != (int (*)(struct target *))0)
            {
                rv += t->toolchain->link (t);
            }
            else
            {
                rv += 1;
            }
        }

        cursor = cdr(cursor);
    }
    
    im->workstack =
        cons (cons (sym_phase, cons (sym_completed, sx_end_of_list)),
              im->workstack);

    return rv;
}
