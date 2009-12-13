/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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

#define _BSD_SOURCE

#include <curie/tree.h>
#include <curie/multiplex.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <icemake/icemake.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

static void write_curie_linker_flags_gcc (struct io *o, struct target *t)
{
    if (truep(t->use_curie))
    {
        if (truep(t->hosted))
        {
            switch (i_os)
            {
                case os_darwin:
                    break;
                default:
                    io_collect (o, " -nodefaultlibs -lc", 19);
                    break;
            }
        }
        else
        {
            if (truep(co_freestanding) && falsep(t->have_cpp))
            {
                io_collect (o, " -nodefaultlibs -nostartfiles -nostdlib", 47);

                switch (i_os)
                {
                    case os_darwin:
                        io_collect (o, " -e _start -lcurie-bootstrap", 28);
                        break;
                    default:
                        io_collect (o, " -u _start -lcurie-bootstrap", 28);
                        break;
                }
            }
        }
    }
}

static sexpr get_special_linker_options_common (sexpr sx)
{
    char *f = getenv ("LDFLAGS");

    if (f != (char *)0)
    {
        char buffer[BUFFERSIZE];
        int j = 0, i;
        sexpr t = sx_end_of_list;

        for (i = 0; f[i] != 0; i++)
        {
            if (f[i] == ' ')
            {
                buffer[j] = 0;

                t = cons (make_string (buffer), t);

                j = 0;
            }
            else
            {
                buffer[j] = f[i];
                j++;
            }
        }

        if (j != 0)
        {
            buffer[j] = 0;

            t = cons (make_string (buffer), t);
        }

        while (consp (t)) { sx = cons (car(t), sx); t = cdr (t); }
    }

    return sx;
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

    if (truep(i_optimise_linking))
    {
        sx = cons (str_Wlx, sx);

        switch (i_os)
        {
            case os_linux:
                sx = cons (str_Wls, cons (str_Wlznoexecstack, cons (str_Wlznorelro, cons (str_Wlgcsections, cons (str_Wlsortcommon, sx)))));
                break;
            default:
                break;
        }
    }

    if (truep(t->use_curie))
    {
        if (truep(t->hosted))
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
        else if (truep(co_freestanding) && falsep(t->have_cpp))
        {
            if (truep(i_static))
                sx = cons (str_static, sx);

            switch (i_os)
            {
                case os_darwin:
                    sx = cons (str_e, cons (str_start, sx));
                    break;
                default:
                    sx = cons (str_u, cons (str_start, sx));
                    break;
            }

            sx = cons (str_nostdlib, cons (str_nostartfiles, cons (str_nodefaultlibs, sx)));
        }
    }

    return sx;
}

static void map_includes (struct tree_node *node, void *psx)
{
    sexpr *sx = (sexpr *)psx;
    char buffer[BUFFERSIZE];
    struct target *t = node_get_value (node);

    switch (uname_toolchain)
    {
        case tc_gcc:
            snprintf (buffer, BUFFERSIZE, "-Lbuild/%s/%s", archprefix, sx_string(t->name));
            break;
        case tc_borland:
            snprintf (buffer, BUFFERSIZE, "-Lbuild/%s/%s", archprefix, sx_string(t->name));
            mangle_path_borland (buffer);
            break;
        case tc_msvc:
            snprintf (buffer, BUFFERSIZE, "/LIBPATH:build\\%s\\%s", archprefix, sx_string(t->name));
            break;
    }

    *sx = cons (make_string (buffer), *sx);
}

static sexpr get_special_linker_options (sexpr sx)
{
    define_string (str_multiply_defined, "-multiply_defined");
    define_string (str_warning,          "warning");
        
    if (stringp (i_destdir))
    {
        char buffer[BUFFERSIZE];

        switch (uname_toolchain)
        {
            case tc_msvc:
                switch (i_fsl)
                {
                    case fs_fhs:
                    case fs_fhs_binlib:
                        snprintf (buffer, BUFFERSIZE, "/LIBPATH:%s\\lib",
                                  sx_string(i_destdir));
                        break;
                    case fs_afsl:
                        snprintf (buffer, BUFFERSIZE, "/LIBPATH:%s\\%s\\%s\\lib",
                                  sx_string(i_destdir), uname_os, uname_arch);
                        break;
                }
                break;
            default:
                switch (i_fsl)
                {
                    case fs_fhs:
                    case fs_fhs_binlib:
                        snprintf (buffer, BUFFERSIZE, "-L%s/lib",
                                  sx_string(i_destdir));
                        break;
                    case fs_afsl:
                        snprintf (buffer, BUFFERSIZE, "-L%s/%s/%s/lib",
                                  sx_string(i_destdir), uname_os, uname_arch);
                        break;
                }
                break;
        }

        sx = cons (make_string (buffer), sx);
    }

    tree_map (&targets, map_includes, (void *)&sx);

    if (i_os == os_darwin)
    {
        sx = cons (str_multiply_defined, cons (str_warning, sx));
    }

    return get_special_linker_options_common (sx);
}

