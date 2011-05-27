/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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

#if !defined(_WIN32)
#include <sys/utsname.h>
#endif

#include <icemake/icemake.h>
#if !defined(NOVERSION)
#include <icemake/data.h>
#endif

#include <curie/regex.h>

#include <curie/main.h>

#include <curie/memory.h>
#include <curie/multiplex.h>
#include <curie/exec.h>
#include <curie/signal.h>
#include <curie/filesystem.h>
#include <curie/stack.h>

#include <sievert/shell.h>

#include <ctype.h>

sexpr i_destdir                        = sx_false;
sexpr i_destlibdir                     = sx_false;

sexpr do_build_documentation           = sx_false;

sexpr p_latex                          = sx_false;
sexpr p_pdflatex                       = sx_false;
sexpr p_doxygen                        = sx_false;

static void parse_add_definition
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td,
     sexpr definition);

struct process_data
{
    sexpr command;
    struct icemake *icemake;
    int *failures;
};

static sexpr toolchain_patterns = sx_end_of_list;

static sexpr sx_string_dir_prefix
    (struct toolchain_descriptor *td, sexpr f, sexpr p)
{
    if (td->operating_system == os_windows)
    {
        return sx_join (p, str_backslash, f);
    }

    return sx_join (p, str_slash, f);
}

static sexpr sx_string_dir_prefix_c
    (struct toolchain_descriptor *td, const char *f, sexpr p)
{
    return sx_string_dir_prefix (td, make_string (f), p);
}

static sexpr f_exist_add (sexpr f, sexpr lis)
{
    return truep (filep (f)) ? cons (f, lis) : lis;
}

static sexpr permutate_paths_vendor
    (struct toolchain_descriptor *td, sexpr p, sexpr lis)
{
    lis = f_exist_add (sx_string_dir_prefix_c (td, td->uname_vendor, p), lis);
    lis = f_exist_add (p, lis);

    return lis;
}

static sexpr permutate_paths_toolchain
    (struct toolchain_descriptor *td, sexpr p, sexpr lis)
{
    if (td->uname_toolchain != (const char *)0)
    {
        lis = permutate_paths_vendor
            (td, sx_string_dir_prefix_c (td, td->uname_toolchain, p), lis);
    }
    lis = permutate_paths_vendor (td, p, lis);

    return lis;
}

static sexpr permutate_paths_arch
    (struct toolchain_descriptor *td, sexpr p, sexpr lis)
{
    lis = permutate_paths_toolchain
        (td, sx_string_dir_prefix_c (td, td->uname_arch, p), lis);
    lis = permutate_paths_toolchain (td, p, lis);

    return lis;
}

static sexpr permutate_paths_os
    (struct toolchain_descriptor *td, sexpr p, sexpr lis)
{
    lis = permutate_paths_arch
        (td, sx_string_dir_prefix_c (td, td->uname_os, p), lis);
    lis = permutate_paths_arch
        (td, sx_string_dir_prefix_c (td, "posix", p), lis);
    lis = permutate_paths_arch
        (td, sx_string_dir_prefix_c (td, "ansi", p), lis);
    lis = permutate_paths_arch
        (td, sx_string_dir_prefix_c (td, "generic", p), lis);
    lis = permutate_paths_arch
        (td, p, lis);

    return lis;
}

sexpr icemake_permutate_paths
    (struct toolchain_descriptor *td, sexpr p)
{
    sexpr lis = sx_end_of_list;

    lis = permutate_paths_os (td, p, lis);

    return lis;
}

void mkdir_pi (sexpr path)
{
    char buffer[BUFFERSIZE];
    const char *p = sx_string (path);
    int i;

    for (i = 0; (p[i] != 0) && (i < (BUFFERSIZE - 2)); i++)
    {
        if (((p[i] == '/') || (p[i] == '\\')) && (i > 0))
        {
            buffer[i] = 0;

            if (falsep (filep (make_string (buffer))))
            {
                mkdir (buffer, 0755);
            }
        }

        buffer[i] = p[i];
    }
}

static void mkdir_p (sexpr path)
{
    mkdir_pi (path);
    mkdir (sx_string (path), 0755);
}

static const char *c_getenv (const char *var)
{
    const char **c = (const char **)curie_environment, *d;
    int i;

    while ((d = (*c)) != (const char *)0)
    {
        d = (*c);

        for (i = 0; d[i] == var[i]; i++);

        if ((d[i] == '=') && (var[i] == 0))
        {
            return d + i + 1;
        }

        c++;
    }

    return (const char *)0;
}

sexpr prepend_flags_from_environment (sexpr x, const char *var)
{
    const char *f = c_getenv (var);

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

        while (consp (t)) { x = cons (car(t), x); t = cdr (t); }
    }

    return x;
}

char *mangle_path_borland (char *b)
{
    int i;

    for (i = 0; b[i]; i++)
    {
        if (b[i] == '+')
        {
            b[i] = 'x';
        }
    }

    return b;
}

sexpr mangle_path_borland_sx (sexpr b)
{
    char buffer[BUFFERSIZE];
    int i;
    const char *bx = sx_string (b);

    for (i = 0; (i < (BUFFERSIZE - 2)) && bx[i]; i++)
    {
        buffer[i] = bx[i];
    }

    buffer[i] = 0;

    return make_string (mangle_path_borland (buffer));
}

static sexpr lowercase (sexpr s)
{
    char buffer[BUFFERSIZE], c;
    int i;
    const char *sx = sx_string (s);

    for (i = 0; (i < (BUFFERSIZE - 2)) && sx[i]; i++)
    {
        c = sx[i];

        buffer[i] = ((c >= 'A') && (c <= 'Z'))
                  ? ('a' + (c - 'A'))
                  : c;
    }

    buffer[i] = 0;

    return make_string (buffer);
}

