/*
 *  icemake.c
 *  libcurie/icemake
 *
 *  Created by Magnus Deininger on 01/11/2008.
 *  Renamed from build.c by Magnus Deininger on 17/11/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
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

static char uname_os     [UNAMELENGTH] = "generic";
static char uname_arch   [UNAMELENGTH] = "generic";
static char uname_vendor [UNAMELENGTH] = "unknown";

static enum toolchain uname_toolchain;

static char archbuffer   [BUFFERSIZE];
static char *archprefix;
static char *tcversion                 = (char *)0;

static sexpr sym_library               = sx_false;
static sexpr sym_libraries             = sx_false;
static sexpr sym_test_cases            = sx_false;
static sexpr sym_test_case_reference   = sx_false;
static sexpr sym_programme             = sx_false;
static sexpr sym_hosted                = sx_false;
static sexpr sym_code                  = sx_false;
static sexpr sym_headers               = sx_false;
static sexpr sym_link                  = sx_false;
static sexpr sym_use_objects           = sx_false;
static sexpr sym_assembly              = sx_false;
static sexpr sym_cpp                   = sx_false;
static sexpr sym_c                     = sx_false;
static sexpr sym_libc                  = sx_false;
static sexpr sym_libcurie              = sx_false;
static sexpr sym_freestanding          = sx_false;
static sexpr sym_freestanding_if_asm   = sx_false;
static sexpr sym_data                  = sx_false;
static sexpr sym_description           = sx_false;
static sexpr sym_version               = sx_false;
static sexpr sym_name                  = sx_false;
static sexpr sym_url                   = sx_false;
static sexpr str_bootstrap             = sx_false;
static sexpr str_curie                 = sx_false;
static sexpr str_curie_bootstrap       = sx_false;
static sexpr str_static                = sx_false;
static sexpr str_lc                    = sx_false;

static int alive_processes             = 0;
static int files_open                  = 0;
static int max_processes               = 1;

static sexpr co_freestanding           = sx_false;

static sexpr i_optimise_linking        = sx_false;
static sexpr i_valgrind                = sx_false;
static sexpr i_debug                   = sx_false;

static struct sexpr_io *stdio;

static sexpr workstack                 = sx_end_of_list;

static enum fs_layout i_fsl            = fs_proper;
static enum operating_system i_os      = os_unknown;
static sexpr i_destdir                 = sx_false;
static sexpr do_tests                  = sx_false;
static sexpr do_install                = sx_false;

struct tree targets                    = TREE_INITIALISER;

static char **xenviron;

static sexpr p_c_compiler;
static sexpr p_cpp_compiler;
static sexpr p_assembler;
static sexpr p_linker;
static sexpr p_archiver;
static sexpr p_archive_indexer;
static sexpr p_diff;

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

static sexpr sx_string_dir_prefix_c (char *f, sexpr p)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "%s/%s", sx_string(p), f);

    return make_string(buffer);
}

static sexpr find_actual_file (sexpr p, sexpr file)
{
    sexpr r = sx_string_dir_prefix (file, p);
    struct stat st;

    sx_destroy (p);

    if (stat (sx_string (r), &st) == 0)
    {
        return r;
    }
    else
    {
        sx_destroy (r);
    }

    return sx_false;
}

static sexpr find_in_permutations_vendor (sexpr p, sexpr file)
{
    sexpr r;

    if ((r = find_actual_file (sx_string_dir_prefix_c (uname_vendor, p), file)), stringp(r))
    {
        goto ret;
    }
    else if (sx_xref(p), (r = find_actual_file (p, file)), stringp(r))
    {
        goto ret;
    }

    ret:
    sx_destroy (p);

    return r;
}

static sexpr find_in_permutations_toolchain (sexpr p, sexpr file)
{
    sexpr r;

    switch (uname_toolchain)
    {
        case tc_gcc:
            if ((r = find_in_permutations_vendor (sx_string_dir_prefix_c ("gnu", p), file)), stringp(r))
            {
                goto ret;
            }
            break;
    }

    if (sx_xref(p), (r = find_in_permutations_vendor (p, file)), stringp(r))
    {
        goto ret;
    }

    ret:
    sx_destroy (p);

    return r;
}

static sexpr find_in_permutations_arch (sexpr p, sexpr file)
{
    sexpr r;

    if ((r = find_in_permutations_toolchain (sx_string_dir_prefix_c (uname_arch, p), file)), stringp(r))
    {
        goto ret;
    }
    else if (sx_xref(p), (r = find_in_permutations_toolchain (p, file)), stringp(r))
    {
        goto ret;
    }

    ret:
    sx_destroy (p);

    return r;
}

static sexpr find_in_permutations_os (sexpr p, sexpr file)
{
    sexpr r;

    if ((r = find_in_permutations_arch (sx_string_dir_prefix_c (uname_os, p), file)), stringp(r))
    {
        goto ret;
    }
    else if ((r = find_in_permutations_arch (sx_string_dir_prefix_c ("posix", p), file)), stringp(r))
    {
        goto ret;
    }
    else if ((r = find_in_permutations_arch (sx_string_dir_prefix_c ("ansi", p), file)), stringp(r))
    {
        goto ret;
    }
    else if ((r = find_in_permutations_arch (sx_string_dir_prefix_c ("generic", p), file)), stringp(r))
    {
        goto ret;
    }
    else if (sx_xref(p), (r = find_in_permutations_arch (p, file)), stringp(r))
    {
        goto ret;
    }

    ret:
    sx_destroy (p);

    return r;
}

static sexpr find_in_permutations (sexpr p, sexpr file)
{
    sexpr r;

    if (truep(i_valgrind) && ((r = find_in_permutations_os (sx_string_dir_prefix_c ("valgrind", p), file)), stringp(r)))
    {
        goto ret;
    }
    else if (truep(i_debug) && ((r = find_in_permutations_os (sx_string_dir_prefix_c ("debug", p), file)), stringp(r)))
    {
        goto ret;
    }
    else
    if ((r = find_in_permutations_os (sx_string_dir_prefix_c ("internal", p), file)), stringp(r))
    {
        goto ret;
    }
    else if (sx_xref(p), (r = find_in_permutations_os (p, file)), stringp(r))
    {
        goto ret;
    }

    ret:
    sx_destroy(p);

    return r;
}

static sexpr find_code_with_suffix (sexpr file, char *s)
{
    char buffer[BUFFERSIZE];
    sexpr r, sr;

    snprintf (buffer, BUFFERSIZE, "%s%s", sx_string(file), s);

    r = find_in_permutations (make_string("src"), (sr = make_string (buffer)));

    sx_destroy (sr);

    return r;
}

static sexpr find_test_case_with_suffix (sexpr file, char *s)
{
    char buffer[BUFFERSIZE];
    sexpr r, sr;

    snprintf (buffer, BUFFERSIZE, "%s%s", sx_string(file), s);

    r = find_in_permutations (make_string("tests"), (sr = make_string (buffer)));

    sx_destroy (sr);

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

    r = find_in_permutations (make_string("include"), (sr = make_string (buffer)));

    sx_destroy (sr);

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

static void find_code (struct target *context, sexpr file)
{
    sexpr r;
    sexpr primus   = sx_false;
    sexpr secundus = sx_false;

    if (((r = find_code_S (file)), stringp(r)) ||
        ((r = find_code_s (file)), stringp(r)))
    {
        char buffer[BUFFERSIZE];
        sexpr subfile;

        primus = cons(sym_assembly, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));

        snprintf (buffer, BUFFERSIZE, "%s-highlevel", sx_string (file));
        subfile = make_string (buffer);

        if (((r = find_code_cpp (subfile)), stringp(r)))
        {
            secundus = cons(sym_cpp, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list)));
        }
        else if (((r = find_code_c (subfile)), stringp(r)))
        {
            secundus = cons(sym_c, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list)));
        }

        sx_destroy (subfile);
    }
    else if (((r = find_code_cpp (file)), stringp(r)))
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

static sexpr find_data (struct target *context, sexpr file)
{
    sexpr r;

    if ((r = find_in_permutations (make_string("data"), file)), !stringp(r))
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

            context->libraries = cons (str_lc, context->libraries);
        }
        else if (truep(equalp(sxcaar, sym_name)))
        {
            context->dname = car(cdr(car(definition)));
        }
        else if (truep(equalp(sxcaar, sym_description)))
        {
            context->description = car(cdr(car(definition)));
        }
        else if (truep(equalp(sxcaar, sym_version)))
        {
            context->dversion = car(cdr(car(definition)));
        }
        else if (truep(equalp(sxcaar, sym_url)))
        {
            context->durl = car(cdr(car(definition)));
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
                    sx_destroy (r);
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
}

static struct target *create_library (sexpr definition)
{
    struct target *context = get_context();

    context->name = car(definition);
    context->library = sx_true;
    sx_xref (context->name);

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
    sx_xref (context->name);

    process_definition (context, cdr(definition));

    return context;
}

static sexpr f_exist_add (sexpr f, sexpr lis)
{
    struct stat st;

    if (stat (sx_string (f), &st) == 0)
    {
        return cons (f, lis);
    }
    else
    {
        sx_destroy (f);
    }

    return lis;
}

static sexpr permutate_paths_vendor (sexpr p, sexpr lis)
{
    sx_xref(p);

    lis = f_exist_add (sx_string_dir_prefix_c (uname_vendor, p), lis);
    lis = f_exist_add (p, lis);

    sx_destroy (p);

    return lis;
}

static sexpr permutate_paths_toolchain (sexpr p, sexpr lis)
{
    sx_xref(p);

    switch (uname_toolchain)
    {
        case tc_gcc:
            lis = permutate_paths_vendor (sx_string_dir_prefix_c ("gnu", p), lis);
            break;
    }
    lis = permutate_paths_vendor (p, lis);

    sx_destroy (p);

    return lis;
}

static sexpr permutate_paths_arch (sexpr p, sexpr lis)
{
    sx_xref(p);

    lis = permutate_paths_toolchain (sx_string_dir_prefix_c (uname_arch, p), lis);
    lis = permutate_paths_toolchain (p, lis);

    sx_destroy (p);

    return lis;
}

static sexpr permutate_paths_os (sexpr p, sexpr lis)
{
    sx_xref(p);

    lis = permutate_paths_arch (sx_string_dir_prefix_c (uname_os, p), lis);
    lis = permutate_paths_arch (p, lis);
    lis = permutate_paths_arch (sx_string_dir_prefix_c ("generic", p), lis);
    lis = permutate_paths_arch (sx_string_dir_prefix_c ("ansi", p), lis);
    lis = permutate_paths_arch (sx_string_dir_prefix_c ("posix", p), lis);

    sx_destroy (p);

    return lis;
}

static sexpr permutate_paths (sexpr p)
{
    sexpr lis = sx_end_of_list;

    sx_xref(p);

    lis = permutate_paths_os (p, lis);
    lis = permutate_paths_os (sx_string_dir_prefix_c ("internal", p), lis);
    lis = permutate_paths_os (sx_string_dir_prefix_c ("debug", p), lis);
    lis = permutate_paths_os (sx_string_dir_prefix_c ("valgrind", p), lis);

    sx_destroy(p);

    return lis;
}

static sexpr prepend_includes_gcc (sexpr x)
{
    sexpr include_paths = permutate_paths (make_string ("include"));
    sexpr cur = include_paths;

    while (consp (cur))
    {
        sexpr sxcar = car(cur);
        char buffer [BUFFERSIZE];

        snprintf (buffer, BUFFERSIZE, "-I%s", sx_string(sxcar));

        x = cons (make_string (buffer), x);

        cur = cdr (cur);
    }

    sx_destroy (include_paths);

    return x;
}

static sexpr prepend_ccflags_gcc (sexpr x)
{
    if (truep(co_freestanding))
    {
        switch (i_os)
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

static sexpr prepend_cflags_gcc (sexpr x)
{
    static sexpr str_ffreestanding = sx_false;
    char *f = getenv ("CFLAGS");

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

        while (consp (t)) { x = cons (car(t), x); t = cdr (t); }
    }

    if (falsep(str_ffreestanding))
    {
        str_ffreestanding = make_string ("-ffreestanding");
    }

    if (truep (co_freestanding))
    {
        x = cons (str_ffreestanding, x);
    }

    return prepend_ccflags_gcc(x);
}

static sexpr prepend_cxxflags_gcc (sexpr x)
{
    char *f = getenv ("CXXFLAGS");

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

        while (consp (t)) { x = cons (car(t), x); t = cdr (t); }
    }

    return prepend_ccflags_gcc(x);
}

static void build_object_gcc_assembly (const char *source, const char *target)
{
    workstack
        = cons (cons (p_assembler,
/*                  cons (make_string ("-c"),*/
                    cons (make_string (source),
                      cons (make_string ("-o"),
                        cons (make_string(target), sx_end_of_list))))/*)*/
                , workstack);
}

