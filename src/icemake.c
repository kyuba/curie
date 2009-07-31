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

#include <curie/memory.h>
#include <curie/tree.h>
#include <curie/multiplex.h>
#include <curie/exec.h>
#include <curie/signal.h>
#include <curie/filesystem.h>
#include <curie/stack.h>
#include <curie/gc.h>

#include <string.h>
#include <stdlib.h>
#if !defined(_WIN32)
#include <unistd.h>
#endif
#include <stdio.h>

#ifdef POSIX
#include <sys/utsname.h>
#include <fcntl.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>

#include <icemake/icemake.h>

char uname_os     [UNAMELENGTH]        = "generic";
char uname_arch   [UNAMELENGTH]        = "generic";
char uname_vendor [UNAMELENGTH]        = "unknown";

enum toolchain uname_toolchain;
enum fs_layout i_fsl                   = fs_proper;
enum operating_system i_os             = os_unknown;
enum instruction_set i_is              = is_generic;

char  archbuffer [BUFFERSIZE];
char *archprefix                       = "generic-unknown-generic-generic";
char *tcversion                        = (char *)0;

static int alive_processes             = 0;
static unsigned int max_processes      = 1;

static int failures                    = 0;

sexpr co_freestanding                  = sx_false;

sexpr i_optimise_linking               = sx_false;
sexpr i_combine                        = sx_false;
sexpr i_static                         = sx_true;
sexpr i_debug                          = sx_false;
sexpr i_destdir                        = sx_false;
sexpr i_pname                          = sx_false;
sexpr i_destlibdir                     = sx_false;
sexpr i_dynamic_libraries              = sx_true;

sexpr workstack                        = sx_end_of_list;

sexpr do_tests                         = sx_false;
sexpr do_install                       = sx_false;
sexpr do_build_documentation           = sx_false;

static sexpr all_targets               = sx_end_of_list;
struct tree targets                    = TREE_INITIALISER;

static char **xenviron                 = (char **)0;

sexpr p_c_compiler                     = sx_false;
sexpr p_cpp_compiler                   = sx_false;
sexpr p_assembler                      = sx_false;
sexpr p_linker                         = sx_false;
sexpr p_archiver                       = sx_false;
sexpr p_diff                           = sx_false;

sexpr p_latex                          = sx_false;
sexpr p_pdflatex                       = sx_false;
sexpr p_doxygen                        = sx_false;

static sexpr gc_elements               = sx_end_of_list;

struct sexpr_io *stdio;

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

    switch (i_os)
    {
        case os_windows:
            snprintf (buffer, BUFFERSIZE, "%s\\%s", sx_string(p), sx_string(f));
            break;
        default:
            snprintf (buffer, BUFFERSIZE, "%s/%s", sx_string(p), sx_string(f));
            break;
    }

    return make_string(buffer);
}

