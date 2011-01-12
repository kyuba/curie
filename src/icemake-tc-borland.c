/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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
#include <sievert/filesystem.h>

static sexpr prepend_includes_common (struct target *t, sexpr x)
{
    sexpr p = t->icemake->roots;
    sexpr include_paths = sx_end_of_list;
    sexpr cur;

    while (consp (p))
    {
        include_paths =
            sx_set_merge
                (include_paths, icemake_permutate_paths
                     (t->toolchain, sx_join (car(p), str_include, sx_nil)));
        p = cdr (p);
    }

    cur = path_normalise(include_paths);

    if (stringp (i_destdir))
    {
        x = cons (sx_join (str_dI,
                           icemake_decorate_file
                               (t, ft_header, fet_install_file, sx_nil),
                           sx_nil), x);
    }

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        x = cons (sx_join (str_dI, sxcar, sx_nil), x);

        cur = cdr (cur);
    }
        
    return cons (sx_join (str_dIbuilds, t->toolchain->uname, str_sinclude), x);
}

static sexpr prepend_includes_borland (struct target *t, sexpr x)
{
    return prepend_includes_common (t, x);
}

static sexpr prepend_cflags_borland (sexpr x)
{
    return prepend_flags_from_environment (x, "CFLAGS");
}

static sexpr prepend_cxxflags_borland (sexpr x)
{
    return prepend_flags_from_environment (x, "CXXFLAGS");
}

static void build_object_borland_generic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.borland.bcc32,
                  cons (str_dAT,
                  cons (str_dq,
                    cons (str_dw,
                      prepend_cflags_borland (
                      prepend_includes_borland (t,
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list)))))))))));
}

static void build_object_borland_cpp
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.borland.bcc32,
                  cons (str_dAT,
                  cons (str_dq,
                  cons (str_dP,
                    cons (str_dw,
                      prepend_cxxflags_borland (
                      prepend_includes_borland (t,
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list))))))))))));
}

static int build_object_borland
    (struct target *t, sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_resource)))
    {
        /* STUB */
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_borland_cpp (sx_string(source), sx_string(target), t);
    }
    else
    {
        build_object_borland_generic (sx_string(source), sx_string(target), t);
    }

    return 0;
}

static void map_includes (struct tree_node *node, void *psx)
{
    sexpr *sx = (sexpr *)psx;
    struct target *t = node_get_value (node);

    *sx = cons (sx_join (str_dL, icemake_decorate_file
                      (t, ft_other, fet_build_file, sx_nil), sx_nil), *sx);
}

static sexpr get_special_linker_options (struct target *t, sexpr sx)
{
    if (stringp (i_destdir))
    {
        switch (t->icemake->filesystem_layout)
        {
            case fs_fhs:
                sx = cons (sx_join (str_dL, i_destdir, str_slib), sx);
                break;
            case fs_afsl:
                sx = cons (sx_join (str_dL, i_destdir,
                           sx_join (str_slash,
                                    make_string (t->toolchain->uname_os),
                           sx_join (str_slash,
                                    make_string (t->toolchain->uname_arch),
                                    str_slib))), sx);
                break;
        }
    }

    tree_map (&(t->icemake->targets), map_includes, (void *)&sx);

    return prepend_flags_from_environment (sx, "LDFLAGS");
}

static sexpr collect_code (sexpr sx, sexpr code)
{
    while (consp (code))
    {
        sexpr txn = car (code);
        sexpr ttype = car (txn);
        sexpr objectfile = car (cdr (cdr (txn)));

        if (truep(equalp(ttype, sym_assembly)) ||
            truep(equalp(ttype, sym_preproc_assembly)) ||
            truep(equalp(ttype, sym_c)) ||
            truep(equalp(ttype, sym_cpp)) ||
            truep(equalp(ttype, sym_resource)))
        {
            sx = cons (objectfile, sx);
        }

        code = cdr (code);
    }

    return sx;
}

static sexpr collect_library_link_flags (sexpr sx, struct target *t)
{
    char buffer[BUFFERSIZE];
    sexpr cur = t->libraries;

    while (consp (cur))
    {
        sexpr libname = car (cur);

        sx = cons (sx_join (str_lib, libname, str_dot_lib), sx);
        mangle_path_borland (buffer);

        cur = cdr (cur);
    }

    return sx;
}

static void link_programme_borland_filename
    (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list;

    sx = collect_library_link_flags (sx, t);

    sx = cons (str_dq,
               get_special_linker_options (t,
                    cons (str_do,
                        cons (ofile,
                              collect_code (sx, code)))));

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                      cons (t->toolchain->meta_toolchain.borland.bcc32, sx));
}

