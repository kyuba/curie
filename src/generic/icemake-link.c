/*
 *  icemake-link.c
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

#include <curie/tree.h>
#include <curie/multiplex.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <icemake/icemake.h>

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
            if (truep(co_freestanding))
            {
                io_collect (o, " -static -nodefaultlibs -nostartfiles -nostdlib", 47);

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

static sexpr get_libc_linker_options_gcc (struct target *t, sexpr sx)
{
    define_string (str_u,              "-u");
    define_string (str_e,              "-e");
    define_string (str_start,          "_start");
    define_string (str_nostdlib,       "-nostdlib");
    define_string (str_nodefaultlibs,  "-nodefaultlibs");
    define_string (str_nostartfiles,   "-nostartfiles");
    define_string (str_Wlx,            "-Wl,-x");
    define_string (str_Wls,            "-Wl,-s");
    define_string (str_Wlznoexecstack, "-Wl,-z,noexecstack");
    define_string (str_Wlznorelro,     "-Wl,-z,norelro");
    define_string (str_Wlgcsections,   "-Wl,--gc-sections");
    define_string (str_Wlsortcommon,   "-Wl,--sort-common");

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
        else
        {
            if (truep(co_freestanding))
            {
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
    }

    return sx;
}

static void map_includes_gcc (struct tree_node *node, void *psx)
{
    sexpr *sx = (sexpr *)psx;
    char buffer[BUFFERSIZE];
    struct target *t = node_get_value (node);

    snprintf (buffer, BUFFERSIZE, "-Lbuild/%s/%s", sx_string(t->name), archprefix);

    *sx = cons (make_string (buffer), *sx);
}

static sexpr get_special_linker_options_gcc (sexpr sx)
{
    char *f = getenv ("LDFLAGS");

    tree_map (&targets, map_includes_gcc, (void *)&sx);

    if (f != (char *)0)
    {
        char buffer[BUFFERSIZE];
        int j = 0;
        sexpr t = sx_end_of_list;

        for (int i = 0; f[i] != 0; i++)
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

static void link_programme_gcc_filename (sexpr ofile, sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    struct stat res, st;
    char havebin;
    sexpr sx = sx_end_of_list, cur;

    havebin = (stat (sx_string (ofile), &res) == 0);

    cur = t->libraries;

    while (consp (cur))
    {
        sexpr libname = car (cur);

        snprintf (buffer, BUFFERSIZE, "-l%s", sx_string (libname));

        sx = cons (make_string (buffer), sx);

        cur = cdr (cur);
    }

    while (consp (code))
    {
        sexpr objectfile = car (cdr (cdr (car (code))));
        sx = cons (objectfile, sx);

        if (havebin &&
            (stat (sx_string(objectfile), &st) == 0) &&
            (st.st_mtime > res.st_mtime))
        {
            havebin = 0;
        }

        code = cdr (code);
    }

    if (!havebin) {
        workstack
                = cons (cons (p_linker,
                        get_libc_linker_options_gcc (t,
                                get_special_linker_options_gcc (
                                        cons (str_do,
                                              cons (ofile,
                                                      sx)))))
                , workstack);
    }
}

static void link_library_gcc (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    struct stat res, st;
    char havelib;
    sexpr sx = sx_end_of_list, cur;
    struct io *pcfile, *pcfile_hosted;

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.pc", sx_string(t->name), archprefix, sx_string(name));

    multiplex_add_io_no_callback (pcfile = io_open_create (buffer, 0644));

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s-hosted.pc", sx_string(t->name), archprefix, sx_string(name));

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

    snprintf (buffer, BUFFERSIZE, " -l%s\nCflags: -fno-exceptions -ffreestanding\n", sx_string(t->name));

    io_collect (pcfile, buffer, strlen(buffer));
    io_collect (pcfile_hosted, buffer, strlen(buffer));

    if (truep(equalp(name, str_curie)))
    {
        struct sexpr_io *io;

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.sx", sx_string(t->name), archprefix, sx_string(name));

        io = sx_open_io(io_open(-1), io_open_create(buffer, 0644));

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

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", sx_string(t->name), archprefix, sx_string(name));

    havelib = (stat (buffer, &res) == 0);

    while (consp (code))
    {
        sexpr objectfile = car (cdr (cdr (car (code))));
        sx = cons (objectfile, sx);

        if (havelib &&
            (stat (sx_string(objectfile), &st) == 0) &&
            (st.st_mtime > res.st_mtime))
        {
            havelib = 0;
        }

        code = cdr (code);
    }

    if (!havelib) {
        workstack
                = cons (cons (p_archiver,
                        cons (str_dr,
                              cons (make_string (buffer),
                                    sx)))
                , workstack);
    }

    if (truep(do_tests))
    {
        sexpr s = t->test_cases;

        while (consp (s))
        {
            sexpr s2 = cdr(cdr(car(s)));
            sexpr s3 = car(s2);
            sexpr s4 = car(cdr(s2));
            sexpr s5 = cons(cons (sym_c, cons (s3, cons(s3, sx_end_of_list))), sx_end_of_list);

            link_programme_gcc_filename (s4, name, s5, t);

            s = cdr (s);
        }
    }
}

static void link_programme_gcc (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s", sx_string(t->name), archprefix, sx_string(name));

    link_programme_gcc_filename (make_string (buffer), name, code, t);
}

static void link_library (sexpr name, sexpr code, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            link_library_gcc (name, code, t); break;
    }
}

static void link_programme (sexpr name, sexpr code, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            link_programme_gcc (name, code, t); break;
    }
}

static void do_link_target(struct target *t)
{
    if (truep(t->library))
    {
        link_library (t->name, t->code, t);

        if (!eolp(t->bootstrap))
        {
            char buffer[BUFFERSIZE];
            snprintf (buffer, BUFFERSIZE, "%s-bootstrap", sx_string(t->name));

            link_library (make_string(buffer), t->bootstrap, t);
        }
    }
    else
    {
        link_programme (t->name, t->code, t);
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
