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

#include <curie/filesystem.h>

static void build_documentation_tex (sexpr file, sexpr base, struct target *t)
{
    sexpr dir = get_build_file (t, sx_nil),
          b   = sx_join (str_ddsddsdds, file, sx_nil);

    if (stringp (p_pdflatex))
    {
/*        sexpr tar = get_build_file (t, sx_join (base, str_dot_pdf, sx_nil));*/

        t->icemake->workstack
                = cons (cons (sym_chdir,
                              cons (dir,
                                    cons (p_pdflatex,
                                          cons (b,
                                                sx_end_of_list)))),
                        cons (cons (sym_chdir,
                                    cons (dir,
                                          cons (p_pdflatex,
                                                cons (b,
                                                      sx_end_of_list)))),
                              t->icemake->workstack));
    }
    else if (stringp (p_latex))
    {
/*        sexpr tar = get_build_file (t, sx_join (base, str_dot_dvi, sx_nil));*/

        t->icemake->workstack
                = cons (cons (sym_chdir,
                              cons (dir,
                                    cons (p_latex,
                                          cons (b,
                                                sx_end_of_list)))),
                        cons (cons (sym_chdir,
                                    cons (dir,
                                          cons (p_latex,
                                                cons (b,
                                                      sx_end_of_list)))),
                              t->icemake->workstack));
    }
}

static void build_documentation_doxygen (struct icemake *icemake)
{
    if (stringp (p_doxygen) && truep (filep (str_doxyfile)))
    {
        icemake->workstack = sx_set_add (icemake->workstack,
                          cons (p_doxygen, sx_end_of_list));
    }
}

static void do_build_documentation_target (struct target *t)
{
    sexpr c;

    for (c = t->documentation; consp(c); c = cdr (c))
    {
        sexpr ccar  = car (c);
        sexpr ccaar = car (ccar);
        sexpr ccdr  = cdr (ccar);
        sexpr ccddr = cdr (ccdr);
        sexpr ccdar = car (ccdr);

        if (truep(equalp(ccaar, sym_tex)))
        {
            build_documentation_tex (ccddr, ccdar, t);
        }
    }
}

int icemake_build_documentation (struct icemake *im)
{
    sexpr cursor = im->buildtargets;

    im->workstack =
        cons (cons (sym_phase, cons (sym_build_documentation, sx_end_of_list)),
              im->workstack);

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);
        struct tree_node *node =
            tree_get_node_string (&(im->targets), (char *)target);

        if (node != (struct tree_node *)0)
        {
            do_build_documentation_target (node_get_value(node));
        }

        cursor = cdr(cursor);
    }

    build_documentation_doxygen (im);
    
    im->workstack =
        cons (cons (sym_phase, cons (sym_completed, sx_end_of_list)),
              im->workstack);

    return 0;
}