static sexpr collect_code
        (sexpr sx, sexpr code, char *havebin, struct stat *res)
{
    struct stat st;

    while (consp (code))
    {
        sexpr txn = car (code);
        sexpr ttype = car (txn);
        sexpr objectfile = car (cdr (cdr (txn)));

        if (truep(equalp(ttype, sym_assembly)) ||
            truep(equalp(ttype, sym_preproc_assembly)) ||
            truep(equalp(ttype, sym_c)) ||
            truep(equalp(ttype, sym_cpp)))
        {
            sx = cons (objectfile, sx);

            if ((*havebin) &&
                  (stat (sx_string(objectfile), &st) == 0) &&
                  (st.st_mtime > res->st_mtime))
            {
                *havebin = 0;
            }
        }

        code = cdr (code);
    }

    return sx;
}

static sexpr collect_code_pic
        (sexpr sx, sexpr code, char *havebin, struct stat *res)
{
    struct stat st;

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

            if ((*havebin) &&
                  (stat (sx_string(objectfile), &st) == 0) &&
                  (st.st_mtime > res->st_mtime))
            {
                *havebin = 0;
            }
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
                snprintf (buffer, BUFFERSIZE, "-l%s", sx_string (libname));
                break;
            case tc_borland:
                snprintf (buffer, BUFFERSIZE, "lib%s.lib", sx_string (libname));
                mangle_path_borland (buffer);
                break;
            case tc_msvc:
                snprintf (buffer, BUFFERSIZE, "lib%s.lib", sx_string (libname));
                break;
        }

        sx = cons (make_string (buffer), sx);

        cur = cdr (cur);
    }

    return sx;
}

static void link_programme_gcc_filename (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    struct stat res;
    char havebin;
    sexpr sx = sx_end_of_list;

    if (i_os != os_darwin)
    {
        sx = cons (str_dend_group, sx);
    }
    else if (truep (t->use_curie))
    {
        sx = cons (str_dlcurie, sx);
    }

    havebin = (stat (sx_string (ofile), &res) == 0);

    sx = collect_library_link_flags (sx, t);

    if (i_os != os_darwin)
    {
        sx = cons (str_dstart_group, sx);
    }

    sx = collect_code (sx, code, &havebin, &res);

    if (!havebin) {
        sx = (get_libc_linker_options_gcc (t,
                  get_special_linker_options (
                      cons (str_do,
                          cons (ofile,
                                sx)))));

        workstack = cons (cons (p_linker, sx), workstack);
    }
}

static void link_programme_borland_filename (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    struct stat res;
    char havebin;
    sexpr sx = sx_end_of_list;

    havebin = (stat (sx_string (ofile), &res) == 0);

    sx = collect_library_link_flags (sx, t);
    sx = collect_code (sx, code, &havebin, &res);

    if (!havebin) {
        sx = cons (str_dq,
                   get_special_linker_options (
                        cons (str_do,
                            cons (ofile,
                                  sx))));

        workstack = cons (cons (p_linker, sx), workstack);
    }
}

static void link_programme_msvc_filename (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    struct stat res;
    char havebin;
    int i;
    sexpr sx = get_special_linker_options (sx_end_of_list);

    for (i = 0; (i < 6) && (uname_arch[i] == "x86-64"[i]); i++);

    sx = /*cons (str_slink,*/ cons (((i == 6) ? str_sINCLUDEcmain : str_sINCLUDEcumain), sx)/*)*/;

    havebin = (stat (sx_string (ofile), &res) == 0);

    sx = collect_library_link_flags (sx, t);
    sx = collect_code (sx, code, &havebin, &res);

    if (!havebin) {
        snprintf (buffer, BUFFERSIZE, "/out:%s", sx_string (ofile));
        sx = cons (str_snologo,
                   cons (make_string (buffer),
                         sx));

        workstack = cons (cons (p_linker, sx), workstack);
    }
}