sexpr sx_string_dir_prefix_c (char *f, sexpr p)
{
    char buffer[BUFFERSIZE];

    switch (i_os)
    {
        case os_windows:
            snprintf (buffer, BUFFERSIZE, "%s\\%s", sx_string(p), f);
            break;
        default:
            snprintf (buffer, BUFFERSIZE, "%s/%s", sx_string(p), f);
            break;
    }

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
        case tc_borland:
            if ((r = find_in_permutations_vendor (sx_string_dir_prefix_c ("borland", p), file)), stringp(r))
            {
                return r;
            }
            break;
        case tc_msvc:
            if ((r = find_in_permutations_vendor (sx_string_dir_prefix_c ("msvc", p), file)), stringp(r))
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

static sexpr find_code_pic_s (sexpr file)
{
    sexpr r = find_code_with_suffix (file, ".pic.s");

    return (falsep(r) ? find_code_s (file) : r);
}

static sexpr find_code_S (sexpr file)
{
    return find_code_with_suffix (file, ".S");
}

static sexpr find_code_pic_S (sexpr file)
{
    sexpr r = find_code_with_suffix (file, ".pic.S");

    return (falsep(r) ? find_code_S (file) : r);
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

    switch (uname_toolchain)
    {
        case tc_msvc:
            snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\%s.obj", archprefix, sx_string(name), sx_string(file));
            break;
        case tc_borland:
            snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\%s.obj", archprefix, sx_string(name), sx_string(file));
            {
                int i;
                for (i = 0; buffer[i]; i++)
                {
                    if (buffer[i] == '+')
                    {
                        buffer[i] = 'x';
                    }
                }
            }
            break;
        case tc_gcc:
            snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s.o", archprefix, sx_string(name), sx_string(file));
            break;
    }

    return make_string(buffer);
}

static sexpr generate_pic_object_file_name (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/%s.pic.o", archprefix, sx_string(name), sx_string(file));

    return make_string(buffer);
}

static sexpr generate_test_object_file_name (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    switch (uname_toolchain)
    {
        case tc_msvc:
            snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\tests\\%s.obj", archprefix, sx_string(name), sx_string(file));
            break;
        case tc_borland:
            snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\tests\\%s.obj", archprefix, sx_string(name), sx_string(file));
            {
                int i;

                for (i = 0; buffer[i]; i++)
                {
                    if (buffer[i] == '+')
                    {
                        buffer[i] = 'x';
                    }
                }
            }
            break;
        case tc_gcc:
            snprintf (buffer, BUFFERSIZE, "build/%s/%s/tests/%s.o", archprefix, sx_string(name), sx_string(file));
            break;
    }

    return make_string(buffer);
}

static sexpr generate_test_executable_file_name (sexpr name, sexpr file)
{
    char buffer[BUFFERSIZE];

    switch (i_os)
    {
        case os_windows:
            switch (uname_toolchain)
            {
                case tc_msvc:
                    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\tests\\%s.exe", archprefix, sx_string(name), sx_string(file));
                    break;
                case tc_borland:
                    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\tests\\%s.exe", archprefix, sx_string(name), sx_string(file));
                    {
                        int i;

                        for (i = 0; buffer[i]; i++)
                        {
                            if (buffer[i] == '+')
                            {
                                buffer[i] = 'x';
                            }
                        }
                    }
                    break;
                case tc_gcc:
                    snprintf (buffer, BUFFERSIZE, "build\\%s\\%s\\tests\\%s.exe", archprefix, sx_string(name), sx_string(file));
            }
            break;
        default:
            snprintf (buffer, BUFFERSIZE, "build/%s/%s/tests/%s", archprefix, sx_string(name), sx_string(file));
    }

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
        context->have_cpp = sx_true;

        return cons(sym_cpp, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list)));
    }
    else if (((r = find_code_c (subfile)), stringp(r)))
    {
        return cons(sym_c, cons (r, cons (generate_object_file_name(context->name, subfile), sx_end_of_list)));
    }

    return sx_false;
}

static sexpr find_code_highlevel_pic (struct target *context, sexpr file)
{
    sexpr r;
    char buffer[BUFFERSIZE];
    sexpr subfile;

    snprintf (buffer, BUFFERSIZE, "%s-highlevel", sx_string (file));
    subfile = make_string (buffer);

    if (((r = find_code_cpp (subfile)), stringp(r)))
    {
        context->have_cpp = sx_true;

        return cons(sym_cpp_pic, cons (r, cons (generate_pic_object_file_name(context->name, subfile), sx_end_of_list)));
    }
    else if (((r = find_code_c (subfile)), stringp(r)))
    {
        return cons(sym_c_pic, cons (r, cons (generate_pic_object_file_name(context->name, subfile), sx_end_of_list)));
    }

    return sx_false;
}

