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

#include <sievert/tree.h>
#include <curie/filesystem.h>

#include <stdlib.h>
#include <stdio.h>

static void build_documentation_tex (sexpr file, sexpr base, struct target *t)
{
    char dirbuffer[BUFFERSIZE];
    char buffer[BUFFERSIZE];

    snprintf (dirbuffer, BUFFERSIZE, "build/%s/%s", archprefix, sx_string(t->name));
    snprintf (buffer,    BUFFERSIZE, "../../../%s", sx_string(file));

    if (stringp (p_pdflatex))
    {
        char tarbuffer[BUFFERSIZE];
        struct stat sst, tst;

        snprintf (tarbuffer, BUFFERSIZE, "%s/%s.pdf", dirbuffer, sx_string(base));

        if ((stat (sx_string (file), &sst) == 0) &&
            (stat (tarbuffer, &tst) == 0) &&
            (tst.st_mtime > sst.st_mtime)) return;

        workstack
                = cons (cons (sym_chdir,
                              cons (make_string (dirbuffer),
                                    cons (p_pdflatex,
                                          cons (make_string(buffer),
                                                sx_end_of_list)))),
                        cons (cons (sym_chdir,
                                    cons (make_string (dirbuffer),
                                          cons (p_pdflatex,
                                                cons (make_string(buffer),
                                                      sx_end_of_list)))),
                              workstack));
    }
    else if (stringp (p_latex))
    {
        char tarbuffer[BUFFERSIZE];
        struct stat sst, tst;

        snprintf (tarbuffer, BUFFERSIZE, "%s/%s.dvi", dirbuffer, sx_string(base));

        if ((stat (sx_string (file), &sst) == 0) &&
            (stat (tarbuffer, &tst) == 0) &&
            (tst.st_mtime > sst.st_mtime)) return;

        workstack
                = cons (cons (sym_chdir,
                              cons (make_string (dirbuffer),
                                    cons (p_latex,
                                          cons (make_string(buffer),
                                                sx_end_of_list)))),
                        cons (cons (sym_chdir,
                                    cons (make_string (dirbuffer),
                                          cons (p_latex,
                                                cons (make_string(buffer),
                                                      sx_end_of_list)))),
                              workstack));
    }
}

static void build_documentation_doxygen ()
{
    if (stringp (p_doxygen) && truep (filep (str_doxyfile)))
    {
        workstack = cons (cons (p_doxygen,
                                sx_end_of_list),
                          workstack);
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

static void target_map_build_documentation (struct tree_node *node, void *u)
{
    do_build_documentation_target(node_get_value(node));
}

static void build_documentation_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_build_documentation_target (node_get_value(node));
    }
}

void build_documentation (sexpr buildtargets)
{
    sexpr cursor = buildtargets;

    sx_write (stdio, cons (sym_phase, cons (sym_build_documentation, sx_end_of_list)));

    if (eolp(cursor))
    {
        tree_map (&targets, target_map_build_documentation, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        build_documentation_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    build_documentation_doxygen ();

    loop_processes();
}
