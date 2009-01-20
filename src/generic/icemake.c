/*
 *  icemake.c
 *  libcurie/icemake
 *
 *  Created by Magnus Deininger on 01/11/2008.
 *  Renamed from build.c by Magnus Deininger on 17/11/2008.
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

#include <curie/sexpr.h>
#include <curie/memory.h>
#include <curie/tree.h>
#include <curie/multiplex.h>
#include <curie/exec.h>
#include <curie/signal.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#ifdef POSIX
#include <sys/utsname.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>

#include <icemake/icemake.h>

char uname_os     [BUFFERSIZE]         = "generic";
char uname_arch   [BUFFERSIZE]         = "generic";
char uname_vendor [BUFFERSIZE]         = "unknown";

enum toolchain uname_toolchain;
enum fs_layout i_fsl                   = fs_proper;
enum operating_system i_os             = os_unknown;

static char  archbuffer [BUFFERSIZE];
static char *archprefix;
static char *tcversion                 = (char *)0;

static int alive_processes             = 0;
static int files_open                  = 0;
static unsigned int max_processes      = 1;

sexpr co_freestanding                  = sx_false;

static sexpr i_optimise_linking        = sx_false;
static sexpr i_combine                 = sx_false;
static sexpr i_debug                   = sx_false;

sexpr workstack                        = sx_end_of_list;

sexpr i_destdir                        = sx_false;
sexpr i_pname                          = sx_false;
sexpr do_tests                         = sx_false;
sexpr do_install                       = sx_false;
sexpr do_build_documentation           = sx_false;
sexpr i_destlibdir                     = sx_false;

struct tree targets                    = TREE_INITIALISER;

static char **xenviron;

sexpr p_c_compiler                     = sx_false;
sexpr p_cpp_compiler                   = sx_false;
sexpr p_assembler                      = sx_false;
sexpr p_linker                         = sx_false;
sexpr p_archiver                       = sx_false;
sexpr p_archive_indexer                = sx_false;
sexpr p_diff                           = sx_false;

sexpr p_latex                          = sx_false;
sexpr p_pdflatex                       = sx_false;
sexpr p_latex2html                     = sx_false;
sexpr p_doxygen                        = sx_false;

static struct sexpr_io *stdio;

static void link_programme_gcc_filename (sexpr, sexpr, sexpr, struct target *);

static void *rm_recover(unsigned long int s, void *c, unsigned long int l)
{
    exit(22);
    return (void *)0;
}

static void *gm_recover(unsigned long int s)
{
    exit(23);
    return (void *)0;
}

static sexpr sx_string_dir_prefix (sexpr f, sexpr p)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "%s/%s", sx_string(p), sx_string(f));

    return make_string(buffer);
}

sexpr sx_string_dir_prefix_c (char *f, sexpr p)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "%s/%s", sx_string(p), f);

    return make_string(buffer);
}

static sexpr find_actual_file (sexpr p, sexpr file)
{
    sexpr r = sx_string_dir_prefix (file, p);
    struct stat st;

    if (stat (sx_string (r), &st) == 0)
    {
        return r;
    }

    return sx_false;
}

static sexpr find_in_permutations_vendor (sexpr p, sexpr file)
{
    sexpr r;

    if ((r = find_actual_file (sx_string_dir_prefix_c (uname_vendor, p), file)), stringp(r))
    {
        return r;
    }
    else if ((r = find_actual_file (p, file)), stringp(r))
    {
        return r;
    }

    return sx_false;
}

static sexpr find_in_permutations_toolchain (sexpr p, sexpr file)
{
    sexpr r;

    switch (uname_toolchain)
    {
        case tc_gcc:
            if ((r = find_in_permutations_vendor (sx_string_dir_prefix_c ("gnu", p), file)), stringp(r))
            {
                return r;
            }
            break;
    }

    if ((r = find_in_permutations_vendor (p, file)), stringp(r))
    {
        return r;
    }

    return sx_false;
}

static sexpr find_in_permutations_arch (sexpr p, sexpr file)
{
    sexpr r;

    if ((r = find_in_permutations_toolchain (sx_string_dir_prefix_c (uname_arch, p), file)), stringp(r))
    {
        return r;
    }
    else if ((r = find_in_permutations_toolchain (p, file)), stringp(r))
    {
        return r;
    }

    return sx_false;
}

static sexpr find_in_permutations_os (sexpr p, sexpr file)
{
    sexpr r;

    if ((r = find_in_permutations_arch (sx_string_dir_prefix_c (uname_os, p), file)), stringp(r))
    {
        return r;
    }
    else if ((r = find_in_permutations_arch (sx_string_dir_prefix_c ("posix", p), file)), stringp(r))
    {
        return r;
    }
    else if ((r = find_in_permutations_arch (sx_string_dir_prefix_c ("ansi", p), file)), stringp(r))
    {
        return r;
    }
    else if ((r = find_in_permutations_arch (sx_string_dir_prefix_c ("generic", p), file)), stringp(r))
    {
        return r;
    }
    else if ((r = find_in_permutations_arch (p, file)), stringp(r))
    {
        return r;
    }

    return sx_false;
}

static sexpr find_in_permutations (sexpr p, sexpr file)
{
    sexpr r;

    if (truep(i_debug) && ((r = find_in_permutations_os (sx_string_dir_prefix_c ("debug", p), file)), stringp(r)))
    {
        return r;
    }
    else
    if ((r = find_in_permutations_os (sx_string_dir_prefix_c ("internal", p), file)), stringp(r))
    {
        return r;
    }
    else if ((r = find_in_permutations_os (p, file)), stringp(r))
    {
        return r;
    }

    return sx_false;
}

static sexpr find_code_with_suffix (sexpr file, char *s)
{
    char buffer[BUFFERSIZE];
    sexpr r, sr;

    snprintf (buffer, BUFFERSIZE, "%s%s", sx_string(file), s);

    r = find_in_permutations (str_src, (sr = make_string (buffer)));

    return r;
}

static sexpr find_test_case_with_suffix (sexpr file, char *s)
{
    char buffer[BUFFERSIZE];
    sexpr r, sr;

    snprintf (buffer, BUFFERSIZE, "%s%s", sx_string(file), s);

    r = find_in_permutations (str_tests, (sr = make_string (buffer)));

    return r;
}

static sexpr find_code_c (sexpr file)
{
    return find_code_with_suffix (file, ".c");
}

static sexpr find_code_cpp (sexpr file)
{
    return find_code_with_suffix (file, ".c++");
}

static sexpr find_code_s (sexpr file)
{
    return find_code_with_suffix (file, ".s");
}

static sexpr find_code_S (sexpr file)
{
    return find_code_with_suffix (file, ".S");
}

static sexpr find_test_case_c (sexpr file)
{
    return find_test_case_with_suffix (file, ".c");
}

static sexpr find_test_case_cpp (sexpr file)
{
    return find_test_case_with_suffix (file, ".c++");
}

static sexpr find_header_with_suffix (sexpr name, sexpr file, char *s)
{
    char buffer[BUFFERSIZE];
    sexpr r, sr;

    snprintf (buffer, BUFFERSIZE, "%s/%s%s", sx_string(name), sx_string(file), s);

    r = find_in_permutations (str_include, (sr = make_string (buffer)));

    return r;
}

static sexpr find_header_h (sexpr name, sexpr file)
{
    return find_header_with_suffix (name, file, ".h");
}

static sexpr generate_object_file_name (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s.o", sx_string(name), archprefix, sx_string(file));

    return make_string(buffer);
}

static sexpr generate_test_object_file_name (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/tests/%s.o", sx_string(name), archprefix, sx_string(file));

    return make_string(buffer);
}

static sexpr generate_test_executable_file_name (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/tests/%s", sx_string(name), archprefix, sx_string(file));

    return make_string(buffer);
}

static sexpr find_code_highlevel (struct target *context, sexpr file)
{
    sexpr r;
    char buffer[BUFFERSIZE];
    sexpr subfile;

    snprintf (buffer, BUFFERSIZE, "%s-highlevel", sx_string (file));
    subfile = make_string (buffer);

    if (((r = find_code_cpp (subfile)), stringp(r)))
    {
        return cons(sym_cpp, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list)));
    }
    else if (((r = find_code_c (subfile)), stringp(r)))
    {
        return cons(sym_c, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list)));
    }

    return sx_false;
}

static void find_code (struct target *context, sexpr file)
{
    sexpr r;
    sexpr primus   = sx_false;
    sexpr secundus = sx_false;

    if (falsep(equalp(str_bootstrap, file)) || truep (co_freestanding))
    {
        if ((r = find_code_S (file)), stringp(r))
        {
            primus = cons(sym_preproc_assembly, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));

            secundus = find_code_highlevel(context, file);
        }
        else if ((r = find_code_s (file)), stringp(r))
        {
            primus = cons(sym_assembly, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));

            secundus = find_code_highlevel(context, file);
        }
    }

    if (falsep(primus))
    {
        if (((r = find_code_cpp (file)), stringp(r)))
        {
            primus = cons(sym_cpp, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));
        }
        else if (((r = find_code_c (file)), stringp(r)))
        {
            primus = cons(sym_c, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));
        }
        else
        {
            fprintf (stderr, "missing code file: %s\n", sx_string(file));
            exit(20);
        }
    }

    if (truep(equalp(file, str_bootstrap)))
    {
        context->bootstrap = cons (primus, context->bootstrap);
        if (!falsep(secundus))
        {
            context->bootstrap = cons (secundus, context->bootstrap);
        }
    }
    else
    {
        context->code = cons (primus, context->code);
        if (!falsep(secundus))
        {
            context->code = cons (secundus, context->code);
        }
    }
}

static void find_test_case (struct target *context, sexpr file)
{
    sexpr r;
    sexpr primus   = sx_false;

    if (((r = find_test_case_cpp (file)), stringp(r)))
    {
        primus = cons(sym_cpp, cons (r, cons (generate_test_object_file_name(context->name, file), cons (generate_test_executable_file_name(context->name, file), sx_end_of_list))));
    }
    else if (((r = find_test_case_c (file)), stringp(r)))
    {
        primus = cons(sym_c, cons (r, cons (generate_test_object_file_name(context->name, file), cons (generate_test_executable_file_name(context->name, file), sx_end_of_list))));
    }
    else
    {
        fprintf (stderr, "missing test case: %s\n", sx_string(file));
        exit(20);
    }

    context->test_cases = cons (primus, context->test_cases);
}

static void find_header (struct target *context, sexpr file)
{
    sexpr r;

    if ((r = find_header_h (context->name, file)), stringp(r))
    {
        context->headers = cons (cons(file, cons (r, sx_end_of_list)), context->headers);
    }
    else
    {
        fprintf (stderr, "missing header file: %s\n", sx_string(file));
        exit(21);
    }
}

static sexpr find_documentation_with_suffix (sexpr file, char *s)
{
    char buffer[BUFFERSIZE];
    sexpr r, sr;

    snprintf (buffer, BUFFERSIZE, "%s%s", sx_string(file), s);

    r = find_in_permutations (str_documentation, (sr = make_string (buffer)));

    return r;
}

static sexpr find_documentation_tex (sexpr file)
{
    return find_documentation_with_suffix (file, ".tex");
}

static void find_documentation (struct target *context, sexpr file)
{
    sexpr r;

    if ((r = find_documentation_tex (file)), stringp(r))
    {
        context->documentation = cons(cons(sym_tex, r), context->documentation);
    }
    else
    {
        fprintf (stderr, "missing documentation file: %s\n", sx_string(file));
        exit(21);
    }
}

static sexpr find_data (struct target *context, sexpr file)
{
    sexpr r;

    if ((r = find_in_permutations (str_data, file)), !stringp(r))
    {
        fprintf (stderr, "missing data file: %s\n", sx_string(file));
        exit(21);
    }

    return r;
}

static struct target *get_context()
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER (sizeof(struct target));
    struct target *context = get_pool_mem (&pool);

    context->library        = sx_false;
    context->hosted         = sx_false;
    context->use_curie      = sx_false;
    context->libraries      = sx_end_of_list;
    context->olibraries     = sx_end_of_list;
    context->code           = sx_end_of_list;
    context->test_cases     = sx_end_of_list;
    context->test_reference = sx_end_of_list;
    context->bootstrap      = sx_end_of_list;
    context->headers        = sx_end_of_list;
    context->use_objects    = sx_end_of_list;
    context->data           = sx_end_of_list;
    context->dname          = sx_false;
    context->description    = sx_false;
    context->dversion       = sx_false;
    context->durl           = sx_false;
    context->documentation  = sx_end_of_list;

    return context;
}

static void process_definition (struct target *context, sexpr definition)
{
    char buffer[BUFFERSIZE];

    while (consp(definition))
    {
        sexpr sxcar = car (definition);
        sexpr sxcaar = car (sxcar);

        if (truep(equalp(sxcar, sym_hosted)))
        {
            context->hosted = sx_true;
        }
        else if (truep(equalp(sxcaar, sym_name)))
        {
            context->dname = car(cdr(sxcar));
        }
        else if (truep(equalp(sxcaar, sym_description)))
        {
            context->description = car(cdr(sxcar));
        }
        else if (truep(equalp(sxcaar, sym_version)))
        {
            context->dversion = car(cdr(sxcar));
        }
        else if (truep(equalp(sxcaar, sym_url)))
        {
            context->durl = car(cdr(sxcar));
        }
        else if (truep(equalp(sxcar, sym_libcurie)))
        {
            context->use_curie = sx_true;

            if (falsep (context->hosted))
            {
                context->libraries = cons (str_curie_bootstrap, context->libraries);
            }

            context->libraries = cons (str_curie, context->libraries);

            if (falsep(equalp(str_curie, context->name)))
            {
                context->olibraries = cons (str_curie, context->olibraries);
            }
        }
        else if (truep(equalp(sxcaar, sym_code)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                find_code (context, car (sxc));

                sxc = cdr (sxc);
            }
        }
        else if (truep(equalp(sxcaar, sym_libraries)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                context->libraries = cons (car (sxc), context->libraries);
                context->olibraries = cons (car (sxc), context->olibraries);

                sxc = cdr (sxc);
            }
        }
        else if (truep(equalp(sxcaar, sym_test_cases)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                find_test_case (context, car (sxc));

                sxc = cdr (sxc);
            }
        }
        else if (truep(equalp(sxcaar, sym_headers)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                find_header (context, car (sxc));

                sxc = cdr (sxc);
            }
        }
        else if (truep(equalp(sxcaar, sym_documentation)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                find_documentation (context, car (sxc));

                sxc = cdr (sxc);
            }
        }
        else if (truep(equalp(sxcaar, sym_data)))
        {
            sexpr name = cdr (sxcar);

            if (consp (name))
            {
                sexpr d = sx_end_of_list;
                sexpr sxc = cdr (name);
                name = car (name);

                while (consp (sxc))
                {
                    sexpr filename = car (sxc);
                    d = cons (cons (filename, find_data (context, filename)), d);

                    sxc = cdr (sxc);
                }

                d = cons (name, d);

                context->data = cons (d, context->data);
            }
        }
        else if (truep(equalp(sxcaar, sym_use_objects)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                context->use_objects = cons (car(sxc), context->use_objects);

                sxc = cdr (sxc);
            }
        }
        else if (truep(equalp(sxcaar, sym_test_case_reference)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                context->test_reference
                        = cons (car(sxc), context->test_reference);

                sxc = cdr (sxc);
            }
        }
        else if (truep(equalp(sxcaar, sym_freestanding_if_asm)))
        {
            sexpr sxc = cdr (sxcar);

            co_freestanding = sx_true;

            while (consp (sxc))
            {
                sexpr r;

                if (((r = find_code_s(car(sxc))), !falsep(r)) ||
                    ((r = find_code_S(car(sxc))), !falsep(r)))
                {
                    sxc = cdr (sxc);
                }
                else
                {
                    co_freestanding = sx_false;
                    break;
                }
            }
        }

        definition = cdr (definition);
    }

    snprintf (buffer, BUFFERSIZE, "build/%s", sx_string(context->name));

    mkdir (buffer, 0755);

    snprintf (buffer, BUFFERSIZE, "build/%s/%s", sx_string(context->name), archprefix);

    mkdir (buffer, 0755);

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/tests", sx_string(context->name), archprefix);

    mkdir (buffer, 0755);

    if (truep(context->hosted))
    {
        context->libraries = cons (str_lc, context->libraries);
    }

    if (truep(i_combine) &&
        (uname_toolchain == tc_gcc) &&
        consp(context->code))
    {
        sexpr ccur = sx_end_of_list;
        sexpr cur = context->code;

        sexpr clist = sx_end_of_list;

        do {
            sexpr sxcar = car (cur);
            sexpr sxcaar = car (sxcar);
            sexpr sxcadar = car (cdr (sxcar));

            if (truep(equalp(sxcaar, sym_c)))
            {
                clist = cons (sxcadar, clist);
            }
            else
            {
                ccur = cons (sxcar, ccur);
            }

            cur = cdr (cur);
        } while (consp(cur));

        if (consp (clist))
        {
            char oname[BUFFERSIZE];
            sexpr sx_o;

            snprintf (oname, BUFFERSIZE, "%s-combined-c-source",
                      sx_string(context->name));

            sx_o = make_string (oname);

            ccur = cons (cons (sym_c, cons (clist, cons (generate_object_file_name (context->name, sx_o), sx_end_of_list))), ccur);

            sx_destroy (sx_o);
        }

        context->code = ccur;
    }
}

static struct target *create_library (sexpr definition)
{
    struct target *context = get_context();

    context->name = car(definition);
    context->library = sx_true;

    process_definition (context, cdr(definition));

    if (falsep(equalp(str_curie, context->name)))
    {
        context->libraries = cons (context->name, context->libraries);
    }

    return context;
}

static struct target *create_programme (sexpr definition)
{
    struct target *context = get_context();

    context->name = car(definition);

    process_definition (context, cdr(definition));

    return context;
}

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

static void map_includes_gcc (struct tree_node *node, void *psx)
{
    sexpr *sx = (sexpr *)psx;
    char buffer[BUFFERSIZE];
    struct target *t = node_get_value (node);

    snprintf (buffer, BUFFERSIZE, "-Lbuild/%s/%s", sx_string(t->name), archprefix);

    *sx = cons (make_string (buffer), *sx);
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

static void link_programme_gcc (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s", sx_string(t->name), archprefix, sx_string(name));

    link_programme_gcc_filename (make_string (buffer), name, code, t);
}

static void run_tests_library_gcc (sexpr name, struct target *t)
{
    sexpr s = t->test_cases;

    while (consp (s))
    {
        sexpr r = cdr(cdr(cdr(car(s))));

        workstack = cons (r, workstack);

        s = cdr (s);
    }
}

static void diff_test_reference_library_gcc (sexpr name, struct target *t)
{
    sexpr cur = t->test_reference;

    while (consp (cur))
    {
        sexpr r = car (cur);
        sexpr o = car (r), g = cdr (r);

        workstack = cons (cons (p_diff, cons (o, cons (g, sx_end_of_list))), workstack);

        cur = cdr (cur);
    }
}

static void post_process_library_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", sx_string(t->name), archprefix, sx_string(name));

    workstack
        = cons (cons (p_archive_indexer,
                  cons (make_string (buffer),
                        sx_end_of_list))
                , workstack);
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
                      get_library_install_path(name))
                , workstack);
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

static void run_tests_library (sexpr name, struct target *t)
{
    sexpr cur = t->test_reference;

    while (consp (cur))
    {
        sexpr r = car (car (cur));

        unlink (sx_string (r));

        cur = cdr (cur);
    }

    switch (uname_toolchain)
    {
        case tc_gcc:
            run_tests_library_gcc (name, t); break;
    }
}

static void build_documentation_library (sexpr name, struct target *t)
{
}

static void build_documentation_programme (sexpr name, struct target *t)
{
}

static void diff_test_reference_library (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            diff_test_reference_library_gcc (name, t); break;
    }
}

static void post_process_library (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            post_process_library_gcc (name, t); break;
    }
}

static void post_process_programme (sexpr name, struct target *t)
{
/*    switch (uname_toolchain)
    {
        case tc_gcc:
            post_process_programme_gcc (name, t); break;
    }*/
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