static void find_code (struct target *context, sexpr file)
{
    sexpr r;
    sexpr primus   = sx_false;
    sexpr secundus = sx_false;
    sexpr tertius  = sx_false;
    sexpr quartus  = sx_false;

    if (falsep(equalp(str_bootstrap, file)) || truep (co_freestanding))
    {
        if ((r = find_code_S (file)), stringp(r))
        {
            primus = cons(sym_preproc_assembly, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));

            if (truep(i_dynamic_libraries))
            {
                switch (uname_toolchain)
                {
                    case tc_gcc:
                        secundus = cons(sym_preproc_assembly_pic, cons (find_code_pic_S (file), cons (generate_pic_object_file_name(context->name, file), sx_end_of_list)));
                        break;
                    case tc_msvc:
                    case tc_borland:
                        break;
                }
            }

            tertius = find_code_highlevel     (context, file);
            if (truep(i_dynamic_libraries))
            {
                switch (uname_toolchain)
                {
                    case tc_gcc:
                        quartus = find_code_highlevel_pic (context, file);
                        break;
                    case tc_msvc:
                    case tc_borland:
                        break;
                }
            }
        }
        else if ((r = find_code_s (file)), stringp(r))
        {
            primus = cons(sym_assembly, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));

            if (truep(i_dynamic_libraries))
            {
                switch (uname_toolchain)
                {
                    case tc_gcc:
                        secundus = cons(sym_assembly_pic, cons (find_code_pic_s (file), cons (generate_pic_object_file_name(context->name, file), sx_end_of_list)));
                        break;
                    case tc_msvc:
                    case tc_borland:
                        break;
                }
            }

            tertius = find_code_highlevel     (context, file);
            if (truep(i_dynamic_libraries))
            {
                switch (uname_toolchain)
                {
                    case tc_gcc:
                        quartus = find_code_highlevel_pic (context, file);
                        break;
                    case tc_msvc:
                    case tc_borland:
                        break;
                }
            }
        }
    }

    if (falsep(primus))
    {
        if (((r = find_code_cpp (file)), stringp(r)))
        {
            context->have_cpp = sx_true;

            primus = cons(sym_cpp, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));

            if (truep(i_dynamic_libraries))
            {
                switch (uname_toolchain)
                {
                    case tc_gcc:
                        secundus = cons(sym_cpp_pic, cons (r, cons (generate_pic_object_file_name(context->name, file), sx_end_of_list)));
                        break;
                    case tc_msvc:
                    case tc_borland:
                        break;
                }
            }
        }
        else if (((r = find_code_c (file)), stringp(r)))
        {
            primus = cons(sym_c, cons (r, cons (generate_object_file_name(context->name, file), sx_end_of_list)));

            if (truep(i_dynamic_libraries))
            {
                switch (uname_toolchain)
                {
                    case tc_gcc:
                        secundus = cons(sym_c_pic, cons (r, cons (generate_pic_object_file_name(context->name, file), sx_end_of_list)));
                        break;
                    case tc_msvc:
                    case tc_borland:
                        break;
                }
            }
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
        if (!falsep(tertius))
        {
            context->bootstrap = cons (tertius, context->bootstrap);
        }
        if (!falsep(quartus))
        {
            context->bootstrap = cons (quartus, context->bootstrap);
        }
    }
    else
    {
        context->code = cons (primus, context->code);
        if (!falsep(secundus))
        {
            context->code = cons (secundus, context->code);
        }
        if (!falsep(tertius))
        {
            context->code = cons (tertius, context->code);
        }
        if (!falsep(quartus))
        {
            context->code = cons (quartus, context->code);
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
    define_symbol (sym_man1, "man1");
    define_symbol (sym_man2, "man2");
    define_symbol (sym_man3, "man3");
    define_symbol (sym_man4, "man4");
    define_symbol (sym_man5, "man5");
    define_symbol (sym_man6, "man6");
    define_symbol (sym_man7, "man7");
    define_symbol (sym_man8, "man8");

    sexpr r;

    if ((r = find_documentation_tex (file)), stringp(r))
    {
        context->documentation = cons(cons(sym_tex, cons (file, r)), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".1")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man1, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".2")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man2, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".3")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man3, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".4")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man4, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".5")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man5, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".6")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man6, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".7")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man7, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (file, ".8")), stringp(r))
    {
        context->documentation = cons(cons(sym_man, cons (file, cons(r, cons (sym_man8, sx_end_of_list)))), context->documentation);
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
    context->programme      = sx_false;
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
    context->have_cpp       = sx_false;

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

                    if (i_os != os_windows)
                    {
                        i_dynamic_libraries = sx_false;
                    }

                    break;
                }
            }
        }

        definition = cdr (definition);
    }

    snprintf (buffer, BUFFERSIZE, "build/%s", archprefix);

    mkdir (buffer, 0755);

    snprintf (buffer, BUFFERSIZE, "build/%s/%s", archprefix, sx_string(context->name));

    mkdir (buffer, 0755);

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/tests", archprefix, sx_string(context->name));

    mkdir (buffer, 0755);
    
    if (uname_toolchain == tc_borland)
    {
        int i;

        snprintf (buffer, BUFFERSIZE, "build/%s/%s", archprefix, sx_string(context->name));
        for (i = 0; buffer[i]; i++)
        {
            if (buffer[i] == '+')
            {
                buffer[i] = 'x';
            }
        }

        mkdir (buffer, 0755);

        snprintf (buffer, BUFFERSIZE, "build/%s/%s/tests", archprefix, sx_string(context->name));
        for (i = 0; buffer[i]; i++)
        {
            if (buffer[i] == '+')
            {
                buffer[i] = 'x';
            }
        }

        mkdir (buffer, 0755);
    }

    if (uname_toolchain == tc_gcc)
    {
        if (truep(i_combine) && consp(context->code))
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
            }

            context->code = ccur;
        }

        if (truep (context->have_cpp))
        {
            context->libraries = cons (str_supcpp, context->libraries);
            if (i_os == os_darwin)
              context->libraries = cons (str_gcc_eh, context->libraries);
        }
        else
        {
            context->libraries = cons (str_gcc, context->libraries);
        }
    }

    if (truep(context->hosted) && falsep (context->have_cpp) &&
        (uname_toolchain == tc_gcc))
    {
        switch (i_os)
        {
            case os_windows:
                context->libraries = cons (str_kernel32, context->libraries);
                context->libraries = cons (str_mingw32, context->libraries);
                context->libraries = cons (str_coldname, context->libraries);
                context->libraries = cons (str_mingwex, context->libraries);
                context->libraries = cons (str_msvcrt, context->libraries);
                break;
            default:
                context->libraries = cons (str_lc, context->libraries);
        }
    }

    if (truep(context->use_curie))
    {
        if (falsep (context->hosted) && falsep (context->have_cpp))
        {
            context->libraries = cons (str_curie_bootstrap, context->libraries);
        }

        context->libraries = cons (str_curie, context->libraries);

        if (falsep(equalp(str_curie, context->name)))
        {
            context->olibraries = cons (str_curie, context->olibraries);
        }
    }

    all_targets = cons (context->name, all_targets);
}

