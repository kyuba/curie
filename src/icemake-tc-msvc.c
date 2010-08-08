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
#include <curie/filesystem.h>

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

static void write_curie_sx (sexpr name, struct target *t)
{
    if (truep(equalp(name, str_curie)))
    {
        sexpr b = get_build_file (t, sx_join (str_lib, name, str_dot_sx));
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

static sexpr prepend_includes_msvc (struct target *t, sexpr x)
{
    sexpr include_paths = icemake_permutate_paths (t->toolchain, str_include);
    sexpr cur = include_paths;

    if (stringp (i_destdir))
    {
        x = cons (sx_join (str_sI,
                           icemake_decorate_file
                               (t, ft_header, fet_install_file, sx_nil),
                           sx_nil), x);
    }

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        x = cons (sx_join (str_sI, sxcar, sx_nil), x);

        cur = cdr (cur);
    }

    return cons (sx_join (str_sIbuilds, t->toolchain->uname, str_sinclude), x);
}

static sexpr prepend_cflags_msvc (sexpr x)
{
    return prepend_flags_from_environment (x, "CFLAGS");
}

static sexpr prepend_cxxflags_msvc (sexpr x)
{
    return prepend_flags_from_environment (x, "CXXFLAGS");
}

static void build_object_msvc_generic
    (const char *source, const char *target, struct target *t)
{
    sexpr b = sx_join (str_sFo, make_string (target), sx_nil);

    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
                        cons (t->toolchain->meta_toolchain.msvc.cl,
                      cons (str_sc,
                      cons (str_snologo,
                      cons (str_sTC,
                        prepend_cflags_msvc (
                        prepend_includes_msvc (t,
                          cons (make_string (source),
                              cons (b, sx_end_of_list)))))))));
}

static void build_object_msvc_cpp
    (const char *source, const char *target, struct target *t)
{
    sexpr b = sx_join (str_sFo, make_string (target), sx_nil);

    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
                        cons (t->toolchain->meta_toolchain.msvc.cl,
                      cons (str_sc,
                      cons (str_snologo,
                      cons (str_sTP,
                      cons (str_sEHsc,
                        prepend_cxxflags_msvc (
                        prepend_includes_msvc (t,
                          cons (make_string (source),
                              cons (b, sx_end_of_list))))))))));
}

static void build_object_msvc_resource
    (const char *source, const char *target, struct target *t)
{
    sexpr b = sx_join (str_sfo, make_string (target), sx_nil);

    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
                        cons (t->toolchain->meta_toolchain.msvc.rc,
                      cons (b, cons (make_string(source), sx_end_of_list))));
}

static int build_object_msvc
    (struct target *t, sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_resource)))
    {
        build_object_msvc_resource (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_msvc_cpp (sx_string(source), sx_string(target), t);
    }
    else
    {
        build_object_msvc_generic (sx_string(source), sx_string(target), t);
    }

    return 0;
}

static void map_includes (struct tree_node *node, void *psx)
{
    sexpr *sx = (sexpr *)psx;
    struct target *t = node_get_value (node);

    *sx = cons (sx_join (str_sLIBPATHc,
                         get_build_file (t, sx_nil), sx_nil), *sx);
}

static sexpr get_special_linker_options (struct target *t, sexpr sx)
{
    define_string (str_smachinec,        "/machine:");
    define_string (str_x86,              "x86");
    define_string (str_x64,              "x64");
    sexpr machine_spec = sx_nil;

    if (t->toolchain->instruction_set == is_x86)
    {
        if (t->toolchain->instruction_set_options & IS_32_BIT)
        {
            machine_spec = sx_join (str_smachinec, str_x86, sx_nil);
        }
        else if (t->toolchain->instruction_set_options & IS_64_BIT)
        {
            machine_spec = sx_join (str_smachinec, str_x64, sx_nil);
        }
    }

    if (nilp (machine_spec))
    {
        machine_spec =
            sx_join (str_smachinec,
                     make_string (t->toolchain->uname_arch),
                     sx_nil);
    }
        
    if (stringp (i_destdir))
    {
        switch (t->icemake->filesystem_layout)
        {
            case fs_fhs:
                sx = cons (sx_join (str_sLIBPATHc, i_destdir,
                                    str_bslib), sx);
                break;
            case fs_afsl:
                sx = cons (sx_join (str_sLIBPATHc, i_destdir,
                           sx_join (str_backslash,
                                    make_string (t->toolchain->uname_os),
                           sx_join (str_backslash,
                                    make_string(t->toolchain->uname_arch),
                                    str_bslib))), sx);
                break;
        }
    }

    tree_map (&(t->icemake->targets), map_includes, (void *)&sx);
        
    sx = cons (machine_spec, sx);

    return prepend_flags_from_environment (sx, "LDFLAGS");
}

static sexpr collect_library_link_flags (sexpr sx, struct target *t)
{
    sexpr cur = t->libraries;

    while (consp (cur))
    {
        sexpr libname = car (cur);

        sx = cons (sx_join (str_lib, libname, str_dot_lib), sx);

        cur = cdr (cur);
    }

    return sx;
}