static void do_cross_link (struct target *target, struct target *source)
{
    sexpr cur = source->code;

    while (consp (cur))
    {
         sexpr ccar = car (cur);
         sexpr cccdr = cdr (ccar);

         target->code = cons (cons (sym_link, cccdr), target->code);

         cur = cdr (cur);
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

static void do_post_process_target(struct target *t)
{
    if (truep(t->library))
    {
        post_process_library (t->name, t);
    }
    else
    {
        post_process_programme (t->name, t);
    }
}

static void do_run_tests_target(struct target *t)
{
    if (truep(t->library))
    {
        diff_test_reference_library (t->name, t);
        run_tests_library (t->name, t);
    }
}

static void do_build_documentation_target(struct target *t)
{
    if (truep(t->library))
    {
        build_documentation_library (t->name, t);
    }
    else
    {
        build_documentation_programme (t->name, t);
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
}

static void link_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_link_target (node_get_value(node));
    }
}

static void post_process_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_post_process_target (node_get_value(node));
    }
}

static void run_tests_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_run_tests_target (node_get_value(node));
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

static void print_help(char *binaryname)
{
    fprintf (stdout,
        "Usage: %s [options] [targets]\n\n"
        "Options:\n"
        " -h           Print help and exit\n"
        " -t <chost>   Specify target CHOST\n"
        " -z <version> Specify target toolchain version\n"
        " -d <destdir> Specify the directory to install to.\n"
        " -i           Install resulting binaries\n"
        " -r           Execute runtime tests\n"
        " -f           Use the FHS layout for installation\n"
        " -b <pname>   Use a modified FHS layout (lib/<pname>/bin/ vs. bin/)\n"
        " -l <libdir>  Use <libdir> instead of 'lib' when installing\n"
        " -s           Use the default FS layout for installation\n"
        " -L           Optimise linking.\n"
        " -c           Use gcc's -combine option for C source files.\n"
        " -j <num>     Spawn <num> processes simultaneously.\n"
        " -D           Use debug code, if available.\n"
        " -x           Build documentation (if possible).\n\n"
        "The [targets] specify a list of things to build, according to the\n"
        "icemake.sx file located in the current working directory.\n\n",
        binaryname);
}