static void tag_target_for_gc (struct target *context)
{
    gc_elements = cons (context->name, gc_elements);
    gc_elements = cons (context->library, gc_elements);
    gc_elements = cons (context->programme, gc_elements);
    gc_elements = cons (context->libraries, gc_elements);
    gc_elements = cons (context->olibraries, gc_elements);
    gc_elements = cons (context->hosted, gc_elements);
    gc_elements = cons (context->use_curie, gc_elements);
    gc_elements = cons (context->code, gc_elements);
    gc_elements = cons (context->test_cases, gc_elements);
    gc_elements = cons (context->test_reference, gc_elements);
    gc_elements = cons (context->bootstrap, gc_elements);
    gc_elements = cons (context->headers, gc_elements);
    gc_elements = cons (context->use_objects, gc_elements);
    gc_elements = cons (context->dname, gc_elements);
    gc_elements = cons (context->description, gc_elements);
    gc_elements = cons (context->dversion, gc_elements);
    gc_elements = cons (context->durl, gc_elements);
    gc_elements = cons (context->documentation, gc_elements);
    gc_elements = cons (context->have_cpp, gc_elements);
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

    tag_target_for_gc (context);

    return context;
}

static struct target *create_programme (sexpr definition)
{
    struct target *context = get_context();

    context->name = car(definition);
    context->programme = sx_true;