static sexpr find_in_permutations
    (struct toolchain_descriptor *td, sexpr p, sexpr file)
{
    sexpr cur = sx_reverse (icemake_permutate_paths (td, p));
    sexpr r, q;

    while (consp (cur))
    {
        q = car (cur);
        r = sx_string_dir_prefix (td, file, q);

        if (truep (filep (r)))
        {
            return r;
        }

        cur = cdr (cur);
    }

    return sx_false;
}

static sexpr find_code_with_suffix
    (struct toolchain_descriptor *td, sexpr path, sexpr file, char *s)
{
    return find_in_permutations
        (td, sx_join (path, str_src, sx_nil),
         sx_join (file, make_string (s), sx_nil));
}

static sexpr find_code_sx
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_code_with_suffix (td, path, file, ".sx");
}

static sexpr find_code_c
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_code_with_suffix (td, path, file, ".c");
}

static sexpr find_code_cpp
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_code_with_suffix (td, path, file, ".c++");
}

static sexpr find_code_s
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_code_with_suffix (td, path, file, ".s");
}

static sexpr find_code_pic_s
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    sexpr r = find_code_with_suffix (td, path, file, ".pic.s");

    return (falsep(r) ? find_code_s (td, path, file) : r);
}

static sexpr find_code_S
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_code_with_suffix (td, path, file, ".S");
}

static sexpr find_code_pic_S
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    sexpr r = find_code_with_suffix (td, path, file, ".pic.S");

    return (falsep(r) ? find_code_S (td, path, file) : r);
}

static sexpr find_code_def
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_code_with_suffix (td, path, file, ".def");
}

static sexpr find_code_rc
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_code_with_suffix (td, path, file, ".rc");
}

static sexpr find_header_with_suffix
    (struct toolchain_descriptor *td, sexpr path, sexpr name, sexpr file,
     char *s) 
{
    return find_in_permutations
        (td, sx_join (path, str_include, sx_nil),
         sx_join (name, str_slash, sx_join (file, make_string (s), sx_nil)));
}

static sexpr find_header_h
    (struct toolchain_descriptor *td, sexpr path, sexpr name, sexpr file)
{
    return find_header_with_suffix (td, path, name, file, ".h");
}

static sexpr generate_object_file_name (sexpr file, struct target *t)
{
    return icemake_decorate_file (t, ft_object, fet_build_file, file);
}

static sexpr generate_resource_file_name (struct target *t)
{
    return icemake_decorate_file (t, ft_resource, fet_build_file, t->name);
}

static sexpr generate_pic_object_file_name (sexpr file, struct target *t)
{
    return icemake_decorate_file (t, ft_object_pic, fet_build_file, file);
}

static sexpr find_code_highlevel (struct target *context, sexpr file)
{
    sexpr r, subfile = sx_join (file, str_dhighlevel, sx_nil);

    if (((r = find_code_cpp (context->toolchain, context->base, subfile)),
        stringp(r)))
    {
        context->options |= ICEMAKE_HAVE_CPP;

        return sx_list3 (sym_cpp, r,
                         generate_object_file_name (subfile, context));

    }
    else if (((r = find_code_c (context->toolchain, context->base, subfile)),
             stringp(r)))
    {
        return sx_list3 (sym_c, r,
                         generate_object_file_name (subfile, context));
    }

    return sx_false;
}

static sexpr find_code_highlevel_pic (struct target *context, sexpr file)
{
    sexpr r, subfile = sx_join (file, str_dhighlevel, sx_nil);

    if (((r = find_code_cpp (context->toolchain, context->base, subfile)),
        stringp(r)))
    {
        context->options |= ICEMAKE_HAVE_CPP;

        return sx_list3 (sym_cpp_pic, r,
                         generate_pic_object_file_name (subfile, context));
    }
    else if (((r = find_code_c (context->toolchain, context->base, subfile)),
             stringp(r)))
    {
        return sx_list3 (sym_c_pic, r,
                         generate_pic_object_file_name (subfile, context));
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

    if (consp (file))
    {
        sexpr ch = car (file), d = context->icemake->alternatives;

        while (consp (d))
        {
            sexpr c = car (d);

            if (truep (equalp (car (c), ch)))
            {
                sexpr co = cdr (file), e = cdr (c);

                while (consp (co))
                {
                    if (truep (equalp (e, car (co))))
                    {
                        file = e;
                        break;
                    }

                    co = cdr (co);
                }

                if (consp (file))
                {
                    context->icemake->visualiser.on_error
                        (context->icemake, ie_invalid_choice, file);
                }

                break;
            }

            d = cdr (d);
        }

        if (consp (file))
        {
            file = car (cdr (file));
        }
    }

    if ((r = find_code_S (context->toolchain, context->base, file)), stringp(r))
    {
        primus = sx_list3 (sym_preproc_assembly, r,
                           generate_object_file_name (file, context));

        if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            (context->toolchain->toolchain == tc_gcc) &&
            !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            secundus = sx_list3 (sym_preproc_assembly_pic,
                                 find_code_pic_S (context->toolchain,
                                                  context->base, file),
                                 generate_pic_object_file_name (file, context));
        }

        tertius = find_code_highlevel (context, file);

        if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            (context->toolchain->toolchain == tc_gcc) &&
            !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            quartus = find_code_highlevel_pic (context, file);
        }
    }
    else if ((r = find_code_s (context->toolchain, context->base, file)),
             stringp(r))
    {
        primus = sx_list3 (sym_assembly, r,
                           generate_object_file_name (file, context));

        if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            (context->toolchain->toolchain == tc_gcc) &&
            !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            secundus = sx_list3 (sym_assembly_pic,
                                 find_code_pic_s (context->toolchain,
                                                  context->base, file),
                                 generate_pic_object_file_name (file, context));
        }

        tertius = find_code_highlevel (context, file);

        if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            (context->toolchain->toolchain == tc_gcc) &&
            !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            quartus = find_code_highlevel_pic (context, file);
        }
    }

    if (falsep(primus))
    {
        if (((r = find_code_cpp (context->toolchain, context->base, file)),
            stringp(r)))
        {
            context->options |= ICEMAKE_HAVE_CPP;

            primus = sx_list3 (sym_cpp, r,
                               generate_object_file_name (file, context));

            if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
                (context->toolchain->toolchain == tc_gcc) &&
                !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
            {
                secundus = sx_list3 (sym_cpp_pic, r,
                                     generate_pic_object_file_name
                                        (file, context));
            }
        }
        else if (((r = find_code_c (context->toolchain, context->base, file)),
                 stringp(r)))
        {
            primus = sx_list3 (sym_c, r,
                               generate_object_file_name (file, context));

            if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
                (context->toolchain->toolchain == tc_gcc) &&
                !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
            {
                secundus = sx_list3 (sym_c_pic, r,
                                     generate_pic_object_file_name
                                        (file, context));
            }
        }
        else
        {
            context->icemake->visualiser.on_error
                (context->icemake, ie_missing_code_file, file);
        }
    }

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

