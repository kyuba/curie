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
#include <sievert/filesystem.h>
#include <sievert/metadata.h>

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

static sexpr toolchain_patterns       = sx_end_of_list;
static sexpr toolchain_specifications = sx_end_of_list;
static sexpr toolchain_object_types   = sx_end_of_list;
static sexpr toolchain_install        = sx_end_of_list;
static sexpr toolchain_metadata       = sx_end_of_list;
static sexpr toolchain_build          = sx_end_of_list;
static sexpr toolchain_bind           = sx_end_of_list;
static sexpr toolchain_fs_layout      = sx_end_of_list;

define_symbol (sym_default, "default");
define_symbol (sym_windows, "windows");

static sexpr glue_spec
    (sexpr spec, sexpr e)
{
    sexpr r = sx_end_of_list, c, a, d, s, f, q;

    if (consp (e))
    {
        for (s = e; consp (s); s = cdr (s))
        {
            for (q = glue_spec (spec, car (s)); consp (q); q = cdr (q))
            {
                r = cons (car (q), r);
            }
        }
    }
    else for (c = spec; consp (c); c = cdr (c))
    {
        a = car (c);

        if (consp (a))
        {
            sexpr s = str_blank;

            for (a; consp (a); a = cdr (a))
            {
                d = car (a);

                if (symbolp (d))
                {
                    d = e;
                }

                s = sx_join (s, d, sx_nil);
            }

            r = cons (s, r);
        }
        else if (symbolp (a))
        {
            r = cons (e, r);
        }
        else
        {
            r = cons (a, r);
        }
    }

    return sx_reverse (r);
}

static sexpr process_fold_includes
    (struct target *t, sexpr variables, sexpr spec)
{
    sexpr r = sx_end_of_list, p = t->icemake->roots,
          include_paths = sx_end_of_list, cur, q;

    while (consp (p))
    {
        include_paths =
            sx_set_merge
                (include_paths, icemake_permutate_paths
                     (t->toolchain, sx_join (car(p), str_include, sx_nil)));
        p = cdr (p);
    }

    if (stringp (i_destdir) && falsep (equalp (i_destdir, str_blank)))
    {
        for (q = get_path (t, sx_nonexistent, sym_C_Header,
                           str_blank, sx_nonexistent);
             consp (q); q = cdr (q))
        {
            include_paths = cons (car (q), include_paths);
        }
    }

    for (q = get_path (t, sym_build, sym_C_Header, str_blank, sx_nonexistent);
         consp (q); q = cdr (q))
    {
        include_paths = cons (car (q), include_paths);
    }

    cur = path_normalise(include_paths);

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        for (q = glue_spec (spec, sxcar); consp (q); q = cdr (q))
        {
            r = cons (car (q), r);
        }

        cur = cdr (cur);
    }

    return sx_reverse (r);
}

static sexpr process_fold_library_paths
    (struct target *t, sexpr variables, sexpr spec)
{
    sexpr r = sx_end_of_list, p = t->icemake->roots,
          lib_paths = sx_end_of_list, cur, q;

    while (consp (p))
    {
        lib_paths =
            sx_set_merge
                (lib_paths, icemake_permutate_paths
                     (t->toolchain, sx_join (car(p), str_include, sx_nil)));
        p = cdr (p);
    }

    if (stringp (i_destdir) && falsep (equalp (i_destdir, str_blank)))
    {
        lib_paths = sx_reverse (lib_paths);

        for (q = get_path (t, sx_nonexistent, sym_Shared_Object,
                           str_blank, sx_nonexistent);
             consp (q); q = cdr (q))
        {
            lib_paths = cons (car (q), lib_paths);
        }

        for (q = get_path (t, sx_nonexistent, sym_Static_Library,
                           str_blank, sx_nonexistent);
             consp (q); q = cdr (q))
        {
            lib_paths = cons (car (q), lib_paths);
        }

        lib_paths = sx_reverse (lib_paths);
    }

    cur = path_normalise(lib_paths);

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        for (q = glue_spec (spec, sxcar); consp (q); q = cdr (q))
        {
            r = cons (car (q), r);
        }

        cur = cdr (cur);
    }

    return sx_reverse (r);
}

static sexpr process_fold_libraries
    (struct target *t, sexpr variables, sexpr spec)
{
    sexpr r = sx_end_of_list, p = t->libraries;

    for (p = t->libraries; consp (p); p = cdr (p))
    {
        sexpr sxcar = car (p), q;

        for (q = glue_spec (spec, sxcar); consp (q); q = cdr (q))
        {
            r = cons (car (q), r);
        }
    }

    return sx_reverse (r);
}