    process_definition (context, cdr(definition));

    tag_target_for_gc (context);

    return context;
}

static struct target *create_documentation (sexpr definition)
{
    struct target *context = get_context();

    context->name = car(definition);

    process_definition (context, cdr(definition));

    tag_target_for_gc (context);

    return context;
}

static void print_help(char *binaryname)
{
    fprintf (stdout,
        "Usage: %s [options] [targets]\n"
        "\n"
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
        " -o           Don't link dynamic libraries.\n"
        " -S           Enforce a static link (default).\n"
        " -R           Enforce a dynamic link.\n"
        " -j <num>     Spawn <num> processes simultaneously.\n"
        " -D           Use debug code, if available.\n"
        " -x           Build documentation (if possible).\n"
        "\n"
        "The [targets] specify a list of things to build, according to the\n"
        "icemake.sx file located in the current working directory.\n"
        "\n",
        binaryname);
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

static sexpr cwhich (char *programme)
{
    char buffer[BUFFERSIZE];
    char *x = getenv("PATH"), *y = buffer;

    if (x == (char *)0) return sx_false;

    while ((*x != 0) && (y < (buffer + BUFFERSIZE - 1)))
    {
        if (
#if !defined(_WIN32)
        (*x == ':')
#else
        (*x == ';')
#endif
        ) {
            char subbuffer[BUFFERSIZE];
            struct stat st;

            *y = 0;
            y = buffer;

            snprintf (subbuffer, BUFFERSIZE, "%s/%s", buffer, programme);

            if (stat (subbuffer, &st) == 0)
            {
                return make_string (subbuffer);
            }

            snprintf (subbuffer, BUFFERSIZE, "%s\\%s.exe", buffer, programme);

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
          (w = cwhich (buffer)), stringp(w)))
    {
        return w;
    }
    else if (snprintf (buffer, BUFFERSIZE, "%s-%s", archprefix, programme),
             (w = cwhich (buffer)), stringp(w))
    {
        return w;
    }
    else if ((w = cwhich (programme)), stringp(w))
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

    p_diff = xwhich ("diff");
    if (falsep(p_diff))
    {
        sexpr out;
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_diff, sx_end_of_list))));
    }
}

static void initialise_toolchain_borland()
{
    p_c_compiler = xwhich ("bcc32");
    if (falsep(p_c_compiler))
    {
        fprintf (stderr, "cannot find C compiler.\n");
        exit (21);
    }

    p_linker = p_c_compiler;
    p_assembler = p_c_compiler;
    p_cpp_compiler = p_c_compiler;

    p_archiver = xwhich ("tlib");
    if (falsep(p_archiver))
    {
        fprintf (stderr, "cannot find archiver.\n");
        exit (25);
    }

    p_diff = xwhich ("diff");
    if (falsep(p_diff))
    {
        sexpr out;
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_diff, sx_end_of_list))));
    }
}

static void initialise_toolchain_msvc()
{
    p_c_compiler = xwhich ("cl");
    if (falsep(p_c_compiler))
    {
        fprintf (stderr, "cannot find C compiler.\n");
        exit (21);
    }

    p_linker = p_c_compiler;
    p_assembler = p_c_compiler;
    p_cpp_compiler = p_c_compiler;

    p_archiver = xwhich ("lib");
    if (falsep(p_archiver))
    {
        fprintf (stderr, "cannot find archiver.\n");
        exit (25);
    }

    p_diff = xwhich ("diff");
    if (falsep(p_diff))
    {
        sexpr out;
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_diff, sx_end_of_list))));
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
    }

    p_pdflatex = xwhich ("pdflatex");
    if (falsep(p_pdflatex))
    {
        sx_write (stdio,
                  (out = cons (sym_missing_programme,
                               cons (sym_pdflatex, sx_end_of_list))));
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
    }
}

