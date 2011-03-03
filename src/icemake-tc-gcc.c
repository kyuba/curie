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

static sexpr prepend_includes_gcc (struct target *t, sexpr x)
{
    return prepend_includes_common (t, x);
}

static sexpr prepend_ccflags_gcc (struct toolchain_descriptor *td, sexpr x)
{
    if (td->options & ICEMAKE_OPTION_FREESTANDING)
    {
        switch (td->operating_system)
        {
            case os_darwin:
                x = cons (str_static, x);
                break;
            default:
                break;
        }
    }

    return x;
}

static sexpr prepend_cflags_gcc (struct toolchain_descriptor *td, sexpr x)
{
    define_string (str_ffreestanding, "-ffreestanding");

    x = prepend_flags_from_environment (x, "CFLAGS");

    if (td->options & ICEMAKE_OPTION_FREESTANDING)
    {
        x = cons (str_ffreestanding, x);
    }

    return prepend_ccflags_gcc(td, x);
}

static sexpr prepend_cxxflags_gcc (struct target *t, sexpr x)
{
    define_string (str_dfno_exceptions, "-fno-exceptions");
    define_string (str_dfno_rtti,       "-fno-rtti");

    x = prepend_flags_from_environment (x, "CXXFLAGS");

    x = cons (str_dfno_rtti, cons (str_dfno_exceptions, x));

    return prepend_ccflags_gcc(t->toolchain, x);
}

static void build_object_gcc_assembly
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.as,
                    cons (make_string (source),
                      cons (str_do,
                        cons (make_string(target), sx_end_of_list)))));
}

static void build_object_gcc_preproc_assembly
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
                  prepend_includes_gcc (t,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list)))))));
}

static void build_object_gcc_c
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                       cons (str_dWnounknownpragma,
                        prepend_cflags_gcc (t->toolchain,
                        prepend_includes_gcc (t,
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target),
                                  sx_end_of_list)))))))))))));
}

static void build_object_gcc_c_combine
    (sexpr sources, const char *target, struct target *t)
{
    sexpr item = cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                        cons (str_dWnounknownpragma,
                         prepend_cflags_gcc (t->toolchain,
                         prepend_includes_gcc (t,
                           cons (str_do,
                            cons (make_string (target), sx_end_of_list)))))))));
    sexpr cur, sx;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    sx = cons (str_dcombine, cons (str_dc, item));

    if ((t->options & ICEMAKE_PROGRAMME))
    {
        sexpr c = t->code;
        char allow_whole_program = 1;

        while (!eolp (c))
        {
            sexpr ca = car (c);
            sexpr type = car (ca);

            if (falsep (equalp (type, sym_resource)) &&
                falsep (equalp (type, sym_c)) &&
                falsep (equalp (type, sym_c_pic)))
            {
                allow_whole_program = 0;
                break;
            }

            c = cdr (c);
        }

        c = t->libraries;

        while (!eolp (c))
        {
            sexpr ca = car (c);

            if (falsep (equalp (ca, str_gcc)))
            {
                allow_whole_program = 0;
                break;
            }

            c = cdr (c);
        }

        if (allow_whole_program == 1)
        {
            sx = cons (str_dfwhole_program, sx);
        }
    }

    item = cons (t->toolchain->meta_toolchain.gcc.gcc, sx);

    t->icemake->workstack = sx_set_add (t->icemake->workstack, item);
}

static void build_object_gcc_cpp
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gpp,
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (t,
                    prepend_includes_gcc (t,
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list)))))))));
}

static void build_object_gcc_assembly_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
                  prepend_includes_gcc (t,
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))));
}

static void build_object_gcc_preproc_assembly_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
                  prepend_includes_gcc (t,
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))));
}

static void build_object_gcc_c_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
                  cons (str_dfpic,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                       cons (str_dWnounknownpragma,
                        prepend_cflags_gcc (t->toolchain,
                        prepend_includes_gcc (t,
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target),
                                  sx_end_of_list))))))))))))));
}