static void target_map_link (struct tree_node *node, void *u)
{
    do_link_target(node_get_value(node));
}

static void target_map_post_process (struct tree_node *node, void *u)
{
    do_post_process_target(node_get_value(node));
}

static void target_map_run_tests (struct tree_node *node, void *u)
{
    do_run_tests_target(node_get_value(node));
}

static void target_map_build_documentation (struct tree_node *node, void *u)
{
    do_build_documentation_target(node_get_value(node));
}

static void target_map_install (struct tree_node *node, void *u)
{
    do_install_target(node_get_value(node));
}

static void target_map_cross_link (struct tree_node *node, void *u)
{
    struct target *target = (struct target *) node_get_value (node);

    if (!eolp (target->use_objects))
    {
        sexpr cur = target->use_objects;

        while (consp (cur))
        {
            sexpr o = car (cur);
            struct tree_node *n1
                = tree_get_node_string (&targets, (char *)sx_string (o));
            struct target *s;

            if (n1 == (struct tree_node *)0)
            {
                exit (68);
            }

            s = (struct target *) node_get_value (n1);

            do_cross_link (target, s);

            cur = cdr(cur);
        }
    }
}

static void write_uname_element (char *source, char *target, int tlen)
{
    int i = 0;

    while (source[i] != 0)
    {
        if (isalnum(source[i]))
        {
            target[i] = tolower(source[i]);
        }
        else
        {
            target[i] = '-';
        }

        i++;

        if ((i >= (UNAMELENGTH - 1)) || (i >= tlen))
        {
            target[i] = 0;
            return;
        }
    }

    target[i] = 0;
}