static void spawn_stack_items(void (*)(struct exec_context *, void *));

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
        else
        {
            sx_write (stdio, cons (sym_completed, cons (sx, sx_end_of_list)));
        }

        free_exec_context (context);
    }
}

static void process_on_death_nokill(struct exec_context *context, void *p)
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

            failures++;
        }
        else
        {
            sx_write (stdio, cons (sym_completed, cons (sx, sx_end_of_list)));
        }

        free_exec_context (context);
    }
}

static void spawn_item (sexpr sx, void (*f)(struct exec_context *, void *))
{
    sexpr cur = sx, cf = car (sx);
    struct exec_context *context;
    const char *wdir = (const char *)0;
    char odir[BUFFERSIZE];
    int c = 0, exsize;
    char **ex;

    sx_write (stdio, cons (sym_execute, cons (sx, sx_end_of_list)));

    if (truep(equalp(cf, sym_chdir)))
    {
        sexpr cfcdr = cdr (cur);
        sexpr cfcdar = car (cfcdr);

        wdir = sx_string (cfcdar);
        if ((char *)getcwd (odir, BUFFERSIZE) == (char *)0)
        {
            return;
        }

        sx = cdr (cfcdr);
    }

    while (consp (cur))
    {
        c++;
        cur = cdr (cur);
    }
    c++;

    exsize = sizeof(char*)*c;
    ex = aalloc (exsize);

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

    if (wdir != (const char *)0) { chdir (wdir); }
    context = execute (EXEC_CALL_NO_IO | EXEC_CALL_PURGE, ex, xenviron);
    if (wdir != (const char *)0) { chdir (odir); }

    sx = car (sx);

    switch (context->pid)
    {
        case -1: fprintf (stderr, "failed to spawn subprocess\n");
                 exit (22);
        case 0:  fprintf (stderr, "failed to execute binary image\n");
                 exit (23);
        default: alive_processes++;
                 gc_elements = cons (sx, gc_elements);
                 multiplex_add_process (context, f, (void *)sx);
                 afree (exsize, ex);
                 return;
    }
}

static void spawn_stack_items (void (*f)(struct exec_context *, void *))
{
    while (consp (workstack) && (alive_processes < max_processes))
    {
        sexpr wcdr = cdr (workstack);

        spawn_item (car (workstack), f);

        workstack = wcdr;
    }
}

enum signal_callback_result cb_on_bad_signal(enum signal s, void *p)
{
    fprintf (stderr, "problematic signal received: %i\n", s);
    exit (s);

    return scr_keep;
}