static void build_object_gcc_c_pic_combine
    (sexpr sources, const char *target, struct target *t)
{
    sexpr item = cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                        cons (str_dWnounknownpragma,
                         prepend_cflags_gcc (t->toolchain,
                         prepend_includes_gcc (t,
                           cons (str_do,
                            cons (make_string (target), sx_end_of_list)))))))));
    sexpr cur, sx;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    sx = cons (str_dcombine, cons (str_dc, item));

    if ((t->options & ICEMAKE_PROGRAMME))
    {
        sexpr c = t->code;
        char allow_whole_program = 1;

        while (!eolp (c))
        {
            sexpr ca = car (c);
            sexpr type = car (ca);

            if (falsep (equalp (type, sym_resource)) &&
                falsep (equalp (type, sym_c)) &&
                falsep (equalp (type, sym_c_pic)))
            {
                allow_whole_program = 0;
                break;
            }

            c = cdr (c);
        }

        c = t->libraries;

        while (!eolp (c))
        {
            sexpr ca = car (c);

            if (falsep (equalp (ca, str_gcc)))
            {
                allow_whole_program = 0;
                break;
            }

            c = cdr (c);
        }

        if (allow_whole_program == 1)
        {
            sx = cons (str_dfwhole_program, sx);
        }
    }

    item = cons (t->toolchain->meta_toolchain.gcc.gcc, cons (str_dfpic, sx));

    t->icemake->workstack = sx_set_add (t->icemake->workstack, item);
}

static void build_object_gcc_cpp_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gpp,
                  cons (str_dfpic,
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (t,
                    prepend_includes_gcc (t,
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list))))))))));
}

static int build_object_gcc
    (struct target *t, sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_resource)))
    {
        /* STUB */
    }
    else if (truep(equalp(type, sym_assembly)))
    {
        build_object_gcc_assembly
            (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_preproc_assembly)))
    {
        build_object_gcc_preproc_assembly
            (sx_string(source),sx_string(target), t);
    }
    else if (truep(equalp(type, sym_c)))
    {
        if (consp (source))
        {
            build_object_gcc_c_combine
                (source, sx_string(target), t);
        }
        else
        {
            build_object_gcc_c (sx_string(source), sx_string(target), t);
        }
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_gcc_cpp (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_assembly_pic)))
    {
        build_object_gcc_assembly_pic
            (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_preproc_assembly_pic)))
    {
        build_object_gcc_preproc_assembly_pic
            (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_c_pic)))
    {
        if (consp (source))
        {
            build_object_gcc_c_pic_combine
                (source, sx_string(target), t);
        }
        else
        {
            build_object_gcc_c_pic
                (sx_string(source), sx_string(target), t);
        }
    }
    else if (truep(equalp(type, sym_cpp_pic)))
    {
        build_object_gcc_cpp_pic
            (sx_string(source), sx_string(target), t);
    }
    else
    {
        t->icemake->visualiser.on_error
            (t->icemake, ie_unknown_code_file_type, type);
    }

    return 0;
}