static void build_object_gcc_c (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
                  cons (make_string ("-DPOSIX"),
                  cons (make_string ("-DGCC"),
                  cons (make_string ("--std=c99"),
                    cons (make_string ("-Wall"),
                      cons (make_string ("-pedantic"),
                        prepend_cflags_gcc (
                        prepend_includes_gcc (
                          cons (make_string ("-c"),
                            cons (make_string (source),
                              cons (make_string ("-o"),
                                cons (make_string(target), sx_end_of_list))))))))))))
                , workstack);
}

static void build_object_gcc_cpp (const char *source, const char *target)
{
    workstack
        = cons (cons (p_cpp_compiler,
                  cons (make_string ("-DPOSIX"),
                  cons (make_string ("-DGCC"),
                  cons (make_string ("-fno-exceptions"),
                    prepend_cxxflags_gcc (
                    prepend_includes_gcc (
                      cons (make_string ("-c"),
                        cons (make_string (source),
                          cons (make_string ("-o"),
                            cons (make_string(target), sx_end_of_list))))))))))
                , workstack);
}

static void build_object_gcc (sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_assembly)))
    {
        build_object_gcc_assembly (sx_string(source), sx_string(target));
    }
    else if (truep(equalp(type, sym_c)))
    {
        build_object_gcc_c (sx_string(source), sx_string(target));
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_gcc_cpp (sx_string(source), sx_string(target));
    }
    else
    {
        fprintf (stderr, "Unknown code file type: %s\n", sx_symbol(type));
        exit (20);
    }
}

