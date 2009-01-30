/*
 *  icemake-install.c
 *  libcurie/icemake
 *
 *  Split from icemake.c by Magnus Deininger on 20/01/2009.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _BSD_SOURCE

#include <curie/tree.h>
#include <curie/multiplex.h>
#include <curie/filesystem.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <icemake/icemake.h>

static int files_open                  = 0;

static void mkdir_p (sexpr path)
{
    char buffer[BUFFERSIZE];
    struct stat st;
    const char *p = sx_string (path);

    for (int i = 0; (p[i] != 0) && (i < (BUFFERSIZE - 2)); i++)
    {
        if (((p[i] == '/') || (p[i] == '\\')) && (i > 0))
        {
            buffer[i] = 0;

            if (stat(buffer, &st) != 0)
            {
                if (mkdir (buffer, 0777) != 0)
                {
                    perror ("mkdir");
                    exit (67);
                }
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

            symlink (sx_string(source), sx_string(target));
        }
        else if (stat (sx_string (source), &st) == 0)
        {
            mkdir_p (target);

            files_open++;

            in  = io_open_read   (sx_string (source));
            out = io_open_create (sx_string (target), st.st_mode);

            multiplex_add_io (in, install_read, install_close, (void *)out);
            multiplex_add_io_no_callback (out);
        }

        workstack = cdr(workstack);
    }

    while (files_open > 0)
    {
        multiplex();
    }
}

static sexpr get_library_install_path (sexpr name)
{
    char buffer[BUFFERSIZE];

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            snprintf (buffer, BUFFERSIZE, "%s/%s/lib%s.a", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name));
            return make_string (buffer);
            break;
        case fs_proper:
            snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s.a",
                      sx_string(i_destdir), uname_os, uname_arch,
                                sx_string(name));
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
            snprintf (buffer, BUFFERSIZE, "%s/%s/lib%s.so.%s", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name), sx_string(version));
            return make_string (buffer);
            break;
        case fs_proper:
            snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s.so.%s",
                      sx_string(i_destdir), uname_os, uname_arch,
                                sx_string(name), sx_string(version));
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
            snprintf (buffer, BUFFERSIZE, "%s/%s/lib%s.so", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(name));
            return make_string (buffer);
            break;
        case fs_proper:
            snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s.so",
                      sx_string(i_destdir), uname_os, uname_arch,
                                sx_string(name));
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static sexpr get_programme_install_path (sexpr name)
{
    char buffer[BUFFERSIZE];

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
        case fs_proper:
            snprintf (buffer, BUFFERSIZE, "%s/%s/%s/bin/%s",
                      sx_string(i_destdir), uname_os, uname_arch,
                                sx_string(name));
            return make_string (buffer);
            break;
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
        case fs_proper:
            snprintf (buffer, BUFFERSIZE, "%s/generic/doc/%s-%s/%s.%s",
                      sx_string(i_destdir), sx_string(name),
                      sx_string (version), sx_string (file), suffix);
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
        case fs_proper:
            snprintf (buffer, BUFFERSIZE, "%s/%s/%s/include/%s/%s.h",
                      sx_string(i_destdir), uname_os, uname_arch,
                                sx_string(name), sx_string (file));
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
        case fs_proper:
            snprintf (buffer, BUFFERSIZE, "%s/generic/configuration/%s/%s", sx_string(i_destdir), sx_string(name), sx_string (file));
            return make_string (buffer);
            break;
    }

    return sx_false;
}

static void install_library_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", sx_string(t->name), archprefix, sx_string(name));

    workstack
        = cons (cons (make_string (buffer),
                      get_library_install_path(name)),
                workstack);

    if (truep (i_dynamic_libraries))
    {
        char buffer[BUFFERSIZE];
        sexpr fname;

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.so.%s", sx_string(t->name), archprefix, sx_string(name), sx_string(t->dversion));

        fname = make_string(buffer);

        if (truep(filep(fname)))
        {
            workstack
                    = cons (cons (make_string (buffer),
                            get_so_library_install_path(name, t->dversion)),
                    workstack);

            snprintf (buffer, BUFFERSIZE, "lib%s.so.%s", sx_string(name), sx_string(t->dversion));

            workstack
                    = cons (cons (sym_symlink, cons (make_string (buffer),
                            get_so_library_symlink_path(name))),
                    workstack);
        }
    }
}

static void install_programme_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s", sx_string(t->name), archprefix, sx_string(name));

    workstack
        = cons (cons (make_string (buffer),
                      get_programme_install_path(name)), workstack);
}

static void install_headers_gcc (sexpr name, struct target *t)
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

static void install_support_files_gcc (sexpr name, struct target *t)
{
    sexpr cur = t->data, dname;

    if (truep(equalp(name, str_curie)))
    {
        char buffer[BUFFERSIZE];
        sexpr source = sx_false, target = sx_false;

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/libcurie.sx", sx_string(t->name), archprefix);

        source = make_string (buffer);

        switch (i_fsl)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                snprintf (buffer, BUFFERSIZE, "%s/%s/libcurie.sx", sx_string(i_destdir), sx_string (i_destlibdir));
                target = make_string (buffer);
                break;
            case fs_proper:
                snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/libcurie.sx", sx_string(i_destdir), uname_os, uname_arch);
                target = make_string (buffer);
                break;
        }

        workstack = cons (cons (source, target), workstack);
    }

    if (truep(t->library))
    {
        char buffer[BUFFERSIZE];
        sexpr source = sx_false, target = sx_false;

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.pc", sx_string(t->name), archprefix, sx_string(t->name));

        source = make_string (buffer);

        switch (i_fsl)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                snprintf (buffer, BUFFERSIZE, "%s/%s/pkgconfig/lib%s.pc", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(t->name));
                target = make_string (buffer);
                break;
            case fs_proper:
                snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s.pc", sx_string(i_destdir), uname_os, uname_arch, sx_string(t->name));
                target = make_string (buffer);
                break;
        }

        workstack = cons (cons (source, target), workstack);

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s-hosted.pc", sx_string(t->name), archprefix, sx_string(t->name));

        source = make_string (buffer);

        switch (i_fsl)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                snprintf (buffer, BUFFERSIZE, "%s/%s/pkgconfig/lib%s-hosted.pc", sx_string(i_destdir), sx_string (i_destlibdir), sx_string(t->name));
                target = make_string (buffer);
                break;
            case fs_proper:
                snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/lib%s-hosted.pc", sx_string(i_destdir), uname_os, uname_arch, sx_string(t->name));
                target = make_string (buffer);
                break;
        }

        workstack = cons (cons (source, target), workstack);
    }


    while (consp (cur))
    {
        sexpr item = car (cur);

        dname = car (item);
        sexpr ccur = cdr (item);

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
            install_library_gcc (name, t); break;
    }
}

static void install_headers (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_headers_gcc (name, t); break;
    }
}

static void install_documentation_with_suffix (sexpr name, struct target *t, sexpr c4, const char *suffix)
{
    sexpr fn;
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s.%s", sx_string(t->name), archprefix, sx_string(c4), suffix);

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

static void install_documentation (sexpr name, struct target *t)
{
    sexpr c = t->documentation;

    while (consp (c))
    {
        sexpr c2 = car(c);
        sexpr c3 = cdr(c2);
        sexpr c4 = car(c3);

        install_documentation_with_suffix (name, t, c4, "pdf");
        install_documentation_with_suffix (name, t, c4, "dvi");
        install_documentation_with_suffix (name, t, c4, "ps");
        install_documentation_with_suffix (name, t, c4, "eps");
        install_documentation_with_suffix (name, t, c4, "html");

        c = cdr (c);
    }
}

static void install_support_files (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_support_files_gcc (name, t); break;
    }
}

static void install_programme (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_programme_gcc (name, t); break;
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
    else
    {
        install_programme (t->name, t);
    }

    install_headers (t->name, t);
    install_support_files (t->name, t);

    if (do_build_documentation)
    {
        install_documentation (t->name, t);
    }
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