static sexpr get_libc_linker_options_gcc (struct target *t, sexpr sx)
{
    define_string (str_u,                "-u");
    define_string (str_e,                "-e");
    define_string (str_start,            "_start");
    define_string (str_Wlx,              "-Wl,-x");
    define_string (str_Wls,              "-Wl,-s");
    define_string (str_Wlznoexecstack,   "-Wl,-z,noexecstack");
    define_string (str_Wlznorelro,       "-Wl,-z,norelro");
    define_string (str_Wlgcsections,     "-Wl,--gc-sections");
    define_string (str_Wlsortcommon,     "-Wl,--sort-common");

    if (t->icemake->options & ICEMAKE_OPTION_OPTIMISE_LINKING)
    {
        sx = cons (str_Wlx, sx);

        switch (t->toolchain->operating_system)
        {
            case os_linux:
                sx = cons (str_Wls, cons (str_Wlznoexecstack,
                           cons (str_Wlznorelro, cons (str_Wlgcsections,
                                 cons (str_Wlsortcommon, sx)))));
                break;
            default:
                break;
        }
    }

    if (t->options & ICEMAKE_USE_CURIE)
    {
        if (t->options & ICEMAKE_TEST_CASE)
        {
            sx = cons (str_static, sx);
        }

        if (t->options & ICEMAKE_HOSTED)
        {
            switch (t->toolchain->operating_system)
            {
                case os_darwin:
                    break;
                default:
                    sx = cons (str_nodefaultlibs, sx);
                    break;
            }

        }
        else if (t->toolchain->options & ICEMAKE_OPTION_FREESTANDING)
        {
            if ((t->icemake->options & ICEMAKE_OPTION_STATIC) &&
                !(t->options & ICEMAKE_TEST_CASE))
            {
                sx = cons (str_static, sx);
            }

            switch (t->toolchain->operating_system)
            {
                case os_darwin:
                    sx = cons (str_e, cons (str_start, sx));
                    break;
                default:
                    sx = cons (str_u, cons (str_start, sx));
                    break;
            }

            sx = cons (str_nostdlib, cons (str_nostartfiles,
                   cons (str_nodefaultlibs, sx)));
        }
    }

    return sx;
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
    define_string (str_multiply_defined, "-multiply_defined");
    define_string (str_warning,          "warning");
 
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

    if (t->toolchain->operating_system == os_darwin)
    {
        sx = cons (str_multiply_defined, cons (str_warning, sx));
    }

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

static sexpr collect_code_pic (sexpr sx, sexpr code)
{
    while (consp (code))
    {
        sexpr txn = car (code);
        sexpr ttype = car (txn);
        sexpr objectfile = car (cdr (cdr (txn)));

        if (truep(equalp(ttype, sym_assembly_pic)) ||
            truep(equalp(ttype, sym_preproc_assembly_pic)) ||
            truep(equalp(ttype, sym_c_pic)) ||
            truep(equalp(ttype, sym_cpp_pic)))
        {
            sx = cons (objectfile, sx);
        }

        code = cdr (code);
    }

    return sx;
}

static sexpr collect_library_link_flags (sexpr sx, struct target *t)
{
    sexpr cur = t->libraries;

    while (consp (cur))
    {
        sexpr libname = car (cur);

        sx = cons (sx_join (str_dl, libname, sx_nil), sx);

        cur = cdr (cur);
    }

    return sx;
}

static void link_programme_gcc_filename
    (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list;

    if (t->toolchain->operating_system != os_darwin)
    {
        sx = cons (str_dend_group, sx);
    }

    sx = collect_library_link_flags (sx, t);

    if (t->toolchain->operating_system != os_darwin)
    {
        sx = cons (str_dstart_group, sx);
    }

    sx = (get_libc_linker_options_gcc (t,
              get_special_linker_options (t,
                  cons (str_do,
                      cons (ofile,
                            collect_code (sx, code))))));

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                      cons (t->toolchain->meta_toolchain.gcc.gcc, sx));
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

static void link_library_gcc (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list,
          b = icemake_decorate_file
                (t, ft_static_library, fet_build_file, name);

    write_curie_sx (name, t);

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                    cons (t->toolchain->meta_toolchain.gcc.ar,
                    cons (str_dr,
                          cons (str_ds,
                                cons (str_dc,
                                      cons (b, collect_code (sx, code)))))));
}

static void link_library_gcc_dynamic (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list, cur, lname, lshort;

    write_curie_sx (name, t);

    if (t->toolchain->operating_system == os_windows)
    {
        cur = cons (str_kernel32, cons (str_mingw32, cons (str_coldname,
                    cons (str_mingwex, cons (str_msvcrt, t->libraries)))));

        sx = cons (str_dend_group, sx);
        while (consp (cur))
        {
            sexpr libname = car (cur);

            if (falsep(equalp(name, libname)))
            {
                sx = cons (sx_join (str_dl, libname, sx_nil), sx);
            }

            cur = cdr (cur);
        }
        sx = get_special_linker_options (t, cons (str_dstart_group, sx));
    }
    else
    {
        sx = get_special_linker_options (t, sx);
    }

    lshort = icemake_decorate_file
                (t, ft_shared_library_full, fet_decorate_only, name);
    lname = icemake_decorate_file (t, ft_shared_library, fet_build_file, name);

#if !defined(_WIN32)
    symlink (sx_string (lshort), sx_string (lname));
#endif

    cur = t->olibraries;
    while (consp (cur))
    {
        sexpr ca = car (cur);

        /* make sure to link against the required libraries... */
        sx = cons (sx_join (str_dl, ca, sx_nil), sx);

        cur = cdr (cur);
    }

    if (t->toolchain->operating_system == os_linux)
    {
        sx = cons (sx_join (str_dWlcdsonameclib, name,
                     sx_join (str_dot_so_dot, t->dversion, sx_nil)), sx);
    }

    lshort = icemake_decorate_file
                (t, ft_shared_library_full, fet_build_file, name);

    sx = collect_code_pic (sx, code);

    sx = cons (str_nostdlib,
               cons (str_nostartfiles, cons (str_nodefaultlibs, sx)));

    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                    cons (t->toolchain->meta_toolchain.gcc.gcc,
                          cons (((t->toolchain->operating_system
                                       == os_darwin) ? str_ddynamiclib :
                                                       str_dshared),
                                cons (str_dfpic,
                          cons (str_do, cons (lshort, sx))))));

    if (t->toolchain->operating_system == os_windows)
    {
        t->icemake->workstack = sx_set_add (t->icemake->workstack,
                        cons (t->toolchain->meta_toolchain.gcc.gcc,
                              cons (str_dfpic,
                              cons (str_do, cons (lname, sx)))));
    }
}

