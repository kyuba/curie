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

#include <stdlib.h>
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
static sexpr sym_programme             = sx_false;
static sexpr sym_code                  = sx_false;
static sexpr sym_headers               = sx_false;
static sexpr sym_link                  = sx_false;
static sexpr sym_use_objects           = sx_false;
static sexpr sym_assembly              = sx_false;
static sexpr sym_cpp                   = sx_false;
static sexpr sym_c                     = sx_false;

static int alive_processes             = 0;
static int files_open                  = 0;
static int max_processes               = 1;

static struct sexpr_io *stdio;

static sexpr workstack                 = sx_end_of_list;

static enum fs_layout i_fsl            = fs_proper;
static sexpr i_destdir                 = sx_false;

static char **xenviron;

static sexpr p_c_compiler;
static sexpr p_cpp_compiler;
static sexpr p_assembler;
static sexpr p_linker;
static sexpr p_archiver;
static sexpr p_archive_indexer;

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

#ifdef VALGRIND
    if ((r = find_in_permutations_os (sx_string_dir_prefix_c ("valgrind", p), file)), stringp(r))
    {
        goto ret;
    }
    else
#endif
#ifdef DEBUG
    if ((r = find_in_permutations_os (sx_string_dir_prefix_c ("debug", p), file)), stringp(r))
    {
        goto ret;
    }
    else
#endif
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

static void find_code (struct target *context, sexpr file)
{
    sexpr r;

    if (((r = find_code_S (file)), stringp(r)) ||
        ((r = find_code_s (file)), stringp(r)))
    {
        char buffer[BUFFERSIZE];
        sexpr subfile;

        context->code = cons (cons(sym_assembly, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list))), context->code);

        snprintf (buffer, BUFFERSIZE, "%s-highlevel", sx_string (file));
        subfile = make_string (buffer);

        if (((r = find_code_cpp (subfile)), stringp(r)))
        {
            context->code = cons (cons(sym_cpp, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list))), context->code);
        }
        else if (((r = find_code_c (subfile)), stringp(r)))
        {
            context->code = cons (cons(sym_c, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list))), context->code);
        }

        sx_destroy (subfile);
    }
    else if (((r = find_code_cpp (file)), stringp(r)))
    {
        context->code = cons (cons(sym_cpp, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list))), context->code);
    }
    else if (((r = find_code_c (file)), stringp(r)))
    {
        context->code = cons (cons(sym_c, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list))), context->code);
    }
    else
    {
        fprintf (stderr, "missing code file: %s\n", sx_string(file));
        exit(20);
    }
}

static void find_header (struct target *context, sexpr file)
{
    sexpr r;

    if ((r = find_header_h (context->name, file)), stringp(r))
    {
        context->headers = cons (cons(r, sx_end_of_list), context->headers);
    }
    else
    {
        fprintf (stderr, "missing header file: %s\n", sx_string(file));
        exit(21);
    }
}

static struct target *get_context()
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER (sizeof(struct target));
    struct target *context = get_pool_mem (&pool);

    context->library     = sx_false;
    context->code        = sx_end_of_list;
    context->headers     = sx_end_of_list;
    context->use_objects = sx_end_of_list;

    return context;
}