static sexpr initialise_libcurie_filename (char *filename)
{
    struct sexpr_io *io;
    sexpr r;
    struct stat st;

    if (stat(filename, &st) != 0) return sx_false;

    io = sx_open_io(io_open_read(filename), io_open_null);

    while (!eofp(r = sx_read (io)))
    {
        if (truep(equalp(r, sym_freestanding)))
        {
            co_freestanding = sx_true;
        }
        else if (truep(equalp(r, sym_hosted)))
        {
            co_freestanding = sx_false;
            i_dynamic_libraries = sx_false;
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

void count_print_items()
{
    int count = 0;
    sexpr cur;

    for (cur = workstack; consp (cur); count++, cur = cdr (cur));

    if (count > 0)
    {
        sx_write (stdio, cons (sym_items_total,
                               cons (make_integer (count), sx_end_of_list)));
    }
}

void loop_processes()
{
    count_print_items();

    spawn_stack_items (process_on_death);

    while (alive_processes > 0)
    {
        multiplex();
        spawn_stack_items (process_on_death);
    }

    sx_write (stdio, cons (sym_phase, cons (sym_completed, sx_end_of_list)));
}

void loop_processes_nokill()
{
    count_print_items();

    spawn_stack_items (process_on_death_nokill);

    while (alive_processes > 0)
    {
        multiplex();
        spawn_stack_items (process_on_death_nokill);
    }

    sx_write (stdio, cons (sym_phase, cons (sym_completed, sx_end_of_list)));
}

#if defined(_WIN32)
int main (int argc, char **argv)
#else
int main (int argc, char **argv, char **environ)
#endif
{
    struct sexpr_io *io;
    sexpr r;
    int i = 1, q;
    char *target_architecture = (char *)getenv("CHOST");
    sexpr buildtargets = sx_end_of_list;
    sexpr in_dynamic_libraries = sx_nil;
    struct stat st;

    initialise_stack ();
    gc_add_root (&gc_elements);
    terminate_on_allocation_errors();

#if defined(_WIN32)
#else
    xenviron = environ;
#endif

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
                    case 'S':
                        i_static = sx_true;
                        break;
                    case 'R':
                        i_static = sx_false;
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
                    case 'o':
                        in_dynamic_libraries = sx_false;
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
                            unsigned int j;

                            max_processes = 0;

                            for (j = 0; s[j]; j++)
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
/*        if (target_architecture[0] == 'g')
        {
            uname_toolchain = tc_gcc;
        }
        else*/ if (target_architecture[0] == 'b')
        {
            uname_toolchain = tc_borland;
        }
        else if (target_architecture[0] == 'm')
        {
            uname_toolchain = tc_msvc;
        }
        else
        {
            uname_toolchain = tc_gcc;
        }
    }
    else
    {
        char *toolchain = "unknown";
        int j;

        if (!falsep(cwhich("cl")))
        {
            uname_toolchain = tc_msvc;
        }
        else if (!falsep(cwhich("bcc32")))
        {
            uname_toolchain = tc_borland;
        }
        else /* if nothing specific is found, guess it's gcc*/
        {
            uname_toolchain = tc_gcc;
        }

#if defined(POSIX)
        struct utsname un;

        if (uname (&un) >= 0)
        {
            write_uname_element(un.sysname, uname_os,   UNAMELENGTH - 1);
            if ((un.machine[0] == 'i') &&
                ((un.machine[1] == '3') || (un.machine[1] == '4') ||
                 (un.machine[1] == '5') || (un.machine[1] == '6')) &&
                (un.machine[2] == '8') && (un.machine[3] == '6'))
            {
                write_uname_element("x86-32", uname_arch, UNAMELENGTH - 1);
            }
            else
            {
                write_uname_element(un.machine, uname_arch, UNAMELENGTH - 1);
            }
        }

#elif defined(_WIN32)
        write_uname_element ("windows", uname_os, UNAMELENGTH-1);
        write_uname_element ("microsoft", uname_vendor, UNAMELENGTH-1);

#if defined(__ia64__) || defined(__ia64) || defined(_M_IA64) || defined(_M_X64)
        write_uname_element ("x86-64", uname_arch, UNAMELENGTH-1);
#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(_M_IX86) || defined(__INTEL__)
        write_uname_element ("x86-32", uname_arch, UNAMELENGTH-1);
#endif
#endif

        switch (uname_toolchain)
        {
            case tc_gcc:     toolchain = "gnu";     break;
            case tc_borland: toolchain = "borland"; break;
            case tc_msvc:    toolchain = "msvc";    break;
        }

        snprintf (archbuffer, BUFFERSIZE, "%s-%s-%s-%s",
                  uname_arch, uname_vendor, uname_os, toolchain);

        for (j = 0; archbuffer[j]; j++)
        {
            archbuffer[j] = tolower(archbuffer[j]);
        }

        archprefix = archbuffer;
    }

    stdio                   = sx_open_stdout();

    switch (uname_toolchain)
    {
        case tc_gcc:     initialise_toolchain_gcc();     break;
        case tc_borland: initialise_toolchain_borland(); break;
        case tc_msvc:    initialise_toolchain_msvc();    break;
    }

    initialise_toolchain_tex ();
    initialise_toolchain_doxygen ();

    for (q = 0; uname_os[q] && (uname_os[q] == "darwin"[q]); q++);
    if ((q == 6) && (uname_os[q] == 0)) i_os = os_darwin;
    for (q = 0; uname_os[q] && (uname_os[q] == "linux"[q]); q++);
    if ((q == 5) && (uname_os[q] == 0)) i_os = os_linux;
    for (q = 0; uname_os[q] && (uname_os[q] == "windows"[q]); q++);
    if (((q == 7) && (uname_os[q] == 0)) || (q == 3)) i_os = os_windows;

    for (q = 0; uname_arch[q] && (uname_arch[q] == "arm"[q]); q++);
    if ((q == 3) || ((q == 4) && (uname_arch[q] == 0))) i_is = is_arm;

    if (i_os == os_darwin)
    {
        in_dynamic_libraries = sx_false;
    }

    gc_elements = cons (p_c_compiler, gc_elements);
    gc_elements = cons (p_cpp_compiler, gc_elements);
    gc_elements = cons (p_assembler, gc_elements);
    gc_elements = cons (p_linker, gc_elements);
    gc_elements = cons (p_archiver, gc_elements);
    gc_elements = cons (p_diff, gc_elements);
    gc_elements = cons (p_latex, gc_elements);
    gc_elements = cons (p_c_compiler, gc_elements);
    gc_elements = cons (i_destdir, gc_elements);
    gc_elements = cons (i_pname, gc_elements);
    gc_elements = cons (i_destlibdir, gc_elements);
    gc_elements = cons (p_doxygen, gc_elements);
    gc_elements = cons (buildtargets, gc_elements);

    multiplex_io();
/*    multiplex_all_processes();*/
    multiplex_signal_primary();
    multiplex_process();
    multiplex_sexpr();

    multiplex_add_signal (sig_segv, cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_int,  cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_term, cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_bus,  cb_on_bad_signal, (void *)0);

    multiplex_add_sexpr (stdio, (void *)0, (void *)0);

    initialise_libcurie();

    if (nilp(in_dynamic_libraries))
    {
        if (i_os == os_windows)
        {
            i_dynamic_libraries = sx_true;
        }
        else if (falsep(co_freestanding))
        {
            i_dynamic_libraries = sx_false;
        }
    }
    else
    {
        i_dynamic_libraries = in_dynamic_libraries;
    }

    io = sx_open_io (io_open_read("icemake.sx"), io_open_null);

    while (!eofp(r = sx_read (io)))
    {
        struct target *t = (struct target *)0;

        if (nexp(r)) continue;
        if (consp(r)) {
            sexpr sxcar = car (r);

            gc_elements = cons (r, gc_elements);

            if (truep(equalp(sxcar, sym_library)))
            {
                t = create_library (cdr (r));
            }
            else if (truep(equalp(sxcar, sym_programme)))
            {
                t = create_programme (cdr (r));
            }
            else if (truep(equalp(sxcar, sym_documentation)))
            {
                t = create_documentation (cdr (r));
            }
        }

        if (t != (struct target *)0)
        {
            tree_add_node_string_value (&targets, (char *)sx_string(t->name), t);
        }
    }

    sx_close_io (io);

    if (!consp(buildtargets))
    {
        buildtargets = all_targets;
        gc_elements = cons (buildtargets, gc_elements);
    }

    gc_invoke();

    sx_write (stdio, cons (sym_targets, buildtargets));

    crosslink_objects ();
    gc_invoke();
    build (buildtargets);
    gc_invoke();
    ice_link (buildtargets);
    gc_invoke();
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

    if (!eolp (buildtargets))
    {
        sx_write (stdio, cons (sym_completed, cons (sym_targets, buildtargets)));
    }

    return failures;
}
