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

static sexpr get_documentation_install_path
    (struct target *t, sexpr name, sexpr file, sexpr version,
     const char *suffix)
{
    switch (t->icemake->filesystem_layout)
    {
        case fs_fhs:
            return sx_join (i_destdir, str_ssharesdocs,
                     sx_join (name, str_dash,
                       sx_join (version, str_slash,
                         sx_join (file, str_dot, make_string (suffix)))));
        case fs_afsl:
            return sx_join (i_destdir, str_sgenericsdocs,
                     sx_join (name, str_dash,
                       sx_join (version, str_slash,
                         sx_join (file, str_dot, make_string (suffix)))));
    }

    return sx_false;
}

static sexpr get_documentation_man_install_path
    (struct target *t, sexpr file, sexpr section)
{
    const char *s = sx_symbol (section);

    switch (t->icemake->filesystem_layout)
    {
        case fs_fhs:
            return sx_join (i_destdir, str_ssharesmans,
                     sx_join (make_string (s), str_slash,
                       sx_join (file, str_dot, make_string (s + 3))));
        case fs_afsl:
            return sx_join (i_destdir, str_sgenericsmans,
                     sx_join (make_string (s), str_slash,
                       sx_join (file, str_dot, make_string (s + 3))));
    }

    return sx_false;
}

static sexpr get_header_install_path
    (struct target *t, sexpr file)
{
    return get_install_file
        (t, sx_join (str_includes, t->name,
              sx_join (str_slash, file, str_dot_h)));
}

static void install_headers (sexpr name, struct target *t)
{
    sexpr c = t->headers;

    while (consp (c))
    {
        sexpr c2 = car(c);
        sexpr c3 = car(c2);
        sexpr c4 = car(cdr(c2));

        t->icemake->workstack = sx_set_add (t->icemake->workstack,
             cons (sym_install, cons (c4, get_header_install_path (t, c3))));

        c = cdr (c);
    }
}

static sexpr get_data_install_path
    (struct target *t, sexpr name, sexpr file)
{
    switch (t->icemake->filesystem_layout)
    {
        case fs_fhs:
            return sx_join (i_destdir, str_setcs,
                     sx_join (name, str_slash, file));
        case fs_afsl:
            return sx_join (i_destdir, str_sgenericsconfigurations,
                     sx_join (name, str_slash, file));
    }

    return sx_false;
}

static void install_support_files (sexpr name, struct target *t)
{
    sexpr cur = t->data, dname;

    if (truep(equalp(name, str_curie)))
    {
        t->icemake->workstack = sx_set_add (t->icemake->workstack,
                           cons (sym_install,
                             cons (get_build_file (t, str_libcuriedsx),
                                get_install_file (t, sx_join (i_destlibdir,
                                                  str_slibcuriedsx, sx_nil)))));
    }

    while (consp (cur))
    {
        sexpr item = car (cur);
        sexpr ccur = cdr (item);

        dname = car (item);

        while (consp(ccur))
        {
            sexpr s = car (ccur);

            t->icemake->workstack = sx_set_add (t->icemake->workstack,
                      cons (sym_install, cons (cdr (s),
                      get_data_install_path (t, dname, car (s)))));

            ccur = cdr (ccur);
        }

        cur = cdr (cur);
    }
}

static void install_documentation_with_suffix
    (sexpr name, struct target *t, sexpr c4, const char *suffix)
{
    sexpr fn = get_build_file (t, sx_join (c4, str_dot, make_string (suffix)));

    if (truep(filep(fn)))
    {
        t->icemake->workstack = sx_set_add (t->icemake->workstack,
                        cons (sym_install, cons (fn,
                              get_documentation_install_path
                                (t, name, c4, t->dversion, suffix))));
    }
}

static void install_documentation_man
    (sexpr name, struct target *t, sexpr file, sexpr abbr, sexpr section)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                      cons (sym_install, cons (file,
                            get_documentation_man_install_path
                                (t, abbr, section))));
}

static void install_documentation (sexpr name, struct target *t)
{
    sexpr c = t->documentation;

    while (consp (c))
    {
        sexpr c2 = car(c);
        sexpr c3 = cdr(c2);
        sexpr c4 = car(c3);

        if (truep(equalp(car(c2), sym_man)))
        {
            sexpr c5 = cdr(c3);
            sexpr filename = car(c5);
            sexpr c7 = cdr(c5);
            sexpr section = car(c7);

            install_documentation_man (name, t, filename, c4, section);
        }
        else
        {
            install_documentation_with_suffix (name, t, c4, "pdf");
            install_documentation_with_suffix (name, t, c4, "dvi");
            install_documentation_with_suffix (name, t, c4, "ps");
            install_documentation_with_suffix (name, t, c4, "eps");
            install_documentation_with_suffix (name, t, c4, "html");
        }
        c = cdr (c);
    }
}

static int do_install_target(struct target *t)
{
    if (!(t->options & ICEMAKE_TEST_CASE))
    {
        install_support_files (t->name, t);
        install_documentation (t->name, t);
        install_headers       (t->name, t);

        if (t->toolchain->install != (int (*)(struct target *))0)
        {
            return t->toolchain->install (t);
        }
    }

    return 0;
}

int icemake_install (struct icemake *im)
{
    sexpr cursor = im->buildtargets;

    if (falsep(i_destdir))
    {
        return 0;
    }

    im->workstack =
        cons (cons (sym_phase, cons (sym_install, sx_end_of_list)),
              im->workstack);

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);
        struct tree_node *node =
            tree_get_node_string (&(im->targets), (char *)target);

        if (node != (struct tree_node *)0)
        {
            do_install_target (node_get_value(node));
        }

        cursor = cdr(cursor);
    }

    im->workstack =
        cons (cons (sym_phase, cons (sym_completed, sx_end_of_list)),
              im->workstack);

    return 0;
}