static void process_definition (struct target *context, sexpr definition)
{
    char buffer[BUFFERSIZE];

    while (consp(definition))
    {
        sexpr sxcar = car (definition);
        sexpr sxcaar = car (sxcar);

        if (truep(equalp(sxcaar, sym_code)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                find_code (context, car (sxc));

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
        else if (truep(equalp(sxcaar, sym_use_objects)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                context->use_objects = cons (car(sxc), context->use_objects);

                sxc = cdr (sxc);
            }
        }


        definition = cdr (definition);
    }

    snprintf (buffer, BUFFERSIZE, "build/%s", sx_string(context->name));

    mkdir (buffer, 0755);

    snprintf (buffer, BUFFERSIZE, "build/%s/%s", sx_string(context->name), archprefix);

    mkdir (buffer, 0755);
}

static struct target *create_library (sexpr definition)
{
    struct target *context = get_context();

    context->name = car(definition);
    context->library = sx_true;
    sx_xref (context->name);

    process_definition (context, cdr(definition));

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

static sexpr prepend_cflags_gcc (sexpr x)
{
    return x;
}

static sexpr prepend_cxxflags_gcc (sexpr x)
{
    return x;
}

static void build_object_gcc_assembly (const char *source, const char *target)
{
    workstack
        = cons (cons (p_assembler,
                    cons (make_string (source),
                      cons (make_string ("-o"),
                        cons (make_string(target), sx_end_of_list))))
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

static void link_library_gcc (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    struct stat res, st;
    char havelib;
    sexpr sx = sx_end_of_list;

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", sx_string(name), archprefix, sx_string(name));

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

    if (havelib) {
        return;
    }

    workstack
        = cons (cons (p_archiver,
                  cons (make_string ("-r"),
                    cons (make_string (buffer),
                          sx)))
                , workstack);
}

static void link_programme_gcc (sexpr name, sexpr code, struct target *t)
{
    char buffer[BUFFERSIZE];
    struct stat res, st;
    char havebin;
    sexpr sx = sx_end_of_list;

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s", sx_string(name), archprefix, sx_string(name));

    havebin = (stat (buffer, &res) == 0);

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

    if (havebin) {
        return;
    }

    workstack
        = cons (cons (p_linker,
                  cons (make_string ("-o"),
                    cons (make_string (buffer),
                          sx)))
                , workstack);
}

static void post_process_library_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", sx_string(name), archprefix, sx_string(name));

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

static void install_library_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", sx_string(name), archprefix, sx_string(name));

    workstack
        = cons (cons (make_string (buffer),
                      get_library_install_path(name))
                , workstack);
}

static void install_programme_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s", sx_string(name), archprefix, sx_string(name));

    workstack
        = cons (cons (make_string (buffer),
                      get_programme_install_path(name))
                , workstack);
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
}

static void do_link_target(struct target *t)
{
    sexpr c = t->code;
    
    if (truep(t->library))
    {
        link_library (t->name, c, t);
    }
    else
    {
        link_programme (t->name, c, t);
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

static void do_install_target(struct target *t)
{    
    if (truep(t->library))
    {
        install_library (t->name, t);
    }
    else
    {
        install_programme (t->name, t);
    }
}

static void build_target (struct tree *targets, const char *target)
{
    struct tree_node *node = tree_get_node_string(targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_build_target (node_get_value(node));
    }
}

static void link_target (struct tree *targets, const char *target)
{
    struct tree_node *node = tree_get_node_string(targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_link_target (node_get_value(node));
    }
}

static void post_process_target (struct tree *targets, const char *target)
{
    struct tree_node *node = tree_get_node_string(targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_post_process_target (node_get_value(node));
    }
}

static void install_target (struct tree *targets, const char *target)
{
    struct tree_node *node = tree_get_node_string(targets, (char *)target);

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
        " -i <destdir> Install resulting binaries to the given destdir\n"
        " -f           Use the FHS layout for installation\n"
        " -s           Use the default FS layout for installation\n\n"
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

static void target_map_install (struct tree_node *node, void *u)
{
    do_install_target(node_get_value(node));
}

static void target_map_cross_link (struct tree_node *node, void *u)
{
    struct tree *t = (struct tree *)u;
    struct target *target = (struct target *) node_get_value (node);
    
    if (!eolp (target->use_objects))
    {
        sexpr cur = target->use_objects;
        
        while (consp (cur))
        {
            sexpr o = car (cur);
            struct tree_node *n1
                = tree_get_node_string (t, (char *)sx_string (o));
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

static void initialise_toolchain_gcc()
{
    char buffer[BUFFERSIZE];
    sexpr w;

    if ((tcversion != (char *)0) &&
        (snprintf (buffer, BUFFERSIZE, "%s-gcc-%s", archprefix, tcversion),
         (w = which (buffer)), stringp(w)))
    {
        p_linker = w;
        p_c_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-gcc", archprefix),
        (w = which (buffer)), stringp(w))
    {
        p_linker = w;
        p_c_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-cc", archprefix),
             (w = which (buffer)), stringp(w))
    {
        p_linker = w;
        p_c_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "gcc"),
             (w = which (buffer)), stringp(w))
    {
        p_linker = w;
        p_c_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "cc"),
             (w = which (buffer)), stringp(w))
    {
        p_linker = w;
        p_c_compiler = w;
    }
    else
    {
        fprintf (stderr, "cannot find C compiler.\n");
        exit (21);
    }

    if ((tcversion != (char *)0) &&
        (snprintf (buffer, BUFFERSIZE, "%s-g++-%s", archprefix, tcversion),
         (w = which (buffer)), stringp(w)))
    {
        p_cpp_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-g++", archprefix),
        (w = which (buffer)), stringp(w))
    {
        p_cpp_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "g++"),
             (w = which (buffer)), stringp(w))
    {
        p_cpp_compiler = w;
    }
    else
    {
        fprintf (stderr, "cannot find C++ compiler.\n");
        exit (22);
    }

    if ((tcversion != (char *)0) &&
        (snprintf (buffer, BUFFERSIZE, "%s-as-%s", archprefix, tcversion),
         (w = which (buffer)), stringp(w)))
    {
        p_assembler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-as", archprefix),
        (w = which (buffer)), stringp(w))
    {
        p_assembler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "as"),
             (w = which (buffer)), stringp(w))
    {
        p_assembler = w;
    }
    else
    {
        fprintf (stderr, "cannot find assembler.\n");
        exit (23);
    }

/*    if ((tcversion != (char *)0) &&
        (snprintf (buffer, BUFFERSIZE, "%s-ld-%s", archprefix, tcversion),
         (w = which (buffer)), stringp(w)))
    {
        p_linker = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-ld", archprefix),
        (w = which (buffer)), stringp(w))
    {
        p_linker = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "ld"),
             (w = which (buffer)), stringp(w))
    {
        p_linker = w;
    }
    else
    {
        fprintf (stderr, "cannot find linker.\n");
        exit (23);
    }*/

    if ((tcversion != (char *)0) &&
        (snprintf (buffer, BUFFERSIZE, "%s-ar-%s", archprefix, tcversion),
         (w = which (buffer)), stringp(w)))
    {
        p_archiver = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-ar", archprefix),
        (w = which (buffer)), stringp(w))
    {
        p_archiver = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "ar"),
             (w = which (buffer)), stringp(w))
    {
        p_archiver = w;
    }
    else
    {
        fprintf (stderr, "cannot find archiver.\n");
        exit (23);
    }

    if ((tcversion != (char *)0) &&
        (snprintf (buffer, BUFFERSIZE, "%s-ranlib-%s", archprefix, tcversion),
         (w = which (buffer)), stringp(w)))
    {
        p_archive_indexer = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-ranlib", archprefix),
        (w = which (buffer)), stringp(w))
    {
        p_archive_indexer = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "ranlib"),
             (w = which (buffer)), stringp(w))
    {
        p_archive_indexer = w;
    }
    else
    {
        fprintf (stderr, "cannot find archive indexer.\n");
        exit (23);
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

    io_write (out,
              (in->buffer) + (in->position),
              (in->length) - (in->position));

    in->position = in->length;
}

static void install_close (struct io *io, void *aux)
{
    struct io *out = (struct io *)aux;

    files_open--;

    multiplex_del_io (out);
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

            files_open ++;

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

static void crosslink_objects (struct tree *targets)
{
    tree_map (targets, target_map_cross_link, (void *)targets);
}

static void build (sexpr buildtargets, struct tree *targets)
{
    sexpr cursor = buildtargets;
    sexpr use_objects = sx_end_of_list;

    if (eolp(cursor))
    {
        tree_map (targets, target_map_build, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);

        struct tree_node *node = tree_get_node_string(targets, (char *)target);

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

        build_target (targets, target);
        cursor = cdr(cursor);
    }

    cursor = use_objects;

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        build_target (targets, sx_string (sxcar));
        cursor = cdr(cursor);
    }

    sx_destroy (use_objects);

    loop_processes();
}

static void link (sexpr buildtargets, struct tree *targets)
{
    sexpr cursor = buildtargets;
    if (eolp(cursor))
    {
        tree_map (targets, target_map_link, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        link_target (targets, sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}

static void post_process (sexpr buildtargets, struct tree *targets)
{
    sexpr cursor = buildtargets;
    if (eolp(cursor))
    {
        tree_map (targets, target_map_post_process, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        post_process_target (targets, sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}

static void install (sexpr buildtargets, struct tree *targets)
{
    sexpr cursor = buildtargets;

    if (falsep(i_destdir))
    {
        return;
    }

    if (eolp(cursor))
    {
        tree_map (targets, target_map_install, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        install_target (targets, sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_install();
}

int main (int argc, char **argv, char **environ)
{
    struct sexpr_io *io;
    sexpr r;
    struct tree targets = TREE_INITIALISER;
    int i = 1;
    char *target_architecture = (char *)0;
    sexpr buildtargets = sx_end_of_list;

    xenviron = environ;

    set_resize_mem_recovery_function(rm_recover);
    set_get_mem_recovery_function(gm_recover);

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
                    case 'i':
                        if (xn < argc)
                        {
                            i_destdir = make_string(argv[xn]);
                            xn++;
                        }
                        break;
                    case 'f':
                        i_fsl = fs_fhs;
                        break;
                    case 's':
                        i_fsl = fs_proper;
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

    sym_library     = make_symbol ("library");
    sym_programme   = make_symbol ("programme");
    sym_code        = make_symbol ("code");
    sym_headers     = make_symbol ("headers");
    sym_use_objects = make_symbol ("use-objects");
    sym_assembly    = make_symbol ("assembly");
    sym_cpp         = make_symbol ("C++");
    sym_c           = make_symbol ("C");
    sym_link        = make_symbol ("link");

    stdio           = sx_open_stdio();

    multiplex_io();
    multiplex_process();
    multiplex_sexpr();
    multiplex_signal();

    multiplex_add_signal (sig_segv, cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_int,  cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_term, cb_on_bad_signal, (void *)0);

    multiplex_add_sexpr (stdio, (void *)0, (void *)0);

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

    crosslink_objects (&targets);

    build (buildtargets, &targets);
    link (buildtargets, &targets);
    post_process (buildtargets, &targets);
    install (buildtargets, &targets);

    return 0;
}
