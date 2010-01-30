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

#define _BSD_SOURCE

#include <sievert/tree.h>
#include <curie/multiplex.h>
#include <curie/filesystem.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>

#include <icemake/icemake.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

static int files_open                  = 0;

static void mkdir_p (sexpr path)
{
    char buffer[BUFFERSIZE];
    struct stat st;
    const char *p = sx_string (path);
    int i;

    for (i = 0; (p[i] != 0) && (i < (BUFFERSIZE - 2)); i++)
    {
        if (((p[i] == '/') || (p[i] == '\\')) && (i > 0))
        {
            buffer[i] = 0;

            if (stat(buffer, &st) != 0)
            {
                mkdir (buffer, 0777);
            }
        }

        buffer[i] = p[i];
    }
}

static void install_read (struct io *in, void *aux)
{
    struct io *out = (struct io *)aux;

    if (((in->length) - (in->position)) > 0)
    {
        io_write (out,
                  (in->buffer) + (in->position),
                  (in->length) - (in->position));

        in->position = in->length;
    }
}

#if defined(_WIN32)
/* windows comes with dedicated file copy functions, so let's use those... */
static void loop_install()
{
    struct stat st;
    int fo = 0;
    sx_write (stdio, cons (sym_phase, cons (sym_install, sx_end_of_list)));
    count_print_items();

    more_files:

    while (consp (workstack))
    {
        sexpr spec = car (workstack);
        sexpr source = car (spec);
        sexpr target = cdr (spec);
        struct io *in, *out;

        if (truep(equalp(source, sym_symlink))) /* no symlinks on windows */
        {
/*            source = car (target);
            target = cdr (target);

            sx_write (stdio, cons (sym_symlink,
                      cons (source, cons (target, sx_end_of_list))));

            symlink (sx_string(source), sx_string(target));*/
        }
        else if (stat (sx_string (source), &st) == 0)
        {
            sx_write (stdio, cons (sym_install,
                      cons (source, cons (target, sx_end_of_list))));

            mkdir_p (target);
            CopyFile (sx_string (source), sx_string (target), FALSE);
        }

        workstack = cdr(workstack);
    }

    sx_write (stdio, cons (sym_phase, cons (sym_completed, sx_end_of_list)));
}

#else
static void install_close (struct io *in, void *aux)
{
    struct io *out = (struct io *)aux;

    if (((in->length) - (in->position)) > 0)
    {
        io_write (out,
                  (in->buffer) + (in->position),
                  (in->length) - (in->position));

        in->position = in->length;
    }

    multiplex_del_io (out);

    files_open--;
}

static void loop_install()
{
    struct stat st;
    sx_write (stdio, cons (sym_phase, cons (sym_install, sx_end_of_list)));
    int fo = 0;
    count_print_items();

    more_files:

    while (consp (workstack))
    {
        sexpr spec = car (workstack);
        sexpr source = car (spec);
        sexpr target = cdr (spec);
        struct io *in, *out;

        if (truep(equalp(source, sym_symlink)))
        {
            source = car (target);
            target = cdr (target);

            sx_write (stdio, cons (sym_symlink,
                      cons (source, cons (target, sx_end_of_list))));

            symlink (sx_string(source), sx_string(target));
        }
        else if (stat (sx_string (source), &st) == 0)
        {
            sx_write (stdio, cons (sym_install,
                      cons (source, cons (target, sx_end_of_list))));

            mkdir_p (target);

            files_open++;

            in  = io_open_read   (sx_string (source));
            out = io_open_create (sx_string (target), st.st_mode);

            multiplex_add_io (in, install_read, install_close, (void *)out);
            multiplex_add_io_no_callback (out);
        }

        workstack = cdr(workstack);
        
        if (files_open >= MAX_FILES_IN_PARALLEL)
        {
            break;
        }
    }

    while (files_open > 0)
    {
        if (fo != files_open)
        {
            fo = files_open;
            sx_write (stdio, cons (sym_items_remaining,
                      cons (make_integer (files_open), sx_end_of_list)));
        }
        
        if ((files_open < MAX_FILES_IN_PARALLEL) && consp (workstack))
        {
            goto more_files;
        }

        multiplex();
    }

    sx_write (stdio, cons (sym_phase, cons (sym_completed, sx_end_of_list)));
}
#endif