static void write_pc_file_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];
    sexpr cur;
    struct io *pcfile, *pcfile_hosted;

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.pc", archprefix, sx_string(t->name), sx_string(name));

    multiplex_add_io_no_callback (pcfile = io_open_create (buffer, 0644));

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s-hosted.pc", archprefix, sx_string(t->name), sx_string(name));

    multiplex_add_io_no_callback (pcfile_hosted = io_open_create (buffer, 0644));

    snprintf (buffer, BUFFERSIZE, "Name: %s\nDescription: %s\nVersion: %s\nURL: %s\nRequires:", sx_string (t->dname), sx_string (t->description), sx_string (t->dversion), sx_string (t->durl));

    io_collect (pcfile, buffer, strlen(buffer));
    io_collect (pcfile_hosted, buffer, strlen(buffer));

    cur = t->olibraries;
    while (consp (cur))
    {
        sexpr ca = car (cur);

        snprintf (buffer, BUFFERSIZE, " lib%s", sx_string (ca));
        io_collect (pcfile, buffer, strlen(buffer));

        snprintf (buffer, BUFFERSIZE, " lib%s-hosted", sx_string (ca));
        io_collect (pcfile_hosted, buffer, strlen(buffer));

        cur = cdr (cur);
    }

    snprintf (buffer, BUFFERSIZE, "\nConflicts:\nLibs:");

    io_collect (pcfile, buffer, strlen(buffer));
    if (truep(equalp(name, str_curie)))
    {
        write_curie_linker_flags_gcc (pcfile, t);
    }

    io_collect (pcfile_hosted, buffer, strlen(buffer));

    snprintf (buffer, BUFFERSIZE, " -l%s\nCflags: -ffreestanding\n", sx_string(t->name));

    io_collect (pcfile, buffer, strlen(buffer));
    io_collect (pcfile_hosted, buffer, strlen(buffer));
}