static void find_header (struct target *context, sexpr file)
{
    sexpr r;

    if ((r = find_header_h (context->toolchain, context->base, context->name, 
                            file)),
        stringp(r))
    {
        context->headers = cons (cons(file, cons (r, sx_end_of_list)),
                                 context->headers);
    }
    else
    {
        context->icemake->visualiser.on_error
            (context->icemake, ie_missing_header, file);
    }
}

static sexpr find_documentation_with_suffix
    (struct toolchain_descriptor *td, sexpr path, sexpr file, char *s)
{
    return find_in_permutations
        (td, sx_join (path, str_documentation, sx_nil),
         sx_join (file, make_string (s), sx_nil));
}

static sexpr find_documentation_tex
    (struct toolchain_descriptor *td, sexpr path, sexpr file)
{
    return find_documentation_with_suffix (td, path, file, ".tex");
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

    if ((r = find_documentation_tex (context->toolchain, context->base, file)),
        stringp(r))
    {
        context->documentation
            = cons(cons(sym_tex, cons (file, r)), context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".1")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man1, sx_end_of_list)))),
                         context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".2")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man2, sx_end_of_list)))),
                         context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".3")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man3, sx_end_of_list)))),
                         context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".4")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man4, sx_end_of_list)))),
                         context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".5")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man5, sx_end_of_list)))),
                         context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".6")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man6, sx_end_of_list)))),
                         context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".7")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man7, sx_end_of_list)))),
                         context->documentation);
    }
    else if ((r = find_documentation_with_suffix
                      (context->toolchain, context->base, file, ".8")),
             stringp(r))
    {
        context->documentation
            = cons(cons(sym_man,
                   cons (file, cons(r, cons (sym_man8, sx_end_of_list)))),
                         context->documentation);
    }
    else
    {
        context->icemake->visualiser.on_error
            (context->icemake, ie_missing_documentation, file);
    }
}

static sexpr find_data (struct target *context, sexpr file)
{
    sexpr r = sx_join (context->base, str_data, sx_nil);

    if ((r = find_in_permutations (context->toolchain, r, file)),
        !stringp(r))
    {
        context->icemake->visualiser.on_error
            (context->icemake, ie_missing_data, file);
    }

    return r;
}

static struct target *get_context
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td)
{
    static struct memory_pool pool
        = MEMORY_POOL_INITIALISER (sizeof(struct target));
    struct target *context = get_pool_mem (&pool);

    context->libraries        = sx_end_of_list;
    context->deffile          = sx_false;
    context->olibraries       = sx_end_of_list;
    context->code             = sx_end_of_list;
    context->test_reference   = sx_end_of_list;
    context->headers          = sx_end_of_list;
    context->data             = sx_end_of_list;
    context->dname            = sx_false;
    context->description      = sx_false;
    context->dversion         = sx_false;
    context->durl             = sx_false;
    context->documentation    = sx_end_of_list;
    context->buildnumber      = make_integer (0);
    context->options          = 0;
    context->toolchain        = td;
    context->icemake          = im;
    context->base             = path;

    if (td->operating_system == os_windows)
    {
        context->buildbase    = str_build_backslash;
    }
    else
    {
        context->buildbase    = str_build_slash;
    }

    return context;
}