static void link_programme_msvc_filename
    (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    int i;
    sexpr sx = get_special_linker_options (t, sx_end_of_list);

    sx = cons ((((t->toolchain->instruction_set == is_x86) &&
                 (t->toolchain->instruction_set_options & IS_64_BIT)) ?
                str_sINCLUDEcmain : str_sINCLUDEcumain), sx);

    sx = collect_library_link_flags (sx, t);

    sx = cons (str_snologo,
               cons (sx_join (str_soutc, ofile, sx_nil),
                     collect_code (sx, code)));

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                       cons (t->toolchain->meta_toolchain.msvc.link, sx));
}

static void link_library_msvc (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list,
          b = sx_join (str_soutc,
                       get_build_file (t, sx_join (str_lib, name, str_dot_lib)),
                       sx_nil);

    write_curie_sx (name, t);

    sx = collect_code (sx, code);

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                       cons (t->toolchain->meta_toolchain.msvc.lib,
                               cons (str_snologo, cons (b, sx))));
}

static void link_library_msvc_dynamic (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = get_special_linker_options (t, sx_end_of_list), cur,
          sxx = sx_end_of_list, b, bi;

    if (falsep (t->deffile))
    {
        link_library_msvc (name, code, t);
        return;
    }

    write_curie_sx (name, t);

    b = get_build_file (t, sx_join (str_lib, name,
                             sx_join (str_dot, t->dversion, str_dot_dll)));

    /* just collect_code(), because msvc doesn't use extra PIC objects */
    sx = collect_code (sx, code);

    cur = t->olibraries;

    while (consp (cur))
    {
        sexpr libname = car (cur);

        sx = cons (sx_join (str_lib, libname, str_dot_lib), sx);

        cur = cdr (cur);
    }

    while (consp (sx))
    {
/*        sxx = cons (str_plus, cons (car (sx), sxx));*/
        sxx = cons (car (sx), sxx);
        sx = cdr (sx);
    }

    bi = sx_join (str_simplibc,
                  get_build_file (t, sx_join (str_lib, name, str_dot_lib)),
                  sx_nil);

    sxx = cons (sx_join (str_sdefc, t->deffile, sx_nil), sxx);

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                    cons (t->toolchain->meta_toolchain.msvc.link,
                          cons (str_snologo,
                            cons (str_sdll,
                              cons (bi,
                                cons (sx_join (str_soutc, b, sx_nil), sxx))))));
}

static int do_link (struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        if (!(t->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) ||
            (t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            link_library_msvc (t->name, t->code, t);
        }

        if ((t->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            !(t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            link_library_msvc_dynamic (t->name, t->code, t);
        }
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        sexpr b = get_build_file
            (t, sx_join (t->name, str_dot_exe, sx_nil));

        link_programme_msvc_filename (b, t->name, t->code, t);
    }

    return 0;
}

static void install_library_msvc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                cons (sym_install,
                   cons (get_build_file (t, sx_join (str_lib, name,
                                                     str_dot_lib)),
                      icemake_decorate_file
                        (t, ft_static_library, fet_install_file, t->name))));

    if ((t->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
        !(t->options & ICEMAKE_NO_SHARED_LIBRARY))
    {
        sexpr fname;

        fname = get_build_file
                  (t, sx_join (str_lib, name,
                        sx_join (str_dot, t->dversion, str_dot_dll)));

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
                          cons (get_build_file
                            (t, sx_join (str_lib, name,
                                         str_dot_dll)),
                          icemake_decorate_file
                            (t, ft_shared_library, fet_install_file,
                             t->name))));
        }
    }
}

static void install_programme_msvc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
         cons (sym_install, cons (make_integer (0555),
               cons (get_build_file (t, sx_join (name, str_dot_exe, sx_nil)),
                     icemake_decorate_file
                       (t, ft_programme, fet_install_file, t->name)))));
}

static int install (struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        install_library_msvc (t->name, t);
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        install_programme_msvc (t->name, t);
    }

    return 0;
}

int icemake_prepare_toolchain_msvc (struct toolchain_descriptor *td)
{
    td->meta_toolchain.msvc.cl   = icemake_which (td, "cl",   "CL");
    td->meta_toolchain.msvc.rc   = icemake_which (td, "rc",   "RC");
    td->meta_toolchain.msvc.link = icemake_which (td, "link", "LINK");
    td->meta_toolchain.msvc.lib  = icemake_which (td, "lib",  "LIB");

    td->build_object = build_object_msvc;
    td->link         = do_link;
    td->install      = install;

    return (falsep (td->meta_toolchain.msvc.cl)   ? 1 : 0) + 
           (falsep (td->meta_toolchain.msvc.rc)   ? 1 : 0) + 
           (falsep (td->meta_toolchain.msvc.link) ? 1 : 0) + 
           (falsep (td->meta_toolchain.msvc.lib)  ? 1 : 0);
}
