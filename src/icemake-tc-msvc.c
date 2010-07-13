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
                           get_install_file (t, str_include), sx_nil), x);
    }

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        x = cons (sx_join (str_sI, sxcar, sx_nil), x);

        cur = cdr (cur);
    }

    return cons (sx_join (str_sIbuilds, architecture, str_sinclude), x);
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

static void build_object_msvc
    (sexpr type, sexpr source, sexpr target, struct target *t)
{
    if (truep(equalp(type, sym_link))) return;

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
}

static void build_object
    (sexpr desc, struct target *t)
{
    sexpr type   = car(desc);
    sexpr source = car(cdr(desc));
    sexpr target = car(cdr(cdr(desc)));

    build_object_msvc (type, source, target, t);
}

static int build   (struct target *t)
{
    sexpr c = t->code;

    while (consp (c))
    {
        build_object(car(c), t);

        c = cdr (c);
    }

    if (truep(do_tests))
    {
        c = t->test_cases;

        while (consp (c))
        {
            build_object(car(c), t);

            c = cdr (c);
        }
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

    /* parsing for x86 subtypes... gotta remember to do this properly one of
     * these days */
    if (((uname_arch[0] == 'x') || (uname_arch[0] == 'X')) &&
         (uname_arch[1] == '8') && (uname_arch[2] == '6')  &&
         (uname_arch[3] != 0))
    {
        if ((uname_arch[4] == '6') && (uname_arch[5] == '4') &&
            (uname_arch[6] == 0))
        {
            machine_spec = sx_join (str_smachinec, str_x64, sx_nil);
        }
        else if ((uname_arch[4] == '3') && (uname_arch[5] == '2') &&
                 (uname_arch[6] == 0))
        {
            machine_spec = sx_join (str_smachinec, str_x86, sx_nil);
        }
    }

    if (nilp (machine_spec))
    {
        machine_spec =
            sx_join (str_smachinec, make_string (uname_arch), sx_nil);
    }
        
    if (stringp (i_destdir))
    {
        switch (t->icemake->filesystem_layout)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                sx = cons (sx_join (str_sLIBPATHc, i_destdir,
                                    str_bslib), sx);
                break;
            case fs_afsl:
                sx = cons (sx_join (str_sLIBPATHc, i_destdir,
                           sx_join (str_backslash,
                                    make_string (t->toolchain->uname_os),
                           sx_join (str_backslash, make_string(uname_arch),
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
    char buffer[BUFFERSIZE];
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

    for (i = 0; (i < 6) && (uname_arch[i] == "x86-64"[i]); i++);

    sx = cons (((i == 6) ? str_sINCLUDEcmain : str_sINCLUDEcumain), sx);

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

    if (truep(do_tests))
    {
        sexpr s = t->test_cases;

        while (consp (s))
        {
            sexpr s1 = car(s);
            sexpr s2 = cdr(cdr(s1));
            sexpr s3 = car(s2);
            sexpr s4 = car(cdr(s2));
            sexpr s5 = cons(cons (car (s1), cons (s3, cons(s3,
                                  sx_end_of_list))), sx_end_of_list);

            link_programme_msvc_filename (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static void link_test_cases
    (sexpr name, sexpr code, struct target *t)
{
    if (truep(do_tests))
    {
        sexpr s = t->test_cases;

        while (consp (s))
        {
            sexpr s1 = car(s);
            sexpr s2 = cdr(cdr(s1));
            sexpr s3 = car(s2);
            sexpr s4 = car(cdr(s2));
            sexpr s5 = cons(cons (car (s1),
                            cons (s3, cons(s3, sx_end_of_list))),
                                 sx_end_of_list);

            link_programme_msvc_filename (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static int do_link (struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        if (falsep(i_dynamic_libraries) ||
            (t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            link_library_msvc (t->name, t->code, t);
        }

        if (truep(i_dynamic_libraries) &&
            !(t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            link_library_msvc_dynamic (t->name, t->code, t);
        }
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        sexpr b = get_build_file
            (t, (i_os == os_windows) ? sx_join (t->name, str_dot_exe, sx_nil)
                                     : t->name);

        link_programme_msvc_filename (b, t->name, t->code, t);
    }

    return 0;
}

static sexpr get_library_install_path (struct target *t)
{
    return get_install_file
        (t, sx_join (i_destlibdir, str_slash,
              sx_join (str_lib, t->name, str_dot_lib)));
}

static sexpr get_so_library_install_path (struct target *t)
{
    return get_install_file
        (t, sx_join (str_bin, str_slash,
              sx_join (str_lib, t->name,
                         sx_join (str_dot, t->dversion, str_dot_dll))));
}

static sexpr get_so_library_symlink_path (struct target *t)
{
    return get_install_file
        (t, sx_join (str_bin, str_slash,
              sx_join (str_lib, t->name, str_dot_dll)));
}

static sexpr get_programme_install_path (struct target *t)
{
    return get_install_file
        (t, sx_join (str_bin, str_slash,
              sx_join (t->name, str_dot_exe, sx_nil)));
}

static sexpr get_header_install_path
    (struct target *t, sexpr file)
{
    return get_install_file
        (t, sx_join (str_includes, t->name,
              sx_join (str_slash, file, str_dot_h)));
}

static sexpr get_data_install_path
    (struct target *t, sexpr name, sexpr file)
{
    switch (t->icemake->filesystem_layout)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            return sx_join (i_destdir, str_setcs,
                     sx_join (name, str_slash, file));
        case fs_afsl:
            return sx_join (i_destdir, str_sgenericsconfigurations,
                     sx_join (name, str_slash, file));
    }

    return sx_false;
}

static void install_library_msvc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                cons (sym_install,
                   cons (get_build_file (t, sx_join (str_lib, name,
                                                     str_dot_lib)),
                      get_library_install_path (t))));

    if (truep (i_dynamic_libraries) &&
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
                                cons (fname, get_so_library_install_path(t)))));

            t->icemake->workstack = sx_set_add (t->icemake->workstack,
                        cons (sym_install,
                          cons (get_build_file
                            (t, sx_join (str_lib, name,
                                         str_dot_dll)),
                          get_so_library_symlink_path (t))));
        }
    }
}

static void install_programme_msvc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
         cons (sym_install, cons (make_integer (0555),
               cons (get_build_file (t, sx_join (name, (i_os == os_windows 
                                                         ? str_dot_exe
                                                         : sx_nil), sx_nil)),
                     get_programme_install_path(t)))));
}

static void install_headers_msvc (sexpr name, struct target *t)
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

    install_headers_msvc (t->name, t);

    return 0;
}

static int test    (struct target *t)
{
    sexpr cur;

    for (cur = t->test_reference; consp (cur); cur = cdr (cur))
    {
        sexpr r = car (car (cur));

        unlink (sx_string (r));
    }

    for (cur = t->test_cases; consp (cur); cur = cdr (cur))
    {
        t->icemake->workstack =
            sx_set_add (t->icemake->workstack,
                        cdr(cdr(cdr(car(cur)))));
    }

    return 0;
}

int icemake_prepare_toolchain_msvc (struct toolchain_descriptor *td)
{
    td->meta_toolchain.msvc.cl   = icemake_which (td, "cl");
    td->meta_toolchain.msvc.rc   = icemake_which (td, "rc");
    td->meta_toolchain.msvc.link = icemake_which (td, "link");
    td->meta_toolchain.msvc.lib  = icemake_which (td, "lib");

    td->build   = build;
    td->link    = do_link;
    td->install = install;
    td->test    = test;

    return (falsep (td->meta_toolchain.msvc.cl)   ? 1 : 0) + 
           (falsep (td->meta_toolchain.msvc.rc)   ? 1 : 0) + 
           (falsep (td->meta_toolchain.msvc.link) ? 1 : 0) + 
           (falsep (td->meta_toolchain.msvc.lib)  ? 1 : 0);
}