static void process_definition
    (struct target *context, sexpr definition)
{
    sexpr sx;
    
    /* find extra options to process first */
    if ((sx = find_code_sx (context->toolchain, context->base, context->name)),
        stringp(sx))
    {
        const char *s = sx_string (sx);
        struct sexpr_io *in = sx_open_i (io_open_read(s));
        sexpr r;

        while (!eofp ((r = sx_read (in))))
        {
            if (truep (equalp (r, sym_freestanding)))
            {
                context->toolchain->options |= ICEMAKE_OPTION_FREESTANDING;
            }
            else if (truep (equalp (r, sym_hosted)))
            {
                context->toolchain->options &= ~ICEMAKE_OPTION_FREESTANDING;
                    
                if (context->toolchain->operating_system != os_windows)
                {
                    context->icemake->options &=
                        ~ICEMAKE_OPTION_DYNAMIC_LINKING;
                }
            }
        }

        sx_close_io (in);

        if ((context->toolchain->operating_system != os_windows) &&
            !(context->toolchain->options & ICEMAKE_OPTION_FREESTANDING))
        {
            context->icemake->options &= ~ICEMAKE_OPTION_DYNAMIC_LINKING;
        }
    }

    while (consp(definition))
    {
        sexpr sxcar = car (definition);
        sexpr sxcaar = car (sxcar);

        if (truep(equalp(sxcar, sym_no_shared_library)))
        {
            context->options |= ICEMAKE_NO_SHARED_LIBRARY;
        }
        else if (truep(equalp(sxcar, sym_hosted)))
        {
            context->options |= ICEMAKE_HOSTED;
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
            context->options |= ICEMAKE_USE_CURIE;
        }
        else if (truep(equalp(sxcaar, sym_code)))
        {
            sexpr sxc = cdr (sxcar);

            while (consp (sxc))
            {
                find_code (context, car (sxc));
                
                sxc = cdr (sxc);
            }

            if ((context->toolchain->operating_system == os_windows) &&
                (context->toolchain->toolchain == tc_msvc))
            {
                sexpr r = find_code_rc
                    (context->toolchain, context->base, context->name);

                if (stringp (r))
                {
                    context->code =
                        cons (cons(sym_resource, cons (r,
                                cons (generate_resource_file_name (context),
                                      sx_end_of_list))),
                              context->code);
                } 
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
        else if (truep(equalp(sxcaar, sym_raw_c)) ||
                 truep(equalp(sxcaar, sym_cpio_c)))
        {
            context->code = sx_set_add (context->code, sxcar);
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
                    d = cons (cons (filename, find_data (context, filename)),d);

                    sxc = cdr (sxc);
                }

                d = cons (name, d);

                context->data = cons (d, context->data);
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

        definition = cdr (definition);
    }

    mkdir_p (icemake_decorate_file (context, ft_other, fet_build_file, sx_nil));

    mkdir_p (sx_join (context->buildbase, context->toolchain->uname,
                      str_sinclude));

    mkdir_p (icemake_decorate_file (context, ft_header, fet_build_file,
                                    sx_nil));

    context->headers =
        cons (cons (str_version,
                    cons (icemake_decorate_file (context, ft_header,
                                                 fet_build_file, str_version),
                          sx_end_of_list)),
              context->headers);

    if (context->toolchain->toolchain == tc_gcc)
    {
        if ((context->options & ICEMAKE_HAVE_CPP) &&
             (context->options & ICEMAKE_HOSTED))
        {
            context->libraries = cons (str_supcpp, context->libraries);
            if (context->toolchain->operating_system == os_darwin)
            {
                context->libraries = cons (str_gcc_eh, context->libraries);
            }
        }
        else
        {
            context->libraries = cons (str_gcc, context->libraries);
        }

        if ((context->options & ICEMAKE_HOSTED) &&
            !(context->options & ICEMAKE_HAVE_CPP))
        {
            switch (context->toolchain->operating_system)
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
    }

    if (context->options & ICEMAKE_USE_CURIE)
    {
        if (!(context->options & ICEMAKE_HOSTED))
        {
            context->libraries = cons (str_curie_bootstrap, context->libraries);
        }

        context->libraries = cons (str_curie, context->libraries);

        if (falsep(equalp(str_curie, context->name)))
        {
            context->olibraries = cons (str_curie, context->olibraries);
        }
    }
}

static struct target *create_archive
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td,
     sexpr definition)
{
    struct target *context = get_context (im, path, td);

    context->name        = car(definition);
    context->dname       = str_archive;
    context->description = str_archive;
    context->dversion    = str_1;

    context->options |= ICEMAKE_LIBRARY | ICEMAKE_USE_CURIE;

    process_definition (context, cdr(definition));

    return context;
}

static struct target *create_library
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td,
     sexpr definition)
{
    struct target *context = get_context (im, path, td);

    context->name = car(definition);

    context->options |= ICEMAKE_LIBRARY;

    process_definition (context, cdr(definition));

    if (falsep(equalp(str_curie, context->name)))
    {
        context->libraries = cons (context->name, context->libraries);
    }
    context->deffile = find_code_def (td, context->base, context->name);

    return context;
}

static struct target *create_programme
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td,
     sexpr definition)
{
    struct target *context = get_context (im, path, td);

    context->name = car(definition);

    context->options |= ICEMAKE_PROGRAMME;

    process_definition (context, cdr(definition));

    return context;
}

static struct target *create_test_case
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td,
     sexpr definition)
{
    struct target *context = get_context (im, path, td);

    context->name        = car(definition);
    context->dname       = str_test_case;
    context->description = str_test_case;
    context->dversion    = str_1;

    context->options |= ICEMAKE_PROGRAMME | ICEMAKE_TEST_CASE;

    process_definition (context, cdr(definition));

    return context;
}

static struct target *create_documentation
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td,
     sexpr definition)
{
    struct target *context = get_context (im, path, td);

    context->name = car(definition);

    process_definition (context, cdr(definition));

    return context;
}

static void combine_code (struct target *context, sexpr type, sexpr object_file)
{
    sexpr ccur = sx_end_of_list;
    sexpr cur = context->code;

    sexpr clist = sx_end_of_list;

    do {
        sexpr sxcar = car (cur);
        sexpr sxcaar = car (sxcar);
        sexpr sxcadar = car (cdr (sxcar));

        if (truep(equalp(sxcaar, type)))
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
        ccur = cons (cons (type, cons (clist, cons (object_file,
                     sx_end_of_list))), ccur);
    }

    context->code = ccur;
}

static void target_map_merge (struct tree_node *node, void *u)
{
    struct target *context = (struct target *)node_get_value(node), *tcontext;
    sexpr cur, ca, nc, ttype;
    const char *target;
    struct icemake *im = (struct icemake *)u;

    if (context->options & ICEMAKE_PROGRAMME)
        /* dont merge objects for libraries */
    {
        for (cur = context->libraries; consp (cur); cur = cdr (cur))
        {
            ca = car (cur);
            target = sx_string (ca);

            if ((node = tree_get_node_string(&(im->targets), (char *)target))
                != (struct tree_node *)0)
            {
                tcontext = (struct target *)node_get_value (node);
                context->code = sx_set_merge (context->code, tcontext->code);
                context->libraries = sx_set_remove (context->libraries, ca);
            }
        }
    }

    if (context->options & ICEMAKE_PROGRAMME)
        /* dont need PIC objects for programmes */
    {
        nc = sx_end_of_list;
        for (cur = context->code; consp(cur); cur = cdr (cur))
        {
            ca = car (cur);
            ttype = car (ca);

            if (falsep(equalp(ttype, sym_assembly_pic)) &&
                falsep(equalp(ttype, sym_preproc_assembly_pic)) &&
                falsep(equalp(ttype, sym_c_pic)) &&
                falsep(equalp(ttype, sym_cpp_pic)))
            {
                nc = sx_set_add (nc, ca);
            }
        }

        context->code = nc;
    }
}