static void write_curie_sx (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    if (truep(equalp(name, str_curie)))
    {
        struct sexpr_io *io;

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.sx", archprefix, sx_string(t->name), sx_string(name));

        io = sx_open_o(io_open_create(buffer, 0644));

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

static void link_library_gcc (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    struct stat res;
    char havelib;
    sexpr sx = sx_end_of_list;

    write_pc_file_gcc (name, t);
    write_curie_sx (name, t);

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", archprefix, sx_string(t->name), sx_string(name));

    havelib = (stat (buffer, &res) == 0);

    sx = collect_code (sx, code, &havelib, &res);

    if (!havelib) {
        workstack
                = cons (cons (p_archiver,
                        cons (str_dr,
                              cons (str_ds,
                                    cons (str_dc,
                                          cons (make_string (buffer),
                                                sx))))),
                workstack);
    }

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

            link_programme_gcc_filename (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static void link_library_gcc_dynamic (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE], lbuffer[BUFFERSIZE];
    struct stat res;
    char havelib;
    sexpr sx = sx_end_of_list, cur;

    write_pc_file_gcc (name, t);
    write_curie_sx (name, t);

    if (i_os == os_windows)
    {
        cur = cons (str_kernel32, cons (str_mingw32, cons (str_coldname, cons (str_mingwex, cons (str_msvcrt, t->libraries)))));

        sx = cons (str_dend_group, sx);
        while (consp (cur))
        {
            sexpr libname = car (cur);

            if (falsep(equalp(str_curie_bootstrap, libname)) && falsep(equalp(name, libname)))
            {
                snprintf (buffer, BUFFERSIZE, "-l%s", sx_string (libname));

                sx = cons (make_string (buffer), sx);
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
            snprintf (buffer, BUFFERSIZE, "lib%s.%s.dll", sx_string(name), sx_string(t->dversion));
            snprintf (lbuffer, BUFFERSIZE, "build/%s/%s/lib%s.dll", archprefix, sx_string(t->name), sx_string(name));
            break;
        default:
            snprintf (buffer, BUFFERSIZE, "lib%s.so.%s", sx_string(name), sx_string(t->dversion));
            snprintf (lbuffer, BUFFERSIZE, "build/%s/%s/lib%s.so", archprefix, sx_string(t->name), sx_string(name));
            break;
    }

#if !defined(_WIN32)
    symlink (buffer, lbuffer);
#endif

    cur = t->olibraries;
    while (consp (cur))
    {
        sexpr ca = car (cur);

        /* make sure to link against the required libraries... */
        snprintf (buffer, BUFFERSIZE, "-l%s", sx_string (ca));
        sx = cons (make_string (buffer), sx);

        cur = cdr (cur);
    }

    if (i_os == os_linux)
    {
        snprintf (buffer, BUFFERSIZE, "-Wl,-soname,lib%s.so.%s", sx_string(name), sx_string(t->dversion));

        sx = cons (make_string (buffer), sx);
    }

    switch (i_os)
    {
        case os_windows:
            snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.%s.dll",
                      archprefix, sx_string(t->name), sx_string(name),
                      sx_string(t->dversion));
            break;
        default:
            snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.so.%s",
                      archprefix, sx_string(t->name), sx_string(name),
                      sx_string(t->dversion));
            break;
    }

    havelib = (stat (buffer, &res) == 0);

    sx = collect_code_pic (sx, code, &havelib, &res);

    sx = cons (str_nostdlib, cons (str_nostartfiles, cons (str_nodefaultlibs, sx)));

    if (!havelib) {
        workstack
                = cons (cons (p_linker,
                              cons (((i_os == os_darwin) ? str_ddynamiclib :
                                                           str_dshared),
                                    cons (str_dfpic,
                              cons (str_do, cons (make_string (buffer), sx))))),
                        workstack);

        if (i_os == os_windows)
        {
            snprintf (lbuffer, BUFFERSIZE, "build/%s/%s/lib%s.dll", archprefix, sx_string(t->name), sx_string(name));

            workstack
                    = cons (cons (p_linker,
                                  cons (str_dfpic,
                                  cons (str_do, cons (make_string (lbuffer), sx)))),
                            workstack);
        }
    }
}

static void link_library_borland (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    struct stat res;
    char havelib;
    sexpr sx = sx_end_of_list;

    write_curie_sx (name, t);

    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.lib", archprefix, sx_string(t->name), sx_string(name));
    mangle_path_borland (buffer);

    havelib = (stat (buffer, &res) == 0);

    sx = collect_code (sx, code, &havelib, &res);

    if (!havelib) {
        sexpr sxx = sx_end_of_list;

        while (consp (sx))
        {
            sxx = cons (str_plus, cons (car (sx), sxx));
            sx = cdr (sx);
        }

        workstack
                = cons (cons (p_archiver, cons (make_string (buffer), sxx)),
                        workstack);
    }

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

            link_programme_borland_filename (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static void link_library_borland_dynamic (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE], lbuffer[BUFFERSIZE];
    struct stat res;
    char havelib;
    sexpr sx = sx_end_of_list, cur;

    write_curie_sx (name, t);

    cur = t->libraries;
    while (consp (cur))
    {
        sexpr libname = car (cur);

        if (falsep(equalp(str_curie_bootstrap, libname)) && falsep(equalp(name, libname)))
        {
            snprintf (buffer, BUFFERSIZE, "lib%s.lib", sx_string (libname));
            mangle_path_borland (buffer);

            sx = cons (make_string (buffer), sx);
        }

        cur = cdr (cur);
    }

    sx = get_special_linker_options (sx);

    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.%s.dll", archprefix, sx_string(t->name), sx_string(name), sx_string(t->dversion));
    mangle_path_borland (buffer);

    havelib = (stat (buffer, &res) == 0);

    /* just collect_code(), because bcc doesn't use extra PIC objects */
    sx = collect_code (sx, code, &havelib, &res);

    if (!havelib) {
        workstack
                = cons (cons (p_linker,
                              cons (str_dq, cons (str_dWD,
                                    get_special_linker_options (
                                        cons (str_do, cons (make_string (buffer), sx)))))),
                        workstack);

        snprintf (lbuffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.dll", archprefix, sx_string(t->name), sx_string(name));
        mangle_path_borland (lbuffer);

        workstack
                = cons (cons (p_linker,
                              cons (str_dq, cons (str_dWD,
                                    get_special_linker_options (
                                        cons (str_do, cons (make_string (lbuffer), sx)))))),
                        workstack);
    }
}

static void link_library_msvc (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE], lbuffer[BUFFERSIZE];
    struct stat res;
    char havelib;
    sexpr sx = sx_end_of_list;

    write_curie_sx (name, t);

    snprintf (lbuffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.lib", archprefix, sx_string(t->name), sx_string(name));
    snprintf (buffer, BUFFERSIZE, "/OUT:%s", lbuffer);

    havelib = (stat (lbuffer, &res) == 0);

    sx = collect_code (sx, code, &havelib, &res);

    if (!havelib) {
        workstack
                = cons (cons (p_archiver,
                              cons (str_snologo,
                                cons (make_string (buffer), sx))),
                        workstack);
    }

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

            link_programme_msvc_filename (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static void link_library_msvc_dynamic (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE], sbuffer[BUFFERSIZE], ibuffer[BUFFERSIZE];
    struct stat res;
    char havelib;
    sexpr sx = get_special_linker_options (sx_end_of_list), cur;

    if (falsep (t->deffile))
    {
        link_library_msvc (name, code, t);
        return;
    }

    write_curie_sx (name, t);

    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.%s.dll", archprefix, sx_string(t->name), sx_string(name), sx_string (t->dversion));

    havelib = (stat (buffer, &res) == 0);

    /* just collect_code(), because msvc doesn't use extra PIC objects */
    sx = collect_code (sx, code, &havelib, &res);

    cur = t->olibraries;

    while (consp (cur))
    {
        sexpr libname = car (cur);

        snprintf (buffer, BUFFERSIZE, "lib%s.lib", sx_string (libname));

        sx = cons (make_string (buffer), sx);

        cur = cdr (cur);
    }

    sx = get_special_linker_options (sx);

    if (!havelib) {
        sexpr sxx = sx_end_of_list;

        while (consp (sx))
        {
/*            sxx = cons (str_plus, cons (car (sx), sxx));*/
            sxx = cons (car (sx), sxx);
            sx = cdr (sx);
        }

        snprintf (ibuffer, BUFFERSIZE, "/implib:build\\%s\\%s\\lib%s.lib", archprefix, sx_string(t->name), sx_string(name));
        snprintf (sbuffer, BUFFERSIZE, "/def:%s", sx_string(t->deffile));
        sxx = cons (make_string (sbuffer), sxx);

        snprintf (buffer, BUFFERSIZE, "/out:build\\%s\\%s\\lib%s.%s.dll", archprefix, sx_string(t->name), sx_string(name), sx_string (t->dversion));

        workstack
                = cons (cons (p_linker,
                              cons (str_snologo,
                                cons (str_sdll,
                                  cons (make_string (ibuffer),
                                  cons (make_string (buffer), sxx))))),
                        workstack);
    }

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

            link_programme_msvc_filename (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static void do_link_target(struct target *t)
{
    if (truep(t->library))
    {
        if ((uname_toolchain != tc_msvc) || falsep(i_dynamic_libraries))
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

        if (truep(i_dynamic_libraries))
        {
            switch (uname_toolchain)
            {
                case tc_gcc:
                    if ((i_os != os_windows) || falsep(t->have_cpp))
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

        if (!eolp(t->bootstrap))
        {
            char buffer[BUFFERSIZE];
            snprintf (buffer, BUFFERSIZE, "%s-bootstrap", sx_string(t->name));

            switch (uname_toolchain)
            {
                case tc_gcc:
                    link_library_gcc
                            (make_string(buffer), t->bootstrap, t); break;
                case tc_borland:
                    link_library_borland
                            (make_string(buffer), t->bootstrap, t); break;
                case tc_msvc:
                    link_library_msvc
                            (make_string(buffer), t->bootstrap, t); break;
            }
        }
    }
    else if (truep(t->programme))
    {
        char buffer[BUFFERSIZE];

        switch (i_os)
        {
            case os_windows:
                snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\%s.exe",
                          archprefix, sx_string(t->name), sx_string(t->name));
                break;
            default:
                snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s",
                          archprefix, sx_string(t->name), sx_string(t->name));
                break;
        }

        switch (uname_toolchain)
        {
            case tc_gcc:
                link_programme_gcc_filename
                        (make_string (buffer), t->name, t->code, t); break;
            case tc_borland:
                link_programme_borland_filename
                        (make_string (buffer), t->name, t->code, t); break;
            case tc_msvc:
                link_programme_msvc_filename
                        (make_string (buffer), t->name, t->code, t); break;
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