static sexpr which (char *programme)
{
    char buffer[BUFFERSIZE];
    char *x = getenv("PATH"), *y = buffer;

    if (x == (char *)0) return sx_false;

    while ((*x != 0) && (y < (buffer + BUFFERSIZE - 1)))
    {
        if ((*x == ':') || (*x == ';'))
        {
            char subbuffer[BUFFERSIZE];
            struct stat st;

            *y = 0;
            y = buffer;

            snprintf (subbuffer, BUFFERSIZE, "%s/%s", buffer, programme);

            if (stat (subbuffer, &st) == 0)
            {
                return make_string (subbuffer);
            }
        }
        else
        {
            *y = *x;
            y++;
        }

        x++;
    }

    return sx_false;
}

static sexpr xwhich (char *programme)
{
    char buffer[BUFFERSIZE];
    sexpr w;

    if ((tcversion != (char *)0) &&
         (snprintf (buffer, BUFFERSIZE, "%s-%s-%s", archprefix, programme, tcversion),
          (w = which (buffer)), stringp(w)))
    {
        return w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-%s", archprefix, programme),
             (w = which (buffer)), stringp(w))
    {
        return w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "ar"),
             (w = which (programme)), stringp(w))
    {
        return w;
    }

    return sx_false;
}

static void initialise_toolchain_gcc()
{
    p_c_compiler = xwhich ("gcc");
    if (falsep(p_c_compiler)) { p_c_compiler = xwhich ("cc"); }
    if (falsep(p_c_compiler))
    {
        fprintf (stderr, "cannot find C compiler.\n");
        exit (21);
    }

    p_linker = p_c_compiler;
/*    p_assembler = p_c_compiler;*/

    p_cpp_compiler = xwhich ("g++");
    if (falsep(p_cpp_compiler))
    {
        fprintf (stderr, "cannot find C++ compiler.\n");
        exit (22);
    }

    p_assembler = xwhich ("as");
    if (falsep(p_assembler))
    {
        fprintf (stderr, "cannot find assembler.\n");
        exit (23);
    }

    p_archiver = xwhich ("ar");
    if (falsep(p_archiver))
    {
        fprintf (stderr, "cannot find archiver.\n");
        exit (25);
    }

    p_archive_indexer = xwhich ("ranlib");
    if (falsep(p_archive_indexer))
    {
        fprintf (stderr, "cannot find archive indexer.\n");
        exit (26);
    }

    p_diff = xwhich ("diff");
    if (falsep(p_diff))
    {
        fprintf (stderr, "cannot find diff programme.\n");
        exit (27);
    }
}