static void target_map_combine (struct tree_node *node, void *u)
{
    struct target *context = (struct target *)node_get_value(node);

    if ((context->icemake->options & ICEMAKE_OPTION_COMBINE) &&
        consp(context->code) &&
        (context->toolchain->toolchain == tc_gcc))
    {
        combine_code (context, sym_c,
                      generate_object_file_name
                        (str_combined_c_source, context));

        combine_code (context, sym_c_pic,
                      generate_pic_object_file_name
                        (str_combined_c_source, context));
    }
}

static void merge_contexts ( struct icemake *im )
{
    tree_map (&(im->targets), target_map_merge,   (void *)im);
    tree_map (&(im->targets), target_map_combine, (void *)im);
}
 
static void write_uname_element (const char *source, char *target, int tlen)
{
    int  i = 0;
    char c = 0;

    while (source[i] != 0)
    {
        c = source[i];

        if ((c >= '0') && (c <= '9'))
        {
            target[i] = c;
        }
        else if ((c >= 'a') && (c <= 'z'))
        {
            target[i] = c;
        }
        else if ((c >= 'A') && (c <= 'Z'))
        {
            target[i] = (c - 'A') + 'a';
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

sexpr icemake_which
    (const struct toolchain_descriptor *td, const char *programme,
     const char *env)
{
    sexpr w, p = make_string (programme);

    if (programme[0] == '/')
    {
        w = make_string (programme);

        if (truep (filep (w)))
        {
            return w;
        }
    }

    if (env != (const char *)0)
    {
        env = c_getenv (env);

        if (env != (const char *)0)
        {
            w = icemake_which (td, env, 0);

            if (stringp (w))
            {
                return w;
            }
        }
    }

    if (td != (struct toolchain_descriptor *)0)
    {
        if ((w = which (sx_join (td->original_toolchain, str_dash, p))),
            stringp(w))
        {
            return w;
        }
    }
    
    w = which (p);

    return w;
}

static void initialise_toolchain_tex (struct toolchain_descriptor *td)
{
    p_latex    = icemake_which (td, "latex",    "LATEX");
    p_pdflatex = icemake_which (td, "pdflatex", "PDFLATEX");
}

static void initialise_toolchain_doxygen (struct toolchain_descriptor *td)
{
    p_doxygen = icemake_which (td, "doxygen",   "DOXYGEN");
}

static void spawn_stack_items (struct icemake *im, int *fl);

static void process_on_death
    (struct exec_context *context, void *p)
{
    struct process_data *d = (struct process_data *)p;

    if (context->status == ps_terminated)
    {
        sexpr sx = (sexpr)d->command;
        (d->icemake->alive_processes)--;

        d->icemake->visualiser.on_command_done
            (d->icemake, sx);

        if (context->exitstatus != 0)
        {
            d->icemake->visualiser.on_warning
                (d->icemake, ie_programme_failed,
                 cons (make_integer (context->exitstatus), sx));

            d->failures++;
        }

        free_exec_context (context);
    }

    free_pool_mem (p);
}

static void spawn_item
    (sexpr sx, void (*f)(struct exec_context *, void *), struct icemake *im,
     int *fl)
{
    sexpr cur = sx, cf = car (sx);
    struct exec_context *context;
    const char *wdir = (const char *)0;
    char odir[BUFFERSIZE];
    int c = 0, exsize;
    char **ex;
    static struct memory_pool pool =
        MEMORY_POOL_INITIALISER(sizeof (struct process_data));
    struct process_data *pd;

    im->visualiser.on_command (im, cons (sym_execute, sx));

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

    for (c = 0; consp (cur); cur = cdr (cur))
    {
        c++;
    }
    c++;

    exsize = sizeof(char*)*c;
    ex = aalloc (exsize);

    for (c = 0, cur = sx; consp (cur); cur = cdr (cur))
    {
        sexpr sxcar = car (cur);
        ex[c] = (char *)sx_string (sxcar);
        c++;
    }
    ex[c] = (char *)0;

    if (wdir != (const char *)0)
    {
        (void)chdir (wdir);
    }

    context = execute (EXEC_CALL_NO_IO | EXEC_CALL_PURGE, ex,
                       curie_environment);

    if (wdir != (const char *)0)
    {
        (void)chdir (odir);
    }

    pd = (struct process_data *)get_pool_mem (&pool);

    pd->command  = car(sx);
    pd->icemake  = im;
    pd->failures = fl;

    switch (context->pid)
    {
        case -1: im->visualiser.on_error
                    (im, ie_failed_to_spawn_subprocess, sx_end_of_list);
                 cexit(-1);
                 break;
        case 0:  im->visualiser.on_error
                    (im, ie_failed_to_execute_binary_image, sx_end_of_list);
                 cexit(0);
                 break;
        default: (im->alive_processes)++;
                 multiplex_add_process (context, f, (void *)pd);
                 afree (exsize, ex);
    }
}

static void spawn_stack_items (struct icemake *im, int *fl)
{
    while (consp (im->workstack) && ((im->alive_processes) < im->max_processes))
    {
        sexpr spec = car (im->workstack);
        sexpr sca  = car (spec);

        im->visualiser.on_command (im, spec);

        if (truep (equalp (sym_install, sca)) ||
            truep (equalp (sym_symlink, sca)))
        {
            if (im->install_file !=
                    (int (*)(struct icemake *, sexpr))0)
            {
                im->install_file (im, cdr (spec));
            }
        }
        else if (falsep (equalp (sym_phase, sca)) &&
                 falsep (equalp (sym_completed, sca)) &&
                 falsep (equalp (sym_targets, sca)))
        {
            spawn_item (spec, process_on_death, im, fl);
        }

        im->workstack = cdr (im->workstack);
    }
}

enum signal_callback_result cb_on_bad_signal(enum signal s, void *p)
{
    cexit (-1);

    return scr_keep;
}

static int icemake_count_print_items (struct icemake *im )
{
    int count = 0;
    sexpr cur, ccur, ca;

    for (cur = im->workstack; consp (cur); cur = cdr (cur))
    {
        ccur = car (cur);
        ca   = car (ccur);

        if (falsep (equalp (ca, sym_phase)) &&
            falsep (equalp (ca, sym_targets)) &&
            falsep (equalp (ca, sym_completed)))
        {
            count++;
        }
    }

    im->visualiser.items (im, count);

    return count;
}

int icemake_loop_processes (struct icemake *im)
{
    int failures = 0;

/*    im->workstack = sx_reverse (im->workstack); */

    icemake_count_print_items (im);

    spawn_stack_items (im, &failures);

    while ((im->alive_processes) > 0)
    {
        multiplex();
        spawn_stack_items (im, &failures);
    }

    return failures;
}

static void read_metadata ( struct icemake *im )
{
    sexpr c = im->roots, t, r;
    struct sexpr_io *io;
    while (consp (c))
    {
        t = sx_join (car (c), str_metadata_sx, sx_nil);
        io = sx_open_i (io_open_read (sx_string(t)));

        while (!eofp(r = sx_read (io)) && !nexp (r))
        {
            struct target *t = (struct target *)0;

            if (consp(r))
            {
                sexpr sxcar = car (r);
                struct tree_node *node =
                    tree_get_node_string(&(im->targets), (char *)sx_string(sxcar));

                if (node != (struct tree_node *)0)
                {
                    t = (struct target *)node_get_value (node);

                    t->buildnumber = car (cdr (r));
                }
            }
        }
        sx_close_io (io);

        c = cdr (c);
    }
}

struct save_metadata_d
{
    struct sexpr_io *io;
    sexpr base;
};

static void target_map_save_metadata (struct tree_node *node, void *i)
{
    struct save_metadata_d *d = (struct save_metadata_d *)i;
    struct target *t = (struct target *)node_get_value (node);

    if (truep (equalp (t->base, d->base)))
    {
        sx_write (d->io, cons (t->name, cons (t->buildnumber, sx_end_of_list)));
    }
}

static void save_metadata ( struct icemake *im )
{
    sexpr c = im->roots, t;
    struct save_metadata_d d;

    while (consp (c))
    {
        d.base = car (c);
        t = sx_join (d.base, str_metadata_sx, sx_nil);
        d.io = sx_open_o (io_open_write (sx_string(t)));

        tree_map (&(im->targets), target_map_save_metadata, (void *)(&d));

        sx_close_io (d.io);
        c = cdr (c);
    }
}

int icemake_default_architecture
    (int (*with_data)(const char *, void *), void *aux)
{
    const char *toolchain = c_getenv ("CHOST");

    if (toolchain == (const char *)0)
    {
#if !defined(_WIN32)
        struct utsname un;
#endif

        char ar_t[UNAMELENGTH] = "generic";
        char os_t[UNAMELENGTH] = "generic";
        char ve_t[UNAMELENGTH] = "unknown";
        sexpr uname_t = sx_false;

        toolchain = "unknown";

        if (!falsep(which(str_cl)))
        {
            toolchain = "msvc";
        }
        else if (!falsep(which(str_bcc32)))
        {
            toolchain = "borland";
        }
        else /* if nothing specific is found, guess it's gcc*/
        {
            toolchain = "gnu";
        }

#if !defined(_WIN32)
        if (uname (&un) >= 0)
        {
            write_uname_element(un.sysname, os_t, UNAMELENGTH - 1);
            write_uname_element(un.machine, ar_t, UNAMELENGTH - 1);
        }
#else

        write_uname_element ("windows",   os_t, UNAMELENGTH-1);
        write_uname_element ("microsoft", ve_t, UNAMELENGTH-1);

#if defined(__ia64__) || defined(__ia64) || defined(_M_IA64) || defined(_M_X64)
        write_uname_element ("x86-64", ar_t, UNAMELENGTH-1);
#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(_M_IX86) || defined(__INTEL__)
        write_uname_element ("x86-32", ar_t, UNAMELENGTH-1);
#endif
#endif

        uname_t = lowercase (sx_join (make_string (ar_t), str_dash,
                             sx_join (make_string (ve_t), str_dash,
                             sx_join (make_string (os_t), str_dash,
                                      make_string (toolchain)))));

        return with_data ((char *)sx_string(uname_t), aux);
    }
    else
    {
        return with_data ((char *)toolchain, aux);
    }
}

int icemake_prepare_toolchain
    (const char *name,
     int (*with_data)(struct toolchain_descriptor *, void *), void *aux)
{
    struct toolchain_descriptor td =
        { tc_generic, os_generic, is_generic, make_string(name) };

    define_symbol (sym_tc,           "tc");
    define_symbol (sym_os,           "os");
    define_symbol (sym_is,           "is");
    define_symbol (sym_vendor,       "vendor");
    define_symbol (sym_gcc,          "gcc");
    define_symbol (sym_msvc,         "msvc");
    define_symbol (sym_borland,      "borland");
    define_symbol (sym_darwin,       "darwin");
    define_symbol (sym_linux,        "linux");
    define_symbol (sym_freebsd,      "freebsd");
    define_symbol (sym_openbsd,      "openbsd");
    define_symbol (sym_dragonflybsd, "dragonflybsd");
    define_symbol (sym_netbsd,       "netbsd");
    define_symbol (sym_windows,      "windows");
    define_symbol (sym_64bit,        "is-64-bit");
    define_symbol (sym_32bit,        "is-32-bit");
    define_symbol (sym_arm,          "arm");
    define_symbol (sym_x86,          "x86");
    define_symbol (sym_mips,         "mips");
    define_symbol (sym_powerpc,      "powerpc");
    define_symbol (sym_sh,           "sh");

    sexpr c;

    td.toolchain  = tc_generic;

    for (c = toolchain_patterns; consp (c); c = cdr (c))
    {
        sexpr a = car (c), aa = car (a);
        sexpr rx = rx_compile_sx (aa);

        if (truep (rx_match (rx, name)))
        {
            sexpr d;

            for (d = cdr (a); consp (d); d = cdr (d))
            {
                a = car (d);

                if (consp (a))
                {
                    aa = car (a);

                    if (truep (equalp (aa, sym_tc)))
                    {
                        a  = cdr (a);
                        aa = car (a);

                        if (truep (equalp (aa, sym_gcc)))
                        {
                            td.toolchain = tc_gcc;
                        }
                        else if (truep (equalp (aa, sym_borland)))
                        {
                            td.toolchain = tc_borland;
                        }
                        else if (truep (equalp (aa, sym_msvc)))
                        {
                            td.toolchain = tc_msvc;
                        }

                        td.toolchain_sym = aa;

                        a  = cdr (a);
                        aa = car (a);

                        td.uname_toolchain = sx_string (aa);
                    }
                    else if (truep (equalp (aa, sym_os)))
                    {
                        a  = cdr (a);
                        aa = car (a);

                        if (truep (equalp (aa, sym_linux)))
                        {
                            td.operating_system = os_linux;
                        }
                        else if (truep (equalp (aa, sym_darwin)))
                        {
                            td.operating_system = os_darwin;
                        }
                        else if (truep (equalp (aa, sym_windows)))
                        {
                            td.operating_system = os_windows;
                        }
                        else if (truep (equalp (aa, sym_netbsd)))
                        {
                            td.operating_system = os_netbsd;
                        }
                        else if (truep (equalp (aa, sym_dragonflybsd)))
                        {
                            td.operating_system = os_dragonflybsd;
                        }
                        else if (truep (equalp (aa, sym_freebsd)))
                        {
                            td.operating_system = os_freebsd;
                        }
                        else if (truep (equalp (aa, sym_openbsd)))
                        {
                            td.operating_system = os_openbsd;
                        }

                        td.operating_system_sym = aa;

                        a  = cdr (a);
                        aa = car (a);

                        td.uname_os = sx_string (aa);
                    }
                    else if (truep (equalp (aa, sym_is)))
                    {
                        a  = cdr (a);
                        aa = car (a);

                        if (truep (equalp (aa, sym_arm)))
                        {
                            td.instruction_set = is_arm;
                        }
                        else if (truep (equalp (aa, sym_x86)))
                        {
                            td.instruction_set = is_x86;
                        }
                        else if (truep (equalp (aa, sym_mips)))
                        {
                            td.instruction_set = is_mips;
                        }
                        else if (truep (equalp (aa, sym_powerpc)))
                        {
                            td.instruction_set = is_powerpc;
                        }
                        else if (truep (equalp (aa, sym_sh)))
                        {
                            td.instruction_set = is_sh;
                        }

                        td.instruction_set_sym = aa;

                        a  = cdr (a);
                        aa = car (a);

                        td.uname_arch = sx_string (aa);
                    }
                    else if (truep (equalp (aa, sym_vendor)))
                    {
                        a  = cdr (a);
                        aa = car (a);

                        td.uname_vendor = sx_string (aa);
                    }
                }
                else if (truep (equalp (a, sym_32bit)))
                {
                    td.instruction_set_options |= IS_32_BIT;
                }
                else if (truep (equalp (a, sym_64bit)))
                {
                    td.instruction_set_options |= IS_64_BIT;
                }
            }
        }
    }

    td.uname =
        sx_join (make_string (td.uname_arch), str_dash,
        sx_join (make_string (td.uname_vendor), str_dash,
        sx_join (make_string (td.uname_os), str_dash,
                 make_string (td.uname_toolchain))));

    initialise_toolchain_tex (&td);
    initialise_toolchain_doxygen (&td);

    switch (td.toolchain)
    {
        case tc_generic: icemake_prepare_toolchain_generic (&td); break;
        case tc_gcc:     icemake_prepare_toolchain_gcc     (&td); break;
        case tc_borland: icemake_prepare_toolchain_borland (&td); break;
        case tc_msvc:    icemake_prepare_toolchain_msvc    (&td); break;
        case tc_latex:   icemake_prepare_toolchain_latex   (&td); break;
        case tc_doxygen: icemake_prepare_toolchain_doxygen (&td); break;
        case tc_unknown: break;
    }

    switch (td.operating_system)
    {
        default:
        case os_generic:
            icemake_prepare_operating_system_generic (0, &td);
            break;
    }

    return with_data (&td, aux);
}

static void parse_add_definition
    (struct icemake *im, sexpr path, struct toolchain_descriptor *td,
     sexpr definition)
{
    struct target *t = (struct target *)0;
    sexpr sxcar = car (definition);

    if (truep(equalp(sxcar, sym_archive)))
    {
        t = create_archive (im, path, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_library)))
    {
        t = create_library (im, path, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_programme)))
    {
        t = create_programme (im, path, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_test_case)))
    {
        t = create_test_case (im, path, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_documentation)))
    {
        t = create_documentation (im, path, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_include)))
    {
        sexpr c = cdr (definition), icemake_sx_path, r;
        struct sexpr_io *io;

        while (consp (c))
        {
            icemake_sx_path = sx_join (path, car (c), sx_nil);

            if (truep (filep (icemake_sx_path)))
            {
                io = sx_open_i (io_open_read (sx_string (icemake_sx_path)));

                while (!eofp(r = sx_read (io)))
                {
                    if (nexp(r)) continue;
                    if (consp(r))
                    {
                        parse_add_definition (im, path, td, r);
                    }
                }

                sx_close_io (io);
            }

            c = cdr (c);
        }
    }

    if (t != (struct target *)0)
    {
        tree_add_node_string_value
            (&(im->targets), (char *)sx_string(t->name), t);
    }
}