static sexpr process_fold_original_libraries
    (struct target *t, sexpr variables, sexpr spec)
{
    sexpr r = sx_end_of_list, p = t->olibraries;

    for (p = t->olibraries; consp (p); p = cdr (p))
    {
        sexpr sxcar = car (p), q;

        for (q = glue_spec (spec, sxcar); consp (q); q = cdr (q))
        {
            r = cons (car (q), r);
        }
    }

    return sx_reverse (r);
}

static sexpr process_fold
    (struct target *t, sexpr variables, sexpr spec)
{
    define_symbol (sym_includes,           "includes");
    define_symbol (sym_libraries,          "libraries");
    define_symbol (sym_original_libraries, "original-libraries");
    define_symbol (sym_library_paths,      "library-paths");

    sexpr r = sx_end_of_list, a = car (spec);

    if (truep (equalp (a, sym_includes)))
    {
        r = process_fold_includes
            (t, variables, cdr (spec));
    }
    else if (truep (equalp (a, sym_libraries)))
    {
        r = process_fold_libraries
            (t, variables, cdr (spec));
    }
    else if (truep (equalp (a, sym_original_libraries)))
    {
        r = process_fold_original_libraries
            (t, variables, cdr (spec));
    }
    else if (truep (equalp (a, sym_library_paths)))
    {
        r = process_fold_library_paths
            (t, variables, cdr (spec));
    }

    return r;
}

static sexpr process_if
    (struct target *t, sexpr variables, sexpr spec)
{
    define_symbol (sym_freestanding,  "freestanding");

    sexpr r = sx_end_of_list, a = car (spec);

    if (truep (equalp (a, sym_freestanding)))
    {
        if (t->toolchain->options & ICEMAKE_OPTION_FREESTANDING)
        {
            r = car (cdr (spec));
        }
        else
        {
            r = car (cdr (cdr (spec)));
        }
    }

    if (nexp (r))
    {
        r = sx_end_of_list;
    }

    return r;
}

static sexpr substitute_variables
    (struct target *t, sexpr variables, sexpr input)
{
    sexpr r = sx_end_of_list, c, s, a, u;

    define_symbol (sym_fold, "fold");
    define_symbol (sym_if,   "if");
    define_symbol (sym_q,    "?");

//    sx_write (sx_open_stdio(), variables);

    for (c = input; consp (c); c = cdr (c))
    {
        a = car (c);

        if (consp (a))
        {
            u = car (a);

            if (truep (equalp (u, sym_fold)))
            {
                s = process_fold (t, variables, cdr (a));

                if (consp (s))
                {
                    for (s = sx_reverse (s); consp (s); s = cdr (s))
                    {
                        r = cons (car (s), r);
                    }
                }
                else
                {
                    r = cons (s, r);
                }
            }
            else if (truep (equalp (u, sym_if)) || truep (equalp (u, sym_q)))
            {
                s = process_if (t, variables, cdr (a));

                if (consp (s))
                {
                    for (s = sx_reverse (s); consp (s); s = cdr (s))
                    {
                        r = cons (car (s), r);
                    }
                }
                else
                {
                    r = cons (s, r);
                }
            }
            else
            {
                r = cons (substitute_variables (t, variables, a), r);
            }
        }
        else
        {
            s = sx_alist_get (variables, a);

            if (nexp (s))
            {
                r = cons (a, r);
            }
            else if (consp (s))
            {
                for (s = sx_reverse (s); consp (s); s = cdr (s))
                {
                    r = cons (car (s), r);
                }
            }
            else if (stringp (s))
            {
                r = cons (s, r);
            }
        }
    }

    return sx_reverse (r);
}

static sexpr get_path_r
    (struct target *context, sexpr rule, sexpr name, sexpr target)
{
    define_symbol (sym_base,         "base");
    define_symbol (sym_root,         "root");
    define_symbol (sym_name,         "name");
    define_symbol (sym_target,       "target");
    define_symbol (sym_uname,        "uname");
    define_symbol (sym_version,      "version");
    define_symbol (sym_build_number, "build-number");
    define_symbol (sym_slash,        "/");

    sexpr r = str_blank, c, a;

    if (consp (name))
    {
        r = sx_end_of_list;

        for (c = name; consp (c); c = cdr (c))
        {
            r = sx_set_merge (r, get_path_r (context, rule, car (c), target));
        }

        return r;
    }

    for (c = rule; consp (c); c = cdr (c))
    {
        a = car (c);

        if (truep (equalp (a, sym_name)))
        {
            r = sx_join (r, name, sx_nil);
        }
        else if (truep (equalp (a, sym_base)))
        {
            r = sx_join (r, context->base, sx_nil);
        }
        else if (truep (equalp (a, sym_root)))
        {
            r = sx_join (r, i_destdir, sx_nil);
        }
        else if (truep (equalp (a, sym_target)))
        {
            r = sx_join (r, target, sx_nil);
        }
        else if (truep (equalp (a, sym_uname)))
        {
            r = sx_join (r, context->toolchain->uname, sx_nil);
        }
        else if (truep (equalp (a, sym_version)))
        {
            r = sx_join (r, context->dversion, sx_nil);
        }
        else if (truep (equalp (a, sym_build_number)))
        {
            r = sx_join (r, context->buildnumber, sx_nil);
        }
        else if (truep (equalp (a, sym_slash)))
        {
            if (truep (equalp (context->toolchain->operating_system_sym,
                               sym_windows)))
            {
                r = sx_join (r, str_backslash, sx_nil);
            }
            else
            {
                r = sx_join (r, str_slash, sx_nil);
            }
        }
        else if (consp (a))
        {
            r = sx_join (r, get_path_r (context, a, name, target), sx_nil);
        }
        else
        {
            r = sx_join (r, a, sx_nil);
        }
    }

    return sx_list1 (r);
}