static int do_link (struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        link_library_gcc (t->name, t->code, t);

        if ((t->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            !(t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            if ((t->toolchain->operating_system != os_windows) ||
                !(t->options & ICEMAKE_HAVE_CPP))
            {
                link_library_gcc_dynamic (t->name, t->code, t);
            }
        }
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        sexpr b = icemake_decorate_file
                    (t, ft_programme, fet_build_file, t->name);

        link_programme_gcc_filename (b, t->name, t->code, t);
    }

    return 0;
}

static void install_library_dynamic_common (sexpr name, struct target *t)
{
    if ((t->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
        !(t->options & ICEMAKE_NO_SHARED_LIBRARY) &&
        (!(t->options & ICEMAKE_HAVE_CPP) ||
         (t->toolchain->operating_system != os_darwin)))
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

            switch (t->toolchain->operating_system)
            {
                case os_windows:
                    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                                cons (sym_install,
                                  cons (icemake_decorate_file
                                            (t, ft_shared_library,
                                             fet_build_file, name),
                                  icemake_decorate_file
                                      (t, ft_shared_library,
                                       fet_install_file, name))));
                    break;
                default:
                    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                                cons (sym_symlink,
                                      cons (icemake_decorate_file
                                                (t, ft_shared_library,
                                                 fet_decorate_only, name),
                                            icemake_decorate_file
                                                (t, ft_shared_library,
                                                 fet_install_file, t->name))));
            }
        }
    }
}

static void install_library_gcc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                cons (sym_install,
                   cons (icemake_decorate_file
                           (t, ft_static_library, fet_build_file, name),
                      icemake_decorate_file
                        (t, ft_static_library, fet_install_file, name))));

    install_library_dynamic_common (name, t);
}

static void install_programme_gcc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
         cons (sym_install, cons (make_integer (0555),
               cons (icemake_decorate_file
                         (t, ft_programme, fet_build_file, name),
                     icemake_decorate_file
                         (t, ft_programme, fet_install_file, name)))));
}

static int install (struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        install_library_gcc (t->name, t);
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        install_programme_gcc (t->name, t);
    }

    return 0;
}

int icemake_prepare_toolchain_gcc (struct toolchain_descriptor *td)
{
    td->meta_toolchain.gcc.gcc = icemake_which (td, "gcc", "CC");
    td->meta_toolchain.gcc.ld  = icemake_which (td, "ld",  "LD");
    td->meta_toolchain.gcc.gpp = icemake_which (td, "g++", "GXX");
    td->meta_toolchain.gcc.as  = icemake_which (td, "as",  "AS");
    td->meta_toolchain.gcc.ar  = icemake_which (td, "ar",  "AR");

    td->build_object = build_object_gcc;
    td->link         = do_link;
    td->install      = install;

    return (falsep (td->meta_toolchain.gcc.gcc) ? 1 : 0) + 
           (falsep (td->meta_toolchain.gcc.ld)  ? 1 : 0) + 
           (falsep (td->meta_toolchain.gcc.gpp) ? 1 : 0) + 
           (falsep (td->meta_toolchain.gcc.as)  ? 1 : 0) +
           (falsep (td->meta_toolchain.gcc.ar)  ? 1 : 0);
}