void icemake_load_data (sexpr data)
{
    define_symbol (sym_patterns, "patterns");

    sexpr t = car (data);

    data = cdr (data);

    if (truep (equalp (t, sym_patterns)))
    {
        toolchain_patterns = sx_set_merge (data, toolchain_patterns);
    }
}

void icemake_load_internal_data ()
{
#if !defined(NOVERSION)
    struct io *io = io_open_buffer ((void *)icemake_data, icemake_data_length);
    struct sexpr_io *i = sx_open_i (io);
    sexpr r;

    while (!eofp (r = sx_read (i)))
    {
        icemake_load_data (r);
    }

    sx_close_io (i);
#endif
}

int icemake_prepare
    (struct icemake *im, const char *path, struct toolchain_descriptor *td,
     unsigned long options, sexpr alternatives,
     int (*with_data)(struct icemake *, void *), void *aux)
{
    struct icemake iml =
        { fs_afsl,
          sx_end_of_list,
          TREE_INITIALISER,
          (int (*)(struct icemake *, sexpr))0,
          0, 0, options, sx_end_of_list, sx_end_of_list, sx_end_of_list };
    sexpr base_path = make_string (path);
    sexpr icemake_sx_path;
    struct sexpr_io *io;
    sexpr r;

    define_string (str_icemakedsx, "icemake.sx");
 
    if (im == (struct icemake *)0)
    {
        im = &iml;
        icemake_prepare_visualiser_stub (im);
    }

    im->roots = sx_set_add (im->roots, base_path);

    im->alternatives = sx_set_merge (im->alternatives, alternatives);
 
    icemake_prepare_operating_system_generic (im, td);
 
    if (td != (struct toolchain_descriptor *)0)
    {
        if (td->operating_system == os_darwin)
        {
            im->options &= ~ICEMAKE_OPTION_DYNAMIC_LINKING;
        }

        if ((td->operating_system != os_windows) &&
            !(td->options & ICEMAKE_OPTION_FREESTANDING))
        {
            im->options &= ~ICEMAKE_OPTION_DYNAMIC_LINKING;
        }
    }

    icemake_sx_path = sx_join (base_path, str_icemakedsx, sx_nil);

    if (falsep (filep (icemake_sx_path)))
    {
        iml.visualiser.on_error
            (im, ie_missing_description_file,
             sx_end_of_list);
    }

    io = sx_open_i (io_open_read (sx_string (icemake_sx_path)));

    while (!eofp(r = sx_read (io)))
    {
        if (nexp(r)) continue;
        if (consp(r))
        {
            parse_add_definition (im, base_path, td, r);
        }
    }

    sx_close_io (io);

    return with_data (im, aux);
}