sexpr get_path
    (struct target *context, sexpr layout, sexpr type, sexpr name, sexpr target)
{
    sexpr r = sx_end_of_list, t, c;

    if (!symbolp (layout))
    {
        layout = sx_alist_get (toolchain_fs_layout, sym_default);
    }

    if (nexp (target))
    {
        target = context->name;
    }

    t = sx_alist_get (toolchain_fs_layout, layout);
    t = sx_alist_get (t, type);

    for (c = t; consp (c); c = cdr (c))
    {
        r = sx_set_merge (r, get_path_r (context, car (c), name, target));
    }

    return r;
}

sexpr get_path_d
    (struct target *context, sexpr layout1, sexpr layout2, sexpr type,
     sexpr name, sexpr target)
{
    return
        get_path (context, layout2, type,
                  get_path (context, layout1, type, name, target),
                  target);
}

static sexpr make_file_name
    (struct target *context, sexpr rule, sexpr name)
{
    return get_path_r (context, rule, name, sx_nonexistent);
}

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

static void find_code (struct target *context, sexpr file)
{
    sexpr r;
    sexpr primus   = sx_false;
    sexpr secundus = sx_false;

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

static sexpr convert_code (sexpr code)
{
    sexpr r = sx_end_of_list, c, a, na, d, t, y, s, l, sa, sb;

//    sx_write (sx_open_stdio(), code);

    for (c = code; consp (c); c = cdr (c))
    {
        a  = car (c);
        na = car (a);

        for (d = cdr (a); consp (d); d = cdr (d))
        {
            t  = car (d);
            y  = car (t);
            s  = cdr (t);
            sa = car (s);
            sb = car (cdr (s));

            l = sx_alist_get    (r, y);
            r = sx_alist_remove (r, y);

            if (nexp (l))
            {
                l = sx_end_of_list;
            }

            l = sx_alist_add    (l, na, sx_list1(cons (sb, sa)));

            r = sx_alist_add    (r, y, l);
        }
    }

//    sx_write (sx_open_stdio(), r);

    return r;
}

static sexpr make_work_tree (struct target *context, sexpr code)
{
    sexpr r, t, a, ty, b, fs, n, uty, c, d, e, na, f, g, j, ufn, jn, uc, ne, q,
          spec, on, y, o, src, w, ps, l, ml, mt, sn, tty;
    enum { m_use_first, m_merge } mode;
    enum { c_consume, c_keep }    consume;
    enum { m_have, m_no_match }   match;
    enum { t_original, t_common } target;
    enum { u_update, u_none }     update;
    enum { l_use, l_dont_use }    libraries;

    define_symbol (sym_input_consume,   "input:consume");
    define_symbol (sym_input_keep,      "input:keep");
    define_symbol (sym_mode_merge,      "mode:merge");
    define_symbol (sym_mode_use_first,  "mode:use-first");
    define_symbol (sym_target_original, "target:original");
    define_symbol (sym_target_common,   "target:common");
    define_symbol (sym_type_programme,  "type:programme");
    define_symbol (sym_type_library,    "type:library");

    define_symbol (sym_target,          "target");
    define_symbol (sym_source,          "source");

    t  = sx_alist_get (toolchain_build, context->toolchain->toolchain_sym);
/*    fs = sx_alist_get
            (toolchain_object_types, context->toolchain->toolchain_sym); */

    r = convert_code (code);

//    sx_write (sx_open_stdio(), r);

    do {
        update = u_none;

        for (a = t; consp (a); a = cdr (a))
        {
            ty      = car (a);
            tty     = car (ty);

            if (truep (sx_set_memberp (context->omit, tty)))
            {
                /* rule is not executed, because object type is specifically
                 * not wanted in declaration of target.  */
                continue;
            }

            mode      = m_use_first;
            consume   = c_keep;
            match     = m_no_match;
            target    = t_original;
            libraries = l_dont_use;
            ps        = sx_end_of_list;

            for ((b = cdr (ty)), (ty = tty); consp (b); b = cdr (b))
            {
                n = car (b);

                if (truep (equalp (sym_input_consume, n)))
                {
                    consume = c_consume;
                }
                else if (truep (equalp (sym_input_keep, n)))
                {
                    consume = c_keep;
                }
                else if (truep (equalp (sym_mode_use_first, n)))
                {
                    mode = m_use_first;
                }
                else if (truep (equalp (sym_mode_merge, n)))
                {
                    mode = m_merge;
                }
                else if (truep (equalp (sym_target_original, n)))
                {
                    target = t_original;
                }
                else if (truep (equalp (sym_target_common, n)))
                {
                    target = t_common;
                }
                else if (truep (equalp (sym_type_programme, n)))
                {
                    if (!(context->options & ICEMAKE_PROGRAMME))
                    {
                        break;
                    }
                }
                else if (truep (equalp (sym_type_library, n)))
                {
                    if (!(context->options & ICEMAKE_LIBRARY))
                    {
                        break;
                    }
                }
                else if (consp (n))
                {
                    uty  = car (n);
                    spec = cdr (n);

                    w    = sx_alist_get (r, uty);

                    ml   = sx_end_of_list;
                    mt   = sx_end_of_list;

                    for (d = w; consp (d); d = cdr (d))
                    {
                        q   = car (d);
                        na  = car (q);

                        if (consume == c_consume)
                        {
                            w = sx_alist_remove (w, na);
                            r = sx_alist_remove (r, uty);
                            r = sx_alist_add    (r, uty, w);

                            update = u_update;
                        }

                        if (truep (sx_set_memberp (ps, na)))
                        {
                            continue;
                        }

                        src = car (cdr (q));
                        if (consp (src))
                        {
                            sn  = car (src);
                            src = cdr (src);
                        }
                        else
                        {
                            sn  = sx_nonexistent;
                        }

                        if (mode == m_use_first)
                        {
                            on  = (target == t_original) ? na : context->name;

                            ufn = get_path_d
                                    (context, context->toolchain->toolchain_sym,
                                     sym_build, ty, on, sn);

                            for (uc = ufn; consp (uc); uc = cdr (uc))
                            {
                                ufn = car (uc);

                                l = sx_alist_get    (r, ty);
                                r = sx_alist_remove (r, ty);

                                o = sx_list5
                                    (na, ufn, src, sx_list1(q),
                                     substitute_variables
                                        (context,
                                         sx_set_merge
                                            (context->toolchain->environment,
                                             sx_list2
                                                (cons (sym_target, ufn),
                                                 cons (sym_source, src))),
                                         spec));

                                if (nexp (l))
                                {
                                    l = sx_end_of_list;
                                }

                                l = sx_set_add   (l, o);
                                r = sx_alist_add (r, ty, l);

                                update = u_update;
                            }

                            ps = sx_set_add (ps, na);
                        }
                        else
                        {
                            ml = sx_set_add (ml, src);
                            mt = sx_set_add (mt, q);
                        }
                    }

                    if ((mode == m_merge) && !eolp (ml) && !eolp (mt))
                    {
                        on = context->name;

                        ufn = get_path_d
                                (context, context->toolchain->toolchain_sym,
                                 sym_build, ty, on, sx_nonexistent);

                        for (uc = ufn; consp (uc); uc = cdr (uc))
                        {
                            ufn = car (uc);

                            l = sx_alist_get    (r, ty);
                            r = sx_alist_remove (r, ty);

                            o = sx_list5
                                (on, ufn, ml, mt,
                                 substitute_variables
                                    (context,
                                     sx_set_merge
                                        (context->toolchain->environment,
                                         sx_list2
                                            (cons (sym_target, ufn),
                                             cons (sym_source, ml))),
                                     spec));

                            if (nexp (l))
                            {
                                l = sx_end_of_list;
                            }

                            l = sx_set_add   (l, o);
                            r = sx_alist_add (r, ty, l);

                            update = u_update;
                        }
                    }
                }
            }
        }
    } while (update == u_update);
//    } while (0);

//    sx_write (sx_open_stdio(), code);
//    sx_write (sx_open_stdio(), r);

    return r;
}

static sexpr find_alternative (struct target *context, sexpr file)
{
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

    return file;
}

static sexpr find_code_with_spec (struct target *context, sexpr code)
{
    sexpr r = sx_end_of_list, c, a, b, t, n, d, e, tfn, ty, q, u;

    t = sx_alist_get
            (toolchain_object_types, context->toolchain->toolchain_sym);

    for (a = code; consp (a); a = cdr (a))
    {
        n = car (a);
        n = find_alternative (context, n);
        q = sx_list1 (n);

        for (c = t; consp (c); c = cdr (c))
        {
            ty = car (c);

            for (u = get_path (context, context->toolchain->toolchain_sym,
                               ty, n, sx_nonexistent);
                 consp (u); u = cdr (u))
            {
                tfn = find_code_with_suffix
                    (context->toolchain, context->base, car (u), "");
//                sx_write (sx_open_stdio(), car(u));

                if (!falsep (tfn))
                {
                    q = cons (sx_list3 (ty, tfn, context->name), q);
                }
            }
        }

        r = cons (sx_reverse(q), r);
    }

//    make_work_tree (context, r);

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
    context->omit             = sx_end_of_list;
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

static void make_metadata_files (struct target *context)
{
    sexpr s = sx_alist_get
        (toolchain_metadata, context->toolchain->toolchain_sym), c, cname;

    cname = context->name;

    for (c = s; consp (c); c = cdr (c))
    {
        sexpr spec = car (c), type, layout, name, sdata, d, ufn;
        const char *data;

        type = car (spec);
        spec = cdr (spec);
        layout = car (spec);
        spec = cdr (spec);
        name = car (spec);
        spec = cdr (spec);

        /* we're abusing the path generation functions here... */
        sdata = get_path_r (context, spec, cname, sx_nonexistent);
        sdata = car (sdata);
        data  = sx_string (sdata);

        ufn = get_path_d (context, context->toolchain->toolchain_sym, layout,
                          type, name, sx_nonexistent);
        for (d = ufn; consp (d); d = cdr (d))
        {
            sexpr fn = car (ufn);
            struct io *o;
            int l;

            mkdir_pi (fn);

            o = io_open_write (sx_string (fn));
            for (l = 0; data[l]; l++);

            io_collect (o, data, l);

            io_close (o);
        }
    }
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

/*            while (consp (sxc))
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
            } */

            context->code = find_code_with_spec (context, cdr(sxcar));
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
        else if (truep(equalp(sxcaar, sym_omit)))
        {
            context->omit = sx_set_merge (context->omit, cdr (sxcar));
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
        if (!(context->options & ICEMAKE_HOSTED) &&
            (context->options & ICEMAKE_PROGRAMME))
        {
            context->libraries = cons (str_curie_bootstrap, context->libraries);
        }

        context->libraries  = cons (str_curie, context->libraries);
        context->olibraries = cons (str_curie, context->olibraries);
    }

    context->olibraries = sx_set_remove (context->olibraries, context->name);
    context->libraries  = sx_set_remove (context->libraries,  context->name);

    make_metadata_files (context);
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
//    tree_map (&(im->targets), target_map_combine, (void *)im);
}
 
static void target_map_make_tree (struct tree_node *node, void *u)
{
    struct target *context = (struct target *)node_get_value(node);
    struct icemake *im     = (struct icemake *)u;
    sexpr c, a;

    for (c = make_work_tree (context, context->code); consp (c); c = cdr (c))
    {
        a = car (c);

        im->worktree = sx_set_merge (im->worktree, cdr (a));
    }
}

static void make_tree ( struct icemake *im )
{
    tree_map (&(im->targets), target_map_make_tree, (void *)im);
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
    sexpr w = sx_false, p = str_blank;

    if (programme != (const char *)0)
    {
        p = make_string (programme);
    }

    if ((td != (struct toolchain_descriptor *)0) &&
        (programme != (const char *)0))
    {
        if ((w = which (sx_join (td->original_toolchain, str_dash, p))),
            stringp(w))
        {
            return w;
        }
    }

    if ((programme != (const char *)0) && (programme[0] == '/'))
    {
        if (truep (filep (p)))
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
    
    if (programme != (const char *)0)
    {
        w = which (p);
    }

    return w;
}

static void spawn_stack_items (struct icemake *im, int *fl);
static sexpr spawn_tree_items (struct icemake *im, int *fl);

static sexpr running_tasks            = sx_end_of_list;
static sexpr running_tasks_o          = sx_end_of_list;
static sexpr running_tasks_a          = sx_end_of_list;

static sexpr get_task_id
    (sexpr task)
{
    sexpr g = sx_alist_get (running_tasks_o, task);

    if (nexp (g))
    {
        g = sx_alist_get (running_tasks_a, task);
    }

    return g;
}

static void finish_task
    (sexpr task)
{
    sexpr g = get_task_id (task);

    if (nexp (g))
    {
        return;
    }

//    n = sx_alist_get (running_tasks, g);

//    sx_write (sx_open_stdio(), sx_list2 (task, g));

    running_tasks   = sx_alist_remove (running_tasks, g);
/*    running_tasks_a = sx_alist_remove (running_tasks_a, car (n));
    running_tasks_o = sx_alist_remove (running_tasks_o, cdr (n));*/
}

static void running_task
    (sexpr task, sexpr original)
{
    static int id = 0;
    sexpr sid = get_task_id (task);

    if (nexp (sid))
    {
        id++;

        sid = make_integer(id);

        running_tasks_a = sx_alist_add (running_tasks_a, task, sid);
        running_tasks_o = sx_alist_add (running_tasks_o, original, sid);
    }

    running_tasks = sx_alist_add (running_tasks, sid, cons (task, original));
}

static sexpr runningp
    (sexpr task)
{
    sexpr g = get_task_id (task), n;

    if (nexp (g))
    {
        return sx_false;
    }

    g = sx_alist_get (running_tasks, g);

    return nexp (g) ? sx_false : sx_true;
}

static sexpr hasrunp
    (sexpr task)
{
    sexpr g = get_task_id (task), n;

    if (nexp (g))
    {
        return sx_false;
    }

    g = sx_alist_get (running_tasks, g);

    /* not in running tasks: task was run previously but is no longer running */
    return nexp (g) ? sx_true : sx_false;
}

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

        finish_task (sx);
    }

    free_pool_mem (p);
}

static void spawn_item
    (sexpr sx, void (*f)(struct exec_context *, void *), struct icemake *im,
     int *fl)
{
    sexpr cur = sx, cf = car (sx), otsk = sx;
    struct exec_context *context;
    const char *wdir = (const char *)0;
    char odir[BUFFERSIZE];
    int c = 0, exsize;
    char **ex;
    static struct memory_pool pool =
        MEMORY_POOL_INITIALISER(sizeof (struct process_data));
    struct process_data *pd;

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

    if (truep (runningp (sx)))
    {
        /* task already running, nothing to do */
        return;
    }

    running_task (sx, otsk);

//    sx_write (sx_open_stdio(), sx);

//    im->visualiser.on_command (im, sx);

    im->visualiser.on_command (im, cons (sym_execute, sx));

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

    pd->command  = sx;
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

struct newerp_data
{
    const char *b;
    int newest;
    
    int res;
};

static void with_metadata_n_b (struct metadata *md, void *aux)
{
    struct newerp_data *d = (struct newerp_data *)aux;
    enum metadata_classification_unix classification;
    int uid, gid, mode, device, attributes;
    long atime = 0, mtime = 0, ctime = 0, size;

    metadata_to_unix
        (md, &classification, &uid, &gid, &mode, &atime, &mtime, &ctime, &size,
         &device, &attributes);

    if ((atime >= d->newest) || (mtime >= d->newest) || (ctime >= d->newest))
    {
        d->res = 0;
    }
}

static void with_metadata_n_a (struct metadata *md, void *aux)
{
    struct newerp_data *d = (struct newerp_data *)aux;
    enum metadata_classification_unix classification;
    int uid, gid, mode, device, attributes;
    long atime = 0, mtime = 0, ctime = 0, size;

    metadata_to_unix
        (md, &classification, &uid, &gid, &mode, &atime, &mtime, &ctime, &size,
         &device, &attributes);

    if (atime > d->newest) { d->newest = atime; }
    if (mtime > d->newest) { d->newest = mtime; }
    if (ctime > d->newest) { d->newest = ctime; }

    if (d->newest == 0)
    {
        return;
    }

    metadata_from_path (d->b, with_metadata_n_b, d);
}

static int newerp (sexpr a, sexpr b)
{
    const char *aa = sx_string(a), *ba = sx_string(b);
    struct newerp_data d = { ba, 0, 1 };

    metadata_from_path (aa, with_metadata_n_a, &d);

    return d.res;
}

static void sti_test_source
    (sexpr source, sexpr target, char *dorun, char *sourcenewer)
{
    if (consp (source))
    {
        sexpr c;

        for (c = source; consp (c); c = cdr (c))
        {
            sti_test_source (car(c), target, dorun, sourcenewer);
        }
    }
    else if (stringp (source))
    {
        if (falsep (filep (source)))
        {
            *dorun = (char)0;
        }
        else if ((*sourcenewer) == (char)0)
        {
            if (falsep (filep (target)))
            {
                *sourcenewer = (char)1;
            }
            else if (newerp (source, target))
            {
                *sourcenewer = (char)1;
            }
        }
    }
}

static sexpr spawn_tree_items_r (struct icemake *im, sexpr tree, int *fl)
{
    sexpr c = tree, name, target, source, sub, run, cs, tfile;
    char dorun = (char)1, sourcenewer = (char)0;

    name   = car (c);
    c      = cdr (c);
    target = car (c);
    c      = cdr (c);
    source = car (c);
    c      = cdr (c);
    sub    = car (c);
    c      = cdr (c);
    run    = car (c);

    tfile = (consp (target)) ? car (target) : target;

/*    sx_write (sx_open_stdio(), make_integer(1));
    sx_write (sx_open_stdio(), name);
    sx_write (sx_open_stdio(), target);
    sx_write (sx_open_stdio(), source);
    sx_write (sx_open_stdio(), sub);
    sx_write (sx_open_stdio(), run);
    sx_write (sx_open_stdio(), make_integer(2));*/

//    sx_write (sx_open_stdio(), make_integer (im->alive_processes));
//    sx_write (sx_open_stdio(), tree);


    /* at this point we remove all tasks that may have been run in the
     * past, which also have been completed at the time checking this.
     */

    for (c = run; consp (c); c = cdr (c))
    {
        sexpr spec = car (c);

        if (truep (hasrunp (spec)))
        {
            run = sx_set_remove (run, spec);
        }
    }

    if (!consp (run) && !consp (sub))
    {
        /* nothing to do here */
        return sx_false;
    }

    im->remaining_tasks++;

    if (consp (sub)) /* need to do something */
    {
        int b = im->alive_processes;
        sexpr cr = sx_end_of_list, co;

        for (c = sub; consp (c); c = cdr (c))
        {
            co = spawn_tree_items_r (im, car(c), fl);

            if (consp (co))
            {
                cr = cons (co, cr);
            }
        }

        sub = cr;

        if (b != im->alive_processes) /* spawned something */
        {
            return sx_list5 (name, target, source, sub, run);
        }
    }

    if (!consp (run) && !consp (sub))
    {
        /* nothing to do here */
        return sx_false;
    }

    if (!consp (sub)) /* run task if no subtasks need to be done */
    {
        /* see if we have any slots free to run something at all */
        if (im->alive_processes >= im->max_processes)
        {
            dorun = (char)0;
        }
        /* check source and target files */
        else
        {
            sti_test_source (source, tfile, &dorun, &sourcenewer);
        }

        /* now run task if possible */
        if (dorun == (char)1)
        {
            /* dont actually run any tasks, unless they have source files newer
             * than the target files. */
            if (sourcenewer == (char)1)
            {
                for (c = run; consp (c); c = cdr (c))
                {
                    sexpr spec = car (c);

                    im->visualiser.on_command (im, spec);
                    spawn_item (spec, process_on_death, im, fl);
                }
            }

            return sx_list5 (name, target, source, sub, run);
        }
    }

    if (!consp (run) && !consp (sub))
    {
        /* nothing to do here */
        return sx_false;
    }

    return sx_list5 (name, target, source, sub, run);
}

static sexpr spawn_tree_items (struct icemake *im, int *fl)
{
    sexpr r = sx_end_of_list, c, a;

    im->remaining_tasks = 0;

//    sx_write (sx_open_stdio(), im->worktree);

    for (c = im->worktree; consp (c); c = cdr (c))
    {
        a = car (c);

        if (consp (a))
        {
            a = spawn_tree_items_r (im, a, fl);

            if (consp (a))
            {
                r = cons (a, r);
            }
        }
    }

    im->worktree = r;

    if (im->remaining_tasks > im->max_tasks)
    {
        im->max_tasks = im->remaining_tasks;
    }

/*
    sx_write (sx_open_stdio (),
              sx_list4 (make_integer (im->alive_processes),
                        make_integer (im->max_processes),
                        make_integer (im->remaining_tasks),
                        make_integer (im->max_tasks))); */

    return r;
}

enum signal_callback_result cb_on_bad_signal(enum signal s, void *p)
{
    cexit (-1);

    return scr_keep;
}

static int icemake_count_print_items (struct icemake *im )
{
/*    int count = 0;
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
    } */

    im->visualiser.items (im, im->alive_processes, im->remaining_tasks,
                          im->max_tasks);

    return im->remaining_tasks;
}

int icemake_loop_processes (struct icemake *im)
{
    int failures = 0;

/*    im->workstack = sx_reverse (im->workstack); */

//    icemake_count_print_items (im);

//    spawn_stack_items (im, &failures);
    spawn_tree_items (im, &failures);

    while ((im->alive_processes) > 0)
    {
        multiplex();
        spawn_tree_items (im, &failures);
//        spawn_stack_items (im, &failures);
        icemake_count_print_items (im);
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
        { tc_generic, os_generic, is_generic, make_string(name),
          "generic", "generic", "generic", "generic" };

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

    td.environment = sx_end_of_list;

    for (c = sx_alist_get (toolchain_bind, td.toolchain_sym);
         consp (c); c = cdr (c))
    {
        sexpr t = car (c), a = car (t), d, e, r = sx_false, u, v;
        const char *s;

        if (truep (equalp (a, sym_split)))
             for (a = cdr (t); consp (a); a = cdr (a))
             for ((v = car (a)), (u = car (v)), (d = cdr (v));
                  consp (d); d = cdr (d))
        {
            e = car (d);

//            sx_write (sx_open_stdio(), e);

            if (stringp (e))
            {
                r = sx_alist_get (td.environment, u);
                td.environment = sx_alist_remove (td.environment, u);

                if (nexp (r))
                {
                    r = sx_end_of_list;
                }

                r = prepend_flags_from_environment (r, sx_string (e));
                td.environment = sx_alist_add (td.environment, u, r);

//                sx_write (sx_open_stdio(), td.environment);
            }
        }
        else for (d = cdr (t); consp (d); d = cdr (d))
        {
            e = car (d);

//            sx_write (sx_open_stdio(), a);
//            sx_write (sx_open_stdio(), e);

            if (stringp (e))
            {
                s = sx_string (e);
                r = icemake_which (&td, 0, s);
            }
            else if (symbolp (e))
            {
                s = sx_symbol (e);
                r = icemake_which (&td, s, 0);
            }
            else
            {
                continue;
            }

//            sx_write (sx_open_stdio(), r);

            if (!falsep (r))
            {
                td.environment = sx_alist_add (td.environment, a, r);
//                sx_write (sx_open_stdio(), td.environment);
                break;
            }
        }
    }

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
    define_symbol (sym_patterns,       "patterns");
    define_symbol (sym_object_types,   "object-types");
    define_symbol (sym_specifications, "specifications");
    define_symbol (sym_metadata,       "metadata");

    sexpr t = car (data);

    data = cdr (data);

    if (truep (equalp (t, sym_patterns)))
    {
        toolchain_patterns = sx_set_merge (data, toolchain_patterns);
    }
    else if (truep (equalp (t, sym_filesystem_layout)))
    {
        toolchain_fs_layout = sx_set_merge (data, toolchain_fs_layout);
    }
    else if (truep (equalp (t, sym_specifications)))
    {
        sexpr c, a, name = sx_nil, aa, cs, o, t;

        for (c = data; consp (c); c = cdr (c))
        {
            for (a = car (c); consp (a); a = cdr (a))
            {
                aa = car (a);

                if (symbolp (aa))
                {
                    if (nilp (name))
                    {
                        name = aa;
                    }
                }
                else if (consp (aa))
                {
                    cs = car (aa);

                    if (truep (equalp (cs, sym_object_types)))
                    {
                        o = sx_alist_get (toolchain_object_types, name);
                        toolchain_object_types =
                            sx_alist_remove (toolchain_object_types, name);

                        if (nexp (o))
                        {
                            o = sx_end_of_list;
                        }

                        o = sx_set_merge (o, cdr (aa));

                        toolchain_object_types =
                            sx_alist_add (toolchain_object_types, name, o);
                    }
                    else if (truep (equalp (cs, sym_bind)))
                    {
                        o = sx_alist_get (toolchain_bind, name);
                        toolchain_bind =
                            sx_alist_remove (toolchain_bind, name);

                        if (nexp (o))
                        {
                            o = sx_end_of_list;
                        }

                        o = sx_alist_merge (o, cdr (aa));

                        toolchain_bind =
                            sx_alist_add (toolchain_bind, name, o);
                    }
                    else if (truep (equalp (cs, sym_build)))
                    {
                        o = sx_alist_get (toolchain_build, name);
                        toolchain_build =
                            sx_alist_remove (toolchain_build, name);

                        if (nexp (o))
                        {
                            o = sx_end_of_list;
                        }

                        o = sx_alist_merge (o, cdr (aa));

                        toolchain_build =
                            sx_alist_add (toolchain_build, name, o);
                    }
                    else if (truep (equalp (cs, sym_metadata)))
                    {
                        o = sx_alist_get (toolchain_metadata, name);
                        toolchain_metadata =
                            sx_alist_remove (toolchain_metadata, name);

                        if (nexp (o))
                        {
                            o = sx_end_of_list;
                        }

                        o = sx_alist_merge (o, cdr (aa));

                        toolchain_metadata =
                            sx_alist_add (toolchain_metadata, name, o);
                    }
                    else if (truep (equalp (cs, sym_install)))
                    {
                        o = sx_alist_get (toolchain_install, name);
                        toolchain_install =
                            sx_alist_remove (toolchain_install, name);

                        if (nexp (o))
                        {
                            o = sx_end_of_list;
                        }

                        o = sx_alist_merge (o, cdr (aa));

                        toolchain_install =
                            sx_alist_add (toolchain_install, name, o);
                    }
                }
            }
        }

        toolchain_specifications =
            sx_set_merge (data, toolchain_specifications);
    }
}

void icemake_load_internal_data ()
{
#if !defined(NOVERSION)
    struct io *io =
        io_open_buffer ((void *)toolchain_data, toolchain_data_length);
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
          0, 0, 0, 0, options,
          sx_end_of_list, sx_end_of_list, sx_end_of_list, sx_end_of_list };
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

    make_tree                (im);

//    sx_write (sx_open_stdio(), im->worktree);

/*    im->workstack =
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

    im->buildtargets = sx_reverse (im->buildtargets); */

    failures += icemake_loop_processes (im);

    save_metadata (im);

    return failures;
}