static void initialise_toolchain_tex()
{
    sexpr out;

    p_latex = xwhich ("latex");
    if (falsep(p_latex))
    {
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_latex, sx_end_of_list))));
        sx_destroy (out);
    }

    p_pdflatex = xwhich ("pdflatex");
    if (falsep(p_pdflatex))
    {
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_pdflatex, sx_end_of_list))));
        sx_destroy (out);
    }

    p_latex2html = xwhich ("latex2html");
    if (falsep(p_latex2html))
    {
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_latex2html, sx_end_of_list))));
        sx_destroy (out);
    }
}

static void initialise_toolchain_doxygen()
{
    sexpr out;

    p_doxygen = xwhich ("doxygen");
    if (falsep(p_doxygen))
    {
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_doxygen, sx_end_of_list))));
        sx_destroy (out);
    }
}

static void spawn_stack_items();

static void process_on_death(struct exec_context *context, void *p)
{
    if (context->status == ps_terminated)
    {
        sexpr sx = (sexpr)p;
        alive_processes--;

        if (context->exitstatus != 0)
        {
            sx_write (stdio, cons (sym_failed,
                                   cons (make_integer (context->exitstatus),
                                         cons (sx,
                                             sx_end_of_list))));

            exit (24);
        }

        free_exec_context (context);
    }
}