static void collect_targets (struct tree_node *n, void *aux)
{
    sexpr *targets = (sexpr *)aux;
    const char *key = (const char *)n->key;

    *targets = sx_set_add (*targets, make_string (key));
}

static void collect_test_targets (struct tree_node *n, void *aux)
{
    sexpr *targets = (sexpr *)aux;
    const char *key = (const char *)n->key;
    struct target *t = (struct target *)node_get_value (n);

    if (t->options & ICEMAKE_TEST_CASE)
    {
        *targets = sx_set_add (*targets, make_string (key));
    }
}

static void remove_test_targets (struct tree_node *n, void *aux)
{
    sexpr *targets = (sexpr *)aux;
    const char *key = (const char *)n->key;
    struct target *t = (struct target *)node_get_value (n);

    if (t->options & ICEMAKE_TEST_CASE)
    {
        *targets = sx_set_remove (*targets, make_string (key));
    }
}

static sexpr dependency_sort (sexpr a, sexpr b, void *aux)
{
    struct icemake *im = (struct icemake *)aux;
    struct tree_node *an = tree_get_node_string
        (&(im->targets), (char *)sx_string (a));
    struct tree_node *bn = tree_get_node_string
        (&(im->targets), (char *)sx_string (b));
    
    if ((an != (struct tree_node *)0) &&
        (bn != (struct tree_node *)0))
    {
        struct target *at = (struct target *)node_get_value (an);
        struct target *bt = (struct target *)node_get_value (bn);

        if (truep (sx_set_memberp (bt->libraries, at->name)) ||
            truep (sx_set_memberp (bt->olibraries, at->name)))
        {
            return sx_true;
        }
    }

    return sx_false;
}

