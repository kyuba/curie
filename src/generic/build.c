/*
 *  build.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 01/11/2008.
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

#include <stdlib.h>
#include <stdio.h>

#ifdef POSIX
#include <sys/utsname.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>

#define BUFFERSIZE 4096
#define UNAMELENGTH 128

enum toolchain
{
    tc_gcc
};

static char uname_os     [UNAMELENGTH] = "generic";
static char uname_arch   [UNAMELENGTH] = "generic";
static char uname_vendor [UNAMELENGTH] = "unknown";

static enum toolchain uname_toolchain;

static char archbuffer   [BUFFERSIZE];
static char *archprefix;

static sexpr sym_library   = sx_false;
static sexpr sym_programme = sx_false;
static sexpr sym_code      = sx_false;
static sexpr sym_assembly  = sx_false;
static sexpr sym_cpp       = sx_false;
static sexpr sym_c         = sx_false;

static struct sexpr_io *stdio;

static sexpr p_c_compiler;
static sexpr p_cpp_compiler;
static sexpr p_assembler;
static sexpr p_linker;

struct target {
    sexpr name;
    sexpr library;
    sexpr code;
};

enum fs_layout
{
    fs_fhs,
    fs_sheer
};

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
#ifdef POSIX
    else if ((r = find_in_permutations_arch (sx_string_dir_prefix_c ("posix", p), file)), stringp(r))
    {
        goto ret;
    }
#endif
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

static struct target *get_context()
{
    static struct memory_pool pool = MEMORY_POOL_INITIALISER (sizeof(struct target));
    struct target *context = get_pool_mem (&pool);

    context->code    = sx_false;
    context->library = sx_false;
    context->code    = sx_end_of_list;

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

static void build_object_gcc_assembly (const char *source, const char *target)
{
    const char *exec[] = { sx_string (p_assembler), "-c", source, "-o", target, (const char *)0};

    fprintf (stdout, "%s -c %s -o %s\n", sx_string (p_assembler), source, target);
}

static void build_object_gcc_c (const char *source, const char *target)
{
    const char *exec[] = { sx_string (p_c_compiler), "-c", source, "-o", target, (const char *)0};

    fprintf (stdout, "%s -c %s -o %s\n", sx_string (p_c_compiler), source, target);
}

static void build_object_gcc_cpp (const char *source, const char *target)
{
    const char *exec[] = { sx_string (p_cpp_compiler), "-c", source, "-o", target, (const char *)0};

    fprintf (stdout, "%s -c %s -o %s\n", sx_string (p_cpp_compiler), source, target);
}

static void build_object_gcc (sexpr type, sexpr source, sexpr target)
{
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

    switch (uname_toolchain)
    {
        case tc_gcc:
            build_object_gcc (type, source, target); break;
    }
}

static void do_build_target(struct target *t)
{
    sexpr c = t->code;
    fprintf (stdout, "building: %s\n", sx_string(t->name));

    while (consp (c))
    {
        build_object(car(c));

        c = cdr (c);
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

static void print_help(char *binaryname)
{
    fprintf (stdout,
        "Usage: %s [options] [targets]\n\n"
        "Options:\n"
        " -h          Print help and exit\n"
        " -t <chost>  Specify target CHOST\n\n"
        "The [targets] specify a list of things to build, according to the\n"
        "icemake.sx file located in the current working directory.\n\n",
        binaryname);
}

static void target_map_build (struct tree_node *node, void *u)
{
    do_build_target(node_get_value(node));
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
    char *x = getenv("PATH"), *y = buffer, *cur = x;

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

    if (snprintf (buffer, BUFFERSIZE, "%s-gcc", archprefix),
        (w = which (buffer)), stringp(w))
    {
        p_c_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-cc", archprefix),
             (w = which (buffer)), stringp(w))
    {
        p_c_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "gcc"),
             (w = which (buffer)), stringp(w))
    {
        p_c_compiler = w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "cc"),
             (w = which (buffer)), stringp(w))
    {
        p_c_compiler = w;
    }
    else
    {
        fprintf (stderr, "cannot find C compiler.\n");
        exit (21);
    }

    if (snprintf (buffer, BUFFERSIZE, "%s-g++", archprefix),
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

    if (snprintf (buffer, BUFFERSIZE, "%s-as", archprefix),
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

    if (snprintf (buffer, BUFFERSIZE, "%s-ld", archprefix),
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
    }
}

int main (int argc, char **argv)
{
    struct sexpr_io *io;
    sexpr r;
    struct tree targets = TREE_INITIALISER;
    int i = 1;
    char *target_architecture = (char *)0;
    sexpr buildtargets = sx_end_of_list;

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

        if (target_architecture[j] == 0)
        {
            fprintf (stderr, "Bad CHOST: %s\n", target_architecture);
            exit (14);
        }

        /* target_architecture => toolchain ID */

        uname_toolchain = tc_gcc;
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

        os = un.sysname;
        arch = un.machine;
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

    sym_library   = make_symbol ("library");
    sym_programme = make_symbol ("programme");
    sym_code      = make_symbol ("code");
    sym_assembly  = make_symbol ("assembly");
    sym_cpp       = make_symbol ("C++");
    sym_c         = make_symbol ("C");
    stdio = sx_open_stdio();

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

    if (eolp(buildtargets))
    {
        tree_map (&targets, target_map_build, (void *)0);
    }
    else while (consp(buildtargets))
    {
        build_target (&targets, sx_string(car(buildtargets)));
        buildtargets = cdr(buildtargets);
    }

    return 0;
}