static sexpr get_library_install_path (sexpr name)
{
    char buffer[BUFFERSIZE];

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            switch (uname_toolchain)
            {
                case tc_msvc:
                case tc_borland:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\lib%s.lib", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name));
                    break;
                case tc_gcc:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/lib%s.a", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name));
                    break;
            }
            return make_string (buffer);
            break;
        case fs_afsl:
            switch (uname_toolchain)
            {
                case tc_msvc:
                case tc_borland:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\%s\\lib\\lib%s.lib", sx_string(i_destdir), uname_os, uname_arch, sx_string(name));
                    break;
                case tc_gcc:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s.a", sx_string(i_destdir), uname_os, uname_arch, sx_string(name));
                    break;
            }
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static sexpr get_so_library_install_path (sexpr name, sexpr version)
{
    char buffer[BUFFERSIZE];

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            switch (i_os)
            {
                case os_windows:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\lib%s.%s.dll", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name), sx_string(version));
                    mangle_path_borland (buffer);
                    break;
                default:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/lib%s.so.%s", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name), sx_string(version));
                    break;
            }
            return make_string (buffer);
            break;
        case fs_afsl:
            switch (i_os)
            {
                case os_windows:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\%s\\bin\\lib%s.%s.dll", sx_string(i_destdir), uname_os, uname_arch, sx_string(name), sx_string(version));
                    mangle_path_borland (buffer);
                    break;
                default:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s.so.%s", sx_string(i_destdir), uname_os, uname_arch, sx_string(name), sx_string(version));
                    break;
            }
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static sexpr get_so_library_symlink_path (sexpr name)
{
    char buffer[BUFFERSIZE];

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            switch (i_os)
            {
                case os_windows:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\lib%s.dll", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name));
                    mangle_path_borland (buffer);
                    break;
                default:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/lib%s.so", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name));
                    break;
            }
            return make_string (buffer);
            break;
        case fs_afsl:
            switch (i_os)
            {
                case os_windows:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\%s\\lib\\lib%s.dll", sx_string(i_destdir), uname_os, uname_arch, sx_string(name));
                    mangle_path_borland (buffer);
                    break;
                default:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s.so", sx_string(i_destdir), uname_os, uname_arch, sx_string(name));
                    break;
            }
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static sexpr get_programme_install_path (sexpr name)
{
    char buffer[BUFFERSIZE];

    switch (i_os)
    {
        case os_windows:
            switch (i_fsl)
            {
                case fs_fhs:
                    snprintf (buffer, BUFFERSIZE, "%s\\bin\\%s.exe", sx_string(i_destdir), sx_string(name));
                    return make_string (buffer);
                    break;
                case fs_fhs_binlib:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\%s\\bin\\%s.exe", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(i_pname), sx_string(name));
                    return make_string (buffer);
                    break;
                case fs_afsl:
                    snprintf (buffer, BUFFERSIZE, "%s\\%s\\%s\\bin\\%s.exe",
                              sx_string(i_destdir), uname_os, uname_arch,
                                        sx_string(name));
                    return make_string (buffer);
                    break;
            }
            break;
        default:
            switch (i_fsl)
            {
                case fs_fhs:
                    snprintf (buffer, BUFFERSIZE, "%s/bin/%s", sx_string(i_destdir), sx_string(name));
                    return make_string (buffer);
                    break;
                case fs_fhs_binlib:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/%s/bin/%s", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(i_pname), sx_string(name));
                    return make_string (buffer);
                    break;
                case fs_afsl:
                    snprintf (buffer, BUFFERSIZE, "%s/%s/%s/bin/%s",
                              sx_string(i_destdir), uname_os, uname_arch,
                                        sx_string(name));
                    return make_string (buffer);
                    break;
            }
    }

    return sx_false;
}

static sexpr get_documentation_install_path (sexpr name, sexpr file, sexpr version, const char *suffix)
{
    char buffer[BUFFERSIZE];

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            snprintf (buffer, BUFFERSIZE, "%s/share/doc/%s-%s/%s.%s",
                      sx_string(i_destdir), sx_string(name), sx_string(version),
                      sx_string (file), suffix);
            return make_string (buffer);
            break;
        case fs_afsl:
            snprintf (buffer, BUFFERSIZE, "%s/generic/doc/%s-%s/%s.%s",
                      sx_string(i_destdir), sx_string(name),
                      sx_string (version), sx_string (file), suffix);
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static sexpr get_documentation_man_install_path (sexpr file, sexpr section)
{
    char buffer[BUFFERSIZE];
    const char *s = sx_symbol (section);

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            snprintf (buffer, BUFFERSIZE, "%s/share/man/%s/%s.%c",
                      sx_string(i_destdir), s, sx_string(file), s[3]);
            return make_string (buffer);
            break;
        case fs_afsl:
            snprintf (buffer, BUFFERSIZE, "%s/generic/man/%s/%s.%c",
                      sx_string(i_destdir), s, sx_string(file), s[3]);
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static sexpr get_header_install_path (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            snprintf (buffer, BUFFERSIZE, "%s/include/%s/%s.h", sx_string(i_destdir), sx_string(name), sx_string (file));
            return make_string (buffer);
            break;
        case fs_afsl:
            snprintf (buffer, BUFFERSIZE, "%s/%s/%s/include/%s/%s.h", sx_string(i_destdir), uname_os, uname_arch, sx_string(name), sx_string (file));
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static sexpr get_data_install_path (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            snprintf (buffer, BUFFERSIZE, "%s/etc/%s/%s", sx_string(i_destdir), sx_string(name), sx_string (file));
            return make_string (buffer);
            break;
        case fs_afsl:
            snprintf (buffer, BUFFERSIZE, "%s/generic/configuration/%s/%s", sx_string(i_destdir), sx_string(name), sx_string (file));
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static void install_library_dynamic_common (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    if (truep (i_dynamic_libraries) &&
        (falsep (t->have_cpp) || (i_os != os_darwin)))
    {
        sexpr fname;

        switch (i_os)
        {
            case os_windows:
                snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.%s.dll", archprefix, sx_string(t->name), sx_string(name), sx_string(t->dversion));
                mangle_path_borland (buffer);
                break;
            default:
                snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.so.%s", archprefix, sx_string(t->name), sx_string(name), sx_string(t->dversion));
                break;
        }

        fname = make_string(buffer);

        if (truep(filep(fname)))
        {
            workstack
                    = cons (cons (make_string (buffer), get_so_library_install_path(name, t->dversion)),
                    workstack);

            switch (i_os)
            {
                case os_windows:
                    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.dll", archprefix, sx_string(t->name), sx_string(name));
                    mangle_path_borland (buffer);

                    workstack
                            = cons (cons (make_string (buffer), get_so_library_symlink_path(name)),
                                    workstack);
                    break;
                default:
                    snprintf (buffer, BUFFERSIZE, "lib%s.so.%s", sx_string(name), sx_string(t->dversion));
                    workstack
                            = cons (cons (sym_symlink, cons (make_string (buffer),
                                          get_so_library_symlink_path(name))),
                                    workstack);
                    break;
            }
        }
    }
}

static void install_library_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", archprefix, sx_string(t->name), sx_string(name));

    workstack
        = cons (cons (make_string (buffer),
                      get_library_install_path(name)),
                workstack);

    install_library_dynamic_common (name, t);
}

static void install_library_borland (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.lib", archprefix, sx_string(t->name), sx_string(name));
    mangle_path_borland (buffer);

    workstack
        = cons (cons (make_string (buffer),
                      get_library_install_path(name)),
                workstack);

    install_library_dynamic_common (name, t);
}

static void install_library_msvc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\lib%s.lib", archprefix, sx_string(t->name), sx_string(name));

    workstack
        = cons (cons (make_string (buffer),
                      get_library_install_path(name)),
                workstack);

    install_library_dynamic_common (name, t);
}

static void install_programme_common (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    switch (i_os)
    {
        case os_windows:
            snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s.exe", archprefix, sx_string(t->name), sx_string(name));
            break;
        default:
            snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s", archprefix, sx_string(t->name), sx_string(name));
    }

    workstack
        = cons (cons (make_string (buffer),
                      get_programme_install_path(name)), workstack);
}

static void install_programme_gcc (sexpr name, struct target *t)
{
    install_programme_common (name, t);
}

static void install_programme_borland (sexpr name, struct target *t)
{
    install_programme_common (name, t);
}

static void install_programme_msvc (sexpr name, struct target *t)
{
    install_programme_common (name, t);
}

static void install_headers_common (sexpr name, struct target *t)
{
    sexpr c = t->headers;

    while (consp (c))
    {
        sexpr c2 = car(c);
        sexpr c3 = car(c2);
        sexpr c4 = car(cdr(c2));

        workstack = cons (cons (c4, get_header_install_path(name, c3)),
                          workstack);

        c = cdr (c);
    }
}

static void install_headers_gcc (sexpr name, struct target *t)
{
    install_headers_common (name, t);
}

static void install_headers_borland (sexpr name, struct target *t)
{
    install_headers_common (name, t);
}

static void install_headers_msvc (sexpr name, struct target *t)
{
    install_headers_common (name, t);
}

static void install_support_files (sexpr name, struct target *t)
{
    sexpr cur = t->data, dname;

    if (truep(equalp(name, str_curie)))
    {
        char buffer[BUFFERSIZE];
        sexpr source = sx_false, target = sx_false;

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/libcurie.sx", archprefix, sx_string(t->name));

        source = make_string (buffer);

        switch (i_fsl)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                snprintf (buffer, BUFFERSIZE, "%s/%s/libcurie.sx", sx_string(i_destdir), sx_string (i_destlibdir));
                target = make_string (buffer);
                break;
            case fs_afsl:
                snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/libcurie.sx", sx_string(i_destdir), uname_os, uname_arch);
                target = make_string (buffer);
                break;
        }

        workstack = cons (cons (source, target), workstack);
    }

    while (consp (cur))
    {
        sexpr item = car (cur);
        sexpr ccur = cdr (item);

        dname = car (item);

        while (consp(ccur))
        {
            sexpr s = car (ccur);

            workstack = cons (cons (cdr (s), get_data_install_path (dname, car (s))), workstack);

            ccur = cdr (ccur);
        }

        cur = cdr (cur);
    }
}

static void install_library (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_library_gcc     (name, t); break;
        case tc_borland:
            install_library_borland (name, t); break;
        case tc_msvc:
            install_library_msvc    (name, t); break;
    }
}

static void install_headers (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_headers_gcc     (name, t); break;
        case tc_borland:
            install_headers_borland (name, t); break;
        case tc_msvc:
            install_headers_msvc    (name, t); break;
    }
}

static void install_documentation_with_suffix (sexpr name, struct target *t, sexpr c4, const char *suffix)
{
    sexpr fn;
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s.%s", archprefix, sx_string(t->name), sx_string(c4), suffix);

    fn = make_string (buffer);

    if (truep(filep(fn)))
    {
        workstack
                = cons (cons (fn,
                              get_documentation_install_path
                                      (name, c4, t->dversion, suffix)),
                        workstack);
    }
}

static void install_documentation_man (sexpr name, struct target *t, sexpr file, sexpr abbr, sexpr section)
{
    workstack = cons (cons (file,
                            get_documentation_man_install_path (abbr, section)),
                      workstack);
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

static void install_programme (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_programme_gcc     (name, t); break;
        case tc_borland:
            install_programme_borland (name, t); break;
        case tc_msvc:
            install_programme_msvc    (name, t); break;
    }
}

static void do_install_target(struct target *t)
{
    if (truep(t->library))
    {
        install_library (t->name, t);

        if (!eolp(t->bootstrap))
        {
            char buffer[BUFFERSIZE];
            snprintf (buffer, BUFFERSIZE, "%s-bootstrap", sx_string(t->name));

            install_library (make_string(buffer), t);
        }
    }
    else if (truep(t->programme))
    {
        install_programme (t->name, t);
    }

    install_headers (t->name, t);
    install_support_files (t->name, t);
    install_documentation (t->name, t);
}

static void install_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_install_target (node_get_value(node));
    }
}

static void target_map_install (struct tree_node *node, void *u)
{
    do_install_target(node_get_value(node));
}

void install (sexpr buildtargets)
{
    sexpr cursor = buildtargets;

    if (falsep(i_destdir))
    {
        return;
    }

    if (eolp(cursor))
    {
        tree_map (&targets, target_map_install, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        install_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_install();
}