static void write_curie_sx (sexpr name, struct target *t)
{
    if (truep(equalp(name, str_curie)))
    {
        sexpr b = icemake_decorate_file
                      (t, ft_library_options, fet_build_file, name);
        struct sexpr_io *io;

        io = sx_open_o (io_open_create (sx_string (b), 0644));

        if (t->toolchain->options & ICEMAKE_OPTION_FREESTANDING)
        {
            sx_write (io, sym_freestanding);
        }
        else
        {
            sx_write (io, sym_hosted);
        }

        multiplex_add_sexpr (io, (void *)0, (void *)0);
    }
}

static void link_library_borland (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list, sxx = sx_end_of_list,
          b = icemake_decorate_file (t, ft_static_library,
                                     fet_build_file, name);

    write_curie_sx (name, t);

    sx = collect_code (sx, code);

    while (consp (sx))
    {
        sxx = cons (str_plus, cons (car (sx), sxx));
        sx = cdr (sx);
    }

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                    cons (t->toolchain->meta_toolchain.borland.tlib,
                          cons (b, sxx)));
}

static void link_library_borland_dynamic
    (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list, cur, b;

    write_curie_sx (name, t);

    cur = t->libraries;
    while (consp (cur))
    {
        sexpr libname = car (cur);

        if (falsep(equalp(name, libname)))
        {
            sx = cons (mangle_path_borland_sx (sx_join (str_lib, libname,
                                                        str_dot_lib)), sx);
        }

        cur = cdr (cur);
    }

    sx = get_special_linker_options (t, sx);

    b = icemake_decorate_file (t, ft_shared_library_full, fet_build_file, name);

    /* just collect_code(), because bcc doesn't use extra PIC objects */
    sx = collect_code (sx, code);

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                    cons (t->toolchain->meta_toolchain.borland.bcc32,
                          cons (str_dq, cons (str_dWD,
                                get_special_linker_options (t,
                                    cons (str_do, cons (b, sx)))))));

    b = icemake_decorate_file (t, ft_shared_library, fet_build_file, name);

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                    cons (t->toolchain->meta_toolchain.borland.bcc32,
                          cons (str_dq, cons (str_dWD,
                                get_special_linker_options (t,
                                    cons (str_do, cons (b, sx)))))));
}

static int do_link (struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        link_library_borland (t->name, t->code, t);

        if ((t->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            !(t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
           link_library_borland_dynamic (t->name, t->code, t);
        }
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        sexpr b = icemake_decorate_file (t, ft_programme, fet_build_file,
                                         t->name);

        link_programme_borland_filename (b, t->name, t->code, t);
    }

    return 0;
}

static void install_library_dynamic_common (sexpr name, struct target *t)
{
    if ((t->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
        !(t->options & ICEMAKE_NO_SHARED_LIBRARY) &&
        (!(t->options & ICEMAKE_HAVE_CPP)))
    {
        sexpr fname = icemake_decorate_file
                        (t, ft_shared_library_full, fet_build_file, name);

        if (truep(filep(fname)))
        {
            t->icemake->workstack = sx_set_add (t->icemake->workstack,
                        cons (sym_install,
                            cons (make_integer(0555),
                                cons (fname,
                                  icemake_decorate_file
                                    (t, ft_shared_library_full,
                                     fet_install_file, t->name)))));

            t->icemake->workstack = sx_set_add (t->icemake->workstack,
                        cons (sym_install,
                            cons (icemake_decorate_file
                              (t, ft_shared_library, fet_build_file,
                               t->name),
                            icemake_decorate_file
                              (t, ft_shared_library, fet_install_file,
                               t->name))));
        }
    }
}

static void install_library_borland (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                cons (sym_install,
                   cons (icemake_decorate_file
                          (t, ft_static_library, fet_build_file, name),
                      icemake_decorate_file
                        (t, ft_static_library, fet_install_file, name))));

    install_library_dynamic_common (name, t);
}

static void install_programme_borland (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
         cons (sym_install, cons (make_integer (0555),
               cons (icemake_decorate_file
                       (t, ft_programme, fet_build_file, t->name),
                     icemake_decorate_file
                       (t, ft_programme, fet_install_file, t->name)))));
}


static int install (struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        install_library_borland (t->name, t);
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        install_programme_borland (t->name, t);
    }

    return 0;
}

int icemake_prepare_toolchain_borland (struct toolchain_descriptor *td)
{
    td->meta_toolchain.borland.bcc32 = icemake_which (td, "bcc32", "BCC32");
    td->meta_toolchain.borland.tlib  = icemake_which (td, "tlib",  "TLIB");

    td->build_object = build_object_borland;
    td->link         = do_link;
    td->install      = install;

    return (falsep (td->meta_toolchain.borland.bcc32) ? 1 : 0) + 
           (falsep (td->meta_toolchain.borland.tlib)  ? 1 : 0);
}