static void build_object(sexpr desc)
{
    sexpr type = car(desc);
    sexpr source = car(cdr(desc));
    sexpr target = car(cdr(cdr(desc)));

    struct stat sst, tst;
    if ((stat (sx_string (source), &sst) == 0) &&
        (stat (sx_string (target), &tst) == 0) &&
        (tst.st_mtime > sst.st_mtime)) return;

    switch (uname_toolchain)
    {
        case tc_gcc:
            build_object_gcc (type, source, target); break;
    }
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
        sx_xref (objectfile);
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
                        cons (make_string ("-r"),
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

            sx_xref (s3);
            sx_xref (s4);

            link_programme_gcc_filename (s4, name, s5, t);

            sx_destroy (s5);

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
    static sexpr str_u                 = sx_false;
    static sexpr str_e                 = sx_false;
    static sexpr str_start             = sx_false;
    static sexpr str_nostdlib          = sx_false;
    static sexpr str_nostartfiles      = sx_false;
    static sexpr str_nodefaultlibs     = sx_false;
    static sexpr str_Wlx               = sx_false;
    static sexpr str_Wls               = sx_false;
    static sexpr str_Wlznoexecstack    = sx_false;
    static sexpr str_Wlznorelro        = sx_false;
    static sexpr str_Wlgcsections      = sx_false;
    static sexpr str_Wlsortcommon      = sx_false;

    if (falsep(str_u))
    {
        str_u                          = make_string ("-u");
        str_e                          = make_string ("-e");
        str_start                      = make_string ("_start");
        str_nostdlib                   = make_string ("-nostdlib");
        str_nostartfiles               = make_string ("-nostartfiles");
        str_nodefaultlibs              = make_string ("-nodefaultlibs");
        str_Wlx                        = make_string ("-Wl,-x");
        str_Wls                        = make_string ("-Wl,-s");
        str_Wlznoexecstack             = make_string ("-Wl,-z,noexecstack");
        str_Wlznorelro                 = make_string ("-Wl,-z,norelro");
        str_Wlgcsections               = make_string ("-Wl,--gc-sections");
        str_Wlsortcommon               = make_string ("-Wl,--sort-common");
    }

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
        sx_xref (objectfile);
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
                                        cons (make_string ("-o"),
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
        sx_xref (r);

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
            snprintf (buffer, BUFFERSIZE, "%s/lib/lib%s.a", sx_string(i_destdir), sx_string(name));
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

    sx_xref (c);

    while (consp (c))
    {
        sexpr c2 = car(c);
        sexpr c3 = car(c2);
        sexpr c4 = car(cdr(c2));

        sx_xref (c3);
        sx_xref (c4);

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
                snprintf (buffer, BUFFERSIZE, "%s/lib/libcurie.sx", sx_string(i_destdir));
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
                snprintf (buffer, BUFFERSIZE, "%s/lib/pkgconfig/lib%s.pc", sx_string(i_destdir), sx_string(t->name));
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
                snprintf (buffer, BUFFERSIZE, "%s/lib/pkgconfig/lib%s-hosted.pc", sx_string(i_destdir), sx_string(t->name));
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

         sx_xref (cccdr);

         target->code = cons (cons (sym_link, cccdr), target->code);

         cur = cdr (cur);
    }
}

static void do_build_target(struct target *t)
{
    sexpr c = t->code;

    while (consp (c))
    {
        build_object(car(c));

        c = cdr (c);
    }

    c = t->bootstrap;

    while (consp (c))
    {
        build_object(car(c));

        c = cdr (c);
    }

    if (truep(do_tests))
    {
        c = t->test_cases;

        while (consp (c))
        {
            build_object(car(c));

            c = cdr (c);
        }
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

static void build_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_build_target (node_get_value(node));
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
        " -s           Use the default FS layout for installation\n"
        " -L           Optimise linking.\n"
        " -V           Use valgrindable code, if available.\n"
        " -D           Use debug code, if available.\n\n"
        "The [targets] specify a list of things to build, according to the\n"
        "icemake.sx file located in the current working directory.\n\n",
        binaryname);
}

static void target_map_build (struct tree_node *node, void *u)
{
    do_build_target(node_get_value(node));
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

static void spawn_stack_items();

static void process_on_death(struct exec_context *context, void *p)
{
    if (context->status == ps_terminated)
    {
        sexpr sx = (sexpr)p;
        alive_processes--;

        if (context->exitstatus != 0)
        {
            sx_write (stdio, cons (make_symbol ("failed"),
                                   cons (make_integer (context->exitstatus),
                                         cons (sx,
                                             sx_end_of_list))));

            exit (24);
        }

        sx_destroy (sx);
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
                 sx_xref (sx);
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

        sx_xref (wcdr);
        sx_destroy (workstack);
        workstack = wcdr;
    }
}

enum signal_callback_result cb_on_bad_signal(enum signal s, void *p)
{
    fprintf (stderr, "problematic signal received: %i\n", s);
    exit (s);

    return scr_keep;
}

static void loop_processes()
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

static void build (sexpr buildtargets)
{
    sexpr cursor = buildtargets;
    sexpr use_objects = sx_end_of_list;

    if (eolp(cursor))
    {
        tree_map (&targets, target_map_build, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);

        struct tree_node *node = tree_get_node_string(&targets, (char *)target);

        if (node != (struct tree_node *)0)
        {
            struct target *t = (struct target *)node_get_value(node);

            if (!eolp (t->use_objects))
            {
                sexpr cuo = t->use_objects;

                while (consp (cuo))
                {
                    sexpr tx = use_objects;
                    sexpr cuocar = car (cuo);
                    char doadd = 1;

                    while (consp (tx))
                    {
                        if (truep(equalp(cuocar, car (tx))))
                        {
                            doadd = 0;
                            break;
                        }
                        tx = cdr (tx);
                    }

                    tx = buildtargets;

                    if (doadd) while (consp (tx))
                    {
                        if (truep(equalp(cuocar, car (tx))))
                        {
                            doadd = 0;
                            break;
                        }
                        tx = cdr (tx);
                    }

                    if (doadd)
                    {
                        sx_xref (cuocar);
                        use_objects = cons (cuocar, use_objects);
                    }

                    cuo = cdr (cuo);
                }
            }
        }

        build_target (target);
        cursor = cdr(cursor);
    }

    cursor = use_objects;

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        build_target (sx_string (sxcar));
        cursor = cdr(cursor);
    }

    sx_destroy (use_objects);

    loop_processes();
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
        if (snprintf (buffer, BUFFERSIZE, "%s/lib/libcurie.sx", sx_string(i_destdir)),
            truep(initialise_libcurie_filename(buffer))) return;

        switch (i_fsl)
        {
            case fs_fhs:
                if (snprintf (buffer, BUFFERSIZE, "%s/usr/lib/libcurie.sx", sx_string(i_destdir)),
                    truep(initialise_libcurie_filename(buffer))) return;
                break;
            case fs_proper:
                if (snprintf (buffer, BUFFERSIZE, "%s/%s/%s/lib/libcurie.sx", sx_string(i_destdir), uname_os, uname_arch),
                    truep(initialise_libcurie_filename(buffer))) return;
                break;
        }
    }

    if (truep(initialise_libcurie_filename("/lib/libcurie.sx"))) return;

    switch (i_fsl)
    {
        case fs_fhs:
            if (truep(initialise_libcurie_filename("/usr/lib/libcurie.sx"))) return;
            if (snprintf (buffer, BUFFERSIZE, "/%s/%s/lib/libcurie.sx", uname_os, uname_arch), truep(initialise_libcurie_filename(buffer))) return;
            break;
        case fs_proper:
            if (snprintf (buffer, BUFFERSIZE, "/%s/%s/lib/libcurie.sx", uname_os, uname_arch), truep(initialise_libcurie_filename(buffer))) return;
            if (truep(initialise_libcurie_filename("/usr/lib/libcurie.sx"))) return;
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
                    case 'D':
                        i_debug = sx_true;
                        break;
                    case 'V':
                        i_valgrind = sx_true;
                        break;
                    case 'f':
                        i_fsl = fs_fhs;
                        break;
                    case 's':
                        i_fsl = fs_proper;
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

    switch (uname_toolchain)
    {
        case tc_gcc: initialise_toolchain_gcc(); break;
    }

    for (q = 0; uname_os[q] && "darwin"[q]; q++);
    if ((q == 6) && (uname_os[q] == 0)) i_os = os_darwin;
    for (q = 0; uname_os[q] && "linux"[q]; q++);
    if ((q == 5) && (uname_os[q] == 0)) i_os = os_linux;

    sym_library             = make_symbol ("library");
    sym_libraries           = make_symbol ("libraries");
    sym_test_cases          = make_symbol ("test-cases");
    sym_test_case_reference = make_symbol ("test-case-reference");
    sym_programme           = make_symbol ("programme");
    sym_hosted              = make_symbol ("hosted");
    sym_code                = make_symbol ("code");
    sym_headers             = make_symbol ("headers");
    sym_use_objects         = make_symbol ("use-objects");
    sym_assembly            = make_symbol ("assembly");
    sym_cpp                 = make_symbol ("C++");
    sym_c                   = make_symbol ("C");
    sym_link                = make_symbol ("link");
    sym_libc                = make_symbol ("libc");
    sym_libcurie            = make_symbol ("libcurie");
    sym_freestanding        = make_symbol ("freestanding");
    sym_freestanding_if_asm = make_symbol ("freestanding-if-assembly");
    sym_data                = make_symbol ("data");
    sym_description         = make_symbol ("description");
    sym_name                = make_symbol ("name");
    sym_version             = make_symbol ("version");
    sym_url                 = make_symbol ("url");
    str_bootstrap           = make_string ("bootstrap");
    str_curie               = make_string ("curie");
    str_curie_bootstrap     = make_string ("curie-bootstrap");
    str_static              = make_string ("-static");
    str_lc                  = make_string ("c");

    stdio                   = sx_open_stdio();

    multiplex_io();
    multiplex_process();
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

        sx_destroy (r);
    }

    sx_close_io (io);

    crosslink_objects ();

    build (buildtargets);
    ice_link (buildtargets);
    post_process (buildtargets);

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