void initialise_icemake ( void )
{
    static int initialised = 0;

    if (initialised == (char)0)
    {
        multiplex_io();
/*        multiplex_all_processes();*/
        multiplex_signal_primary();
        multiplex_process();
        multiplex_sexpr();

        initialised = (char)1;
    }
}

int icemake
    (struct icemake *im)
{
    int failures = 0;

    read_metadata            (im);
    icemake_prepare_archives (im);
    merge_contexts           (im);

    multiplex_add_signal (sig_segv, cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_int,  cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_term, cb_on_bad_signal, (void *)0);
    multiplex_add_signal (sig_bus,  cb_on_bad_signal, (void *)0);

    if (!consp(im->buildtargets))
    {
        tree_map (&(im->targets), collect_targets, &(im->buildtargets));
    }

    tree_map (&(im->targets),
              ((im->options & ICEMAKE_OPTION_TESTS) ? collect_test_targets
                                                    : remove_test_targets),
              &(im->buildtargets));

    im->buildtargets = sx_set_sort_merge
        (im->buildtargets, dependency_sort, (void *)im);

    im->workstack =
        cons (cons (sym_completed, cons (sym_targets, im->buildtargets)),
              im->workstack);

    if (im->options & ICEMAKE_OPTION_INSTALL)
    {
        failures += icemake_install (im);
    }

    if (im->options & ICEMAKE_OPTION_TESTS)
    {
        failures += icemake_run_tests (im);
    }

    if (truep (do_build_documentation))
    {
        failures += icemake_build_documentation (im);
    }

    failures += icemake_link  (im);
    failures += icemake_build (im);

    im->workstack =cons (cons (sym_targets, im->buildtargets), im->workstack);

    im->buildtargets = sx_reverse (im->buildtargets);

    failures += icemake_loop_processes (im);

    save_metadata (im);

    return failures;
}
