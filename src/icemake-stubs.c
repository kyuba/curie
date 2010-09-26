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

int icemake_build_documentation (struct icemake *im) { return 0; }
int icemake_run_tests (struct icemake *im) { return 0; }

int icemake_prepare_toolchain_generic (struct toolchain_descriptor *td)
{ return 0; }
int icemake_prepare_toolchain_borland (struct toolchain_descriptor *td)
{ return 0; }
int icemake_prepare_toolchain_msvc    (struct toolchain_descriptor *td)
{ return 0; }
int icemake_prepare_toolchain_latex   (struct toolchain_descriptor *td)
{ return 0; }
int icemake_prepare_toolchain_doxygen (struct toolchain_descriptor *td)
{ return 0; }

void gc_call (sexpr sx) {}
void initialise_stack () {}

static void target_map_prepare_archives (struct tree_node *node, void *u)
{
    struct target *context = (struct target *)node_get_value(node);
    sexpr c = context->code, d, da;

    while (consp (c))
    {
        d  = car (c);
        da = car (d);

        if (truep (equalp (da, sym_raw_c)) ||
            truep (equalp (da, sym_cpio_c)))
        {
            context->code = sx_set_remove (context->code, d);
        }

        c  = cdr (c);
    }
}

void icemake_prepare_archives (struct icemake *im)
{
    tree_map (&(im->targets), target_map_prepare_archives, (void *)im);
}