static void spawn_item (sexpr sx)
{
    sexpr cur = sx;
    struct exec_context *context;
    int c = 0;

    sx_write (stdio, sx);

    while (consp (cur))
    {
        c++;
        cur = cdr (cur);
    }
    c++;

    char *ex[c];

    c = 0;
    cur = sx;
    while (consp (cur))
    {
        sexpr sxcar = car (cur);
        ex[c] = (char *)sx_string (sxcar);
        c++;
        cur = cdr (cur);
    }
    ex[c] = (char *)0;

    context = execute (EXEC_CALL_NO_IO | EXEC_CALL_PURGE, ex, xenviron);

    switch (context->pid)
    {
        case -1: fprintf (stderr, "failed to spawn subprocess\n");
                 exit (22);
        case 0:  fprintf (stderr, "failed to execute binary image\n");
                 exit (23);
        default: alive_processes++;
                 multiplex_add_process (context, process_on_death, (void *)sx);
                 return;
    }
}

static void spawn_stack_items ()
{
    while (consp (workstack) && (alive_processes < max_processes))
    {
        sexpr wcdr = cdr (workstack);

        spawn_item (car (workstack));

        workstack = wcdr;
    }
}

enum signal_callback_result cb_on_bad_signal(enum signal s, void *p)
{
    fprintf (stderr, "problematic signal received: %i\n", s);
    exit (s);

    return scr_keep;
}

void loop_processes()
{
    spawn_stack_items ();

    while (alive_processes > 0)
    {
        multiplex();
        spawn_stack_items ();
    }
}

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

        if (stat (sx_string (source), &st) == 0)
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

