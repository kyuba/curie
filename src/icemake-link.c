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
#include <curie/multiplex.h>

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

    if (truep(i_optimise_linking))
    {
        sx = cons (str_Wlx, sx);

        switch (i_os)
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
        if (t->options & ICEMAKE_HOSTED)
        {
            switch (i_os)
            {
                case os_darwin:
                    break;
                default:
                    sx = cons (str_nodefaultlibs, sx);
                    break;
            }

        }
        else if (truep(co_freestanding))
        {
            if (truep(i_static))
            {
                sx = cons (str_static, sx);
            }

            switch (i_os)
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

    *sx = cons (sx_join ((uname_toolchain == tc_msvc ? str_sLIBPATHc : str_dL),
                         get_build_file (t, sx_nil), sx_nil), *sx);
}

static sexpr get_special_linker_options (sexpr sx)
{
    define_string (str_multiply_defined, "-multiply_defined");
    define_string (str_warning,          "warning");
        
    if (stringp (i_destdir))
    {
        switch (uname_toolchain)
        {
            case tc_msvc:
                switch (i_fsl)
                {
                    case fs_fhs:
                    case fs_fhs_binlib:
                        sx = cons (sx_join (str_sLIBPATHc, i_destdir,
                                            str_bslib), sx);
                        break;
                    case fs_afsl:
                        sx = cons (sx_join (str_sLIBPATHc, i_destdir,
                               sx_join (str_backslash, make_string (uname_os),
                                 sx_join (str_backslash, make_string(uname_arch),
                                   str_bslib))), sx);
                        break;
                }
                break;
            default:
                switch (i_fsl)
                {
                    case fs_fhs:
                    case fs_fhs_binlib:
                        sx = cons (sx_join (str_dL, i_destdir, str_slib), sx);
                        break;
                    case fs_afsl:
                        sx = cons (sx_join (str_dL, i_destdir,
                              sx_join (str_slash, make_string (uname_os),
                                sx_join (str_slash, make_string (uname_arch),
                                  str_slib))), sx);
                        break;
                }
                break;
        }
    }

    tree_map (&targets, map_includes, (void *)&sx);

    if (i_os == os_darwin)
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
    char buffer[BUFFERSIZE];
    sexpr cur = t->libraries;

    while (consp (cur))
    {
        sexpr libname = car (cur);

        switch (uname_toolchain)
        {
            case tc_gcc:
                sx = cons (sx_join (str_dl, libname, sx_nil), sx);
                break;
            case tc_borland:
                sx = cons (sx_join (str_lib, libname, str_dot_lib), sx);
                mangle_path_borland (buffer);
                break;
            case tc_msvc:
                sx = cons (sx_join (str_lib, libname, str_dot_lib), sx);
                break;
        }

        cur = cdr (cur);
    }

    return sx;
}

static void link_programme_gcc_filename
    (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list;

    if (i_os != os_darwin)
    {
        sx = cons (str_dend_group, sx);
    }

    sx = collect_library_link_flags (sx, t);

    if (i_os != os_darwin)
    {
        sx = cons (str_dstart_group, sx);
    }

    sx = (get_libc_linker_options_gcc (t,
              get_special_linker_options (
                  cons (str_do,
                      cons (ofile,
                            collect_code (sx, code))))));

    workstack = cons (cons (p_linker, sx), workstack);
}

static void link_programme_borland_filename
    (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list;

    sx = collect_library_link_flags (sx, t);

    sx = cons (str_dq,
               get_special_linker_options (
                    cons (str_do,
                        cons (ofile,
                              collect_code (sx, code)))));

    workstack = cons (cons (p_linker, sx), workstack);
}

static void link_programme_msvc_filename
    (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    int i;
    sexpr sx = get_special_linker_options (sx_end_of_list);

    for (i = 0; (i < 6) && (uname_arch[i] == "x86-64"[i]); i++);

    sx = cons (((i == 6) ? str_sINCLUDEcmain : str_sINCLUDEcumain), sx);

    sx = collect_library_link_flags (sx, t);

    sx = cons (str_snologo,
               cons (sx_join (str_soutc, ofile, sx_nil),
                     collect_code (sx, code)));

    workstack = cons (cons (p_linker, sx), workstack);
}

static void write_curie_sx (sexpr name, struct target *t)
{
    if (truep(equalp(name, str_curie)))
    {
        sexpr b = get_build_file (t, sx_join (str_lib, name, str_dot_sx));
        struct sexpr_io *io;

        io = sx_open_o (io_open_create (sx_string (b), 0644));

        if (truep(co_freestanding))
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

static void link_test_cases
    (sexpr name, sexpr code, struct target *t,
     void (*f) (sexpr, sexpr, sexpr, struct target *))
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
            sexpr s5 = cons(cons (car (s1), cons (s3, cons(s3, sx_end_of_list))), sx_end_of_list);

            f (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static void link_library_gcc (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list,
          b = get_build_file (t, sx_join (str_lib, name, str_dot_a));

    write_curie_sx (name, t);

    workstack
            = cons (cons (p_archiver,
                    cons (str_dr,
                          cons (str_ds,
                                cons (str_dc,
                                      cons (b, collect_code (sx, code)))))),
            workstack);
}

static void link_library_gcc_dynamic (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list, cur, lname, lshort;

    write_curie_sx (name, t);

    if (i_os == os_windows)
    {
        cur = cons (str_kernel32, cons (str_mingw32, cons (str_coldname, cons (str_mingwex, cons (str_msvcrt, t->libraries)))));

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
        sx = get_special_linker_options (cons (str_dstart_group, sx));
    }
    else
    {
        sx = get_special_linker_options (sx);
    }

    switch (i_os)
    {
        case os_windows:
            lshort = sx_join (str_lib, name,
                       sx_join (str_dot, t->dversion, str_dot_dll));
            lname  = get_build_file (t, sx_join (str_lib, name, str_dot_dll));
            break;
        default:
            lshort = sx_join (str_lib, name,
                       sx_join (str_dot_so_dot, t->dversion, sx_nil));
            lname  = get_build_file (t, sx_join (str_lib, name, str_dot_so));
    }

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

    if (i_os == os_linux)
    {
        sx = cons (sx_join (str_dWlcdsonameclib, name,
                     sx_join (str_dot_so_dot, t->dversion, sx_nil)), sx);
    }

    lshort = get_build_file (t, lshort);

    sx = collect_code_pic (sx, code);

    sx = cons (str_nostdlib,
               cons (str_nostartfiles, cons (str_nodefaultlibs, sx)));

    workstack
            = cons (cons (p_linker,
                          cons (((i_os == os_darwin) ? str_ddynamiclib :
                                                       str_dshared),
                                cons (str_dfpic,
                          cons (str_do, cons (lshort, sx))))),
                    workstack);

    if (i_os == os_windows)
    {
        workstack
                = cons (cons (p_linker,
                              cons (str_dfpic,
                              cons (str_do, cons (lname, sx)))),
                        workstack);
    }
}

static void link_library_borland (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list,
          b = get_build_file (t, sx_join (str_lib, name, str_dot_lib));

    write_curie_sx (name, t);

    sx = collect_code (sx, code);

    sexpr sxx = sx_end_of_list;

    while (consp (sx))
    {
        sxx = cons (str_plus, cons (car (sx), sxx));
        sx = cdr (sx);
    }

    workstack = cons (cons (p_archiver, cons (b, sxx)), workstack);
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

    sx = get_special_linker_options (sx);

    b = get_build_file (t, sx_join (str_lib, name,
                             sx_join (str_dot, t->dversion, str_dot_dll)));

    /* just collect_code(), because bcc doesn't use extra PIC objects */
    sx = collect_code (sx, code);

    workstack
            = cons (cons (p_linker,
                          cons (str_dq, cons (str_dWD,
                                get_special_linker_options (
                                    cons (str_do, cons (b, sx)))))),
                    workstack);

    b = get_build_file (t, sx_join (str_lib, name, str_dot_dll));

    workstack
            = cons (cons (p_linker,
                          cons (str_dq, cons (str_dWD,
                                get_special_linker_options (
                                    cons (str_do, cons (b, sx)))))),
                    workstack);
}

static void link_library_msvc (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = sx_end_of_list,
          b = sx_join (str_soutc,
                       get_build_file (t, sx_join (str_lib, name, str_dot_lib)),
                       sx_nil);

    write_curie_sx (name, t);

    sx = collect_code (sx, code);

    workstack = cons (cons (p_archiver, cons (str_snologo, cons (b, sx))),
                      workstack);
}

static void link_library_msvc_dynamic (sexpr name, sexpr code, struct target *t)
{
    sexpr sx = get_special_linker_options (sx_end_of_list), cur,
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

    sx = get_special_linker_options (sx);

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

    workstack
            = cons (cons (p_linker,
                          cons (str_snologo,
                            cons (str_sdll,
                              cons (bi,
                              cons (sx_join (str_soutc, b, sx_nil), sxx))))),
                    workstack);

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

static void do_link_target(struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            link_test_cases (t->name, t->code, t,
                             link_programme_gcc_filename);     break;
        case tc_borland:
            link_test_cases (t->name, t->code, t,
                             link_programme_borland_filename); break;
        case tc_msvc:
            link_test_cases (t->name, t->code, t,
                             link_programme_msvc_filename);    break;
    }

    if (t->options & ICEMAKE_LIBRARY)
    {
        if ((uname_toolchain != tc_msvc) || falsep(i_dynamic_libraries) ||
            (t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            switch (uname_toolchain)
            {
                case tc_gcc:
                    link_library_gcc     (t->name, t->code, t); break;
                case tc_borland:
                    link_library_borland (t->name, t->code, t); break;
                case tc_msvc:
                    link_library_msvc    (t->name, t->code, t); break;
            }
        }

        if (truep(i_dynamic_libraries) &&
            !(t->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            switch (uname_toolchain)
            {
                case tc_gcc:
                    if ((i_os != os_windows) ||
                        !(t->options & ICEMAKE_HAVE_CPP))
                    {
                        link_library_gcc_dynamic (t->name, t->code, t);
                    }
                    break;
                case tc_borland:
                    link_library_borland_dynamic (t->name, t->code, t); break;
                case tc_msvc:
                    link_library_msvc_dynamic    (t->name, t->code, t); break;
            }
        }
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        sexpr b = get_build_file
            (t, (i_os == os_windows) ? sx_join (t->name, str_dot_exe, sx_nil)
                                     : t->name);

        switch (uname_toolchain)
        {
            case tc_gcc:
                link_programme_gcc_filename     (b, t->name, t->code, t); break;
            case tc_borland:
                link_programme_borland_filename (b, t->name, t->code, t); break;
            case tc_msvc:
                link_programme_msvc_filename    (b, t->name, t->code, t); break;
        }
    }
}

static void link_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_link_target (node_get_value(node));
    }
}

static void target_map_link (struct tree_node *node, void *u)
{
    do_link_target(node_get_value(node));
}

void ice_link (sexpr buildtargets)
{
    sexpr cursor = buildtargets;
    sx_write (stdio, cons (sym_phase, cons (sym_link, sx_end_of_list)));

    if (eolp(cursor))
    {
        tree_map (&targets, target_map_link, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        link_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}