static void crosslink_objects ()
{
    tree_map (&targets, target_map_cross_link, (void *)0);
}

static void ice_link (sexpr buildtargets)
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

static void post_process (sexpr buildtargets)
{
    sexpr cursor = buildtargets;
    if (eolp(cursor))
    {
        tree_map (&targets, target_map_post_process, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        post_process_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}

static void run_tests (sexpr buildtargets)
{
    sexpr cursor = buildtargets;
    if (eolp(cursor))
    {
        tree_map (&targets, target_map_run_tests, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        run_tests_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}

static void build_documentation (sexpr buildtargets)
{
    sexpr cursor = buildtargets;
    if (eolp(cursor))
    {
        tree_map (&targets, target_map_build_documentation, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        run_tests_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}

static void install (sexpr buildtargets)
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

static sexpr initialise_libcurie_filename (char *filename)
{
    struct sexpr_io *io;
    sexpr r;
    struct stat st;

    if (stat(filename, &st) != 0) return sx_false;

    io = sx_open_io(io_open_read(filename), io_open(-1));

    while (!eofp(r = sx_read (io)))
    {
        if (truep(equalp(r, sym_freestanding)))
        {
            co_freestanding = sx_true;
        }
        else if (truep(equalp(r, sym_hosted)))
        {
            co_freestanding = sx_false;
        }
    }

    sx_close_io (io);

    return sx_true;
}

static void initialise_libcurie()
{
    char buffer[BUFFERSIZE];

    if (snprintf (buffer, BUFFERSIZE, "build/curie/%s/libcurie.sx", archprefix),
        truep(initialise_libcurie_filename(buffer))) return;

    if (!falsep(i_destdir))
    {
        if (snprintf (buffer, BUFFERSIZE, "%s/%s/libcurie.sx", sx_string(i_destdir), sx_string (i_destlibdir)),
            truep(initialise_libcurie_filename(buffer))) return;

        switch (i_fsl)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                if (snprintf (buffer, BUFFERSIZE, "%s/usr/%s/libcurie.sx", sx_string(i_destdir), sx_string (i_destlibdir)),
                    truep(initialise_libcurie_filename(buffer))) return;
                if (snprintf (buffer, BUFFERSIZE, "%s/%s/libcurie.sx", sx_string(i_destdir), sx_string (i_destlibdir)),
                    truep(initialise_libcurie_filename(buffer))) return;
                break;
            case fs_proper:
                if (snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/libcurie.sx", sx_string(i_destdir), uname_os, uname_arch),
                    truep(initialise_libcurie_filename(buffer))) return;
                break;
        }
    }

    if (snprintf (buffer, BUFFERSIZE, "/%s/libcurie.sx", sx_string (i_destlibdir)),
        truep(initialise_libcurie_filename(buffer))) return;

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            if (snprintf (buffer, BUFFERSIZE, "/usr/%s/libcurie.sx", sx_string (i_destlibdir)),
                truep(initialise_libcurie_filename(buffer))) return;
            if (snprintf (buffer, BUFFERSIZE, "/%s/libcurie.sx", sx_string (i_destlibdir)),
                truep(initialise_libcurie_filename(buffer))) return;
        case fs_proper:
            if (snprintf (buffer, BUFFERSIZE, "/%s/%s/lib/libcurie.sx", uname_os, uname_arch), truep(initialise_libcurie_filename(buffer))) return;
            if (truep(initialise_libcurie_filename("/usr/lib/libcurie.sx"))) return;
            if (truep(initialise_libcurie_filename("/lib/libcurie.sx"))) return;
            break;
    }
}

int main (int argc, char **argv, char **environ)
{
    struct sexpr_io *io;
    sexpr r;
    int i = 1, q;
    char *target_architecture = (char *)getenv("CHOST");
    sexpr buildtargets = sx_end_of_list;
    struct stat st;

    xenviron = environ;

    set_resize_mem_recovery_function(rm_recover);
    set_get_mem_recovery_function(gm_recover);

    if (stat ("icemake.sx", &st) != 0)
    {
        perror ("icemake.sx");
        exit (15);
    }

    mkdir ("build", 0755);

    i_destlibdir = str_lib;

    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            int y = 1;
            int xn = i + 1;
            while (argv[i][y] != 0)
            {
                switch (argv[i][y])
                {
                    case 't':
                        if (xn < argc)
                        {
                            target_architecture = argv[xn];
                            xn++;
                        }
                        break;
                    case 'z':
                        if (xn < argc)
                        {
                            tcversion = argv[xn];
                            xn++;
                        }
                        break;
                    case 'd':
                        if (xn < argc)
                        {
                            i_destdir = make_string(argv[xn]);
                            xn++;
                        }
                        break;
                    case 'i':
                        do_install = sx_true;
                        break;
                    case 'L':
                        i_optimise_linking = sx_true;
                        break;
                    case 'c':
                        i_combine = sx_true;
                        break;
                    case 'D':
                        i_debug = sx_true;
                        break;
                    case 'x':
                        do_build_documentation = sx_true;
                        break;
                    case 'f':
                        i_fsl = fs_fhs;
                        break;
                    case 's':
                        i_fsl = fs_proper;
                        break;
                    case 'b':
                        i_fsl = fs_fhs_binlib;
                        if (xn < argc)
                        {
                            i_pname = make_string(argv[xn]);
                            xn++;
                        }
                    case 'l':
                        if (xn < argc)
                        {
                            i_destlibdir = make_string(argv[xn]);
                            xn++;
                        }
                        break;
                    case 'j':
                        if (xn < argc)
                        {
                            char *s = argv[xn];

                            max_processes = 0;

                            for (unsigned int j = 0; s[j]; j++)
                            {
                                max_processes = 10 * max_processes +
                                                (s[j] - '0');
                            }

                            xn++;
                        }

                        if (max_processes == 0)
                        {
                            max_processes = 1;
                        }
                        break;
                    case 'r':
                        do_tests = sx_true;
                        break;
                    case 'h':
                    case '-':
                        print_help (argv[0]);
                        return 0;
                }

                y++;
            }

            i = xn;
        }
        else
        {
            buildtargets = cons (make_string (argv[i]), buildtargets);
            i++;
        }
    }

    if (target_architecture != (char *)0)
    {
        int j = 0;

        archprefix = target_architecture;

        for (j = 0; target_architecture[j] != 0; j++)
        {
            if (target_architecture[j] == '-')
            {
                write_uname_element(target_architecture, uname_arch, j);
                j++;

                break;
            }
        }

        target_architecture += j;
        for (j = 0; target_architecture[j] != 0; j++)
        {
            if (target_architecture[j] == '-')
            {
                write_uname_element(target_architecture, uname_vendor, j);
                j++;

                break;
            }
        }

        target_architecture += j;
        for (j = 0; target_architecture[j] != 0; j++)
        {
            if (target_architecture[j] == '-')
            {
                write_uname_element(target_architecture, uname_os, j);
                j++;

                break;
            }
        }

        target_architecture += j;
        if (target_architecture != 0)
        {
            /* target_architecture => toolchain ID */
            uname_toolchain = tc_gcc;
        }
        else
        {
            uname_toolchain = tc_gcc;
        }
    }
    else
    {
        char *os     = uname_os;
        char *vendor = uname_vendor;
        char *arch   = uname_arch;
        char *toolchain;

        uname_toolchain = tc_gcc;

#ifdef POSIX
        struct utsname un;

        if (uname (&un) >= 0)
        {
            write_uname_element(un.sysname, uname_os, UNAMELENGTH - 1);
            write_uname_element(un.machine, uname_arch, UNAMELENGTH - 1);
        }

        os = uname_os;
        arch = uname_arch;
#endif

        switch (uname_toolchain)
        {
            case tc_gcc: toolchain = "gnu"; break;
        }

        snprintf (archbuffer, BUFFERSIZE, "%s-%s-%s-%s",
                  arch, vendor, os, toolchain);

        for (int j = 0; archbuffer[j]; j++)
        {
            archbuffer[j] = tolower(archbuffer[j]);
        }

        archprefix = archbuffer;
    }

    stdio                   = sx_open_stdio();

    switch (uname_toolchain)
    {
        case tc_gcc: initialise_toolchain_gcc(); break;
    }

    initialise_toolchain_tex ();
    initialise_toolchain_doxygen ();

    for (q = 0; uname_os[q] && "darwin"[q]; q++);
    if ((q == 6) && (uname_os[q] == 0)) i_os = os_darwin;
    for (q = 0; uname_os[q] && "linux"[q]; q++);
    if ((q == 5) && (uname_os[q] == 0)) i_os = os_linux;

    multiplex_io();
    multiplex_all_processes();
    multiplex_sexpr();
    multiplex_signal();

    multiplex_add_signal (sig_segv, cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_int,  cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_term, cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_bus,  cb_on_bad_signal, (void *)0);

    multiplex_add_sexpr (stdio, (void *)0, (void *)0);

    initialise_libcurie();

    io = sx_open_io (io_open_read("icemake.sx"), io_open(-1));

    while (!eofp(r = sx_read (io)))
    {
        struct target *t = (struct target *)0;

        if (nexp(r)) continue;
        if (consp(r)) {
            sexpr sxcar = car (r);

            if (truep(equalp(sxcar, sym_library)))
            {
                t = create_library (cdr (r));
            }
            else if (truep(equalp(sxcar, sym_programme)))
            {
                t = create_programme (cdr (r));
            }

        }

        if (t != (struct target *)0)
        {
            tree_add_node_string_value (&targets, (char *)sx_string(t->name), t);
        }
    }

    sx_close_io (io);

    crosslink_objects ();

    build (buildtargets);
    ice_link (buildtargets);
    post_process (buildtargets);

    if (truep (do_build_documentation))
    {
        build_documentation (buildtargets);
    }

    if (truep (do_tests))
    {
        run_tests (buildtargets);
    }

    if (truep (do_install))
    {
        install (buildtargets);
    }

    return 0;
}
