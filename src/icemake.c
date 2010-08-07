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

#if !defined(_WIN32)
#include <sys/utsname.h>
#endif

#include <icemake/icemake.h>

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

#if !defined(NOVERSION)
#include <icemake/version.h>
#endif

const char *uname_arch   = "generic";
const char *uname_vendor = "unknown";

static unsigned int max_processes      = 1;

sexpr i_destdir                        = sx_false;
sexpr i_destlibdir                     = sx_false;
static sexpr in_dynamic_libraries      = sx_nil;

sexpr do_build_documentation           = sx_false;

sexpr p_diff                           = sx_false;

sexpr p_latex                          = sx_false;
sexpr p_pdflatex                       = sx_false;
sexpr p_doxygen                        = sx_false;

static sexpr i_alternatives            = sx_end_of_list;

struct sexpr_io *stdio;

static void parse_add_definition
    (struct icemake *im, struct toolchain_descriptor *td, sexpr definition);

static void on_warning (enum icemake_error error, const char *text);
static void on_error   (enum icemake_error error, const char *text);

struct process_data
{
    sexpr command;
    struct icemake *icemake;
    int *failures;
};

struct icemake_meta
{
    sexpr buildtargets;
    enum fs_layout filesystem_layout;
    unsigned long options;
};

/* toolchain classification patterns */
struct toolchain_pattern
{
    const char *pattern;

    enum toolchain        toolchain;
    enum operating_system operating_system;
    enum instruction_set  instruction_set;

    const char *uname_os;
    const char *uname_arch;
    const char *uname_vendor;
    const char *uname_toolchain;
 
    unsigned long instruction_set_options;
    unsigned long instruction_set_level;
};

static struct toolchain_pattern toolchain_pattern[] =
{
    { ".*",
        /* defaults */
        tc_generic, os_generic,      is_generic,
        "generic",      "generic", "generic",   "generic",
        0, 0 },

    { ".*-([Dd]arwin|[Ll]inux|.*[Bb][Ss][Dd]).*",
        /* default to gcc on BSDs, Darwin and Linux */
        tc_gcc,     os_unknown,      is_unknown,
        0,              0,         0,           "gnu",
        0, 0 },

    /* toolchains */
    { ".*-(gcc|gnu)",
        tc_gcc,     os_unknown,      is_unknown,
        0,              0,         0,           "gnu",
        0, 0 },
    { ".*-borland",
        tc_borland, os_unknown,      is_unknown,
        0,              0,         0,           "borland",
        0, 0 },
    { ".*-msvc",
        tc_msvc,    os_windows,      is_unknown,
        "windows",      0,         "microsoft", "msvc",
        0, 0 },
    { ".*-latex",
        tc_latex,   os_unknown,      is_unknown,
        0,              0,         0,           "latex",
        0, 0 },
    { ".*-doxygen",
        tc_doxygen, os_unknown,      is_unknown,
        0,              0,         0,           "doxygen",
        0, 0 },

    /* operating systems */
    { ".*-[Dd]arwin.*",
        tc_unknown, os_darwin,       is_unknown,
        "darwin",       0,         0,           0,
        0, 0 },
    { ".*-[Ll]inux.*",
        tc_unknown, os_linux,        is_unknown,
        "linux",        0,         0,           0,
        0, 0 },
    { ".*-[Ww]indows.*",
        tc_unknown, os_windows,      is_unknown,
        "windows",      0,         "microsoft", 0,
        0, 0 },
    { ".*-[Ff]ree[Bb][Ss][Dd].*",
        tc_unknown, os_freebsd,      is_unknown,
        "freebsd",      0,          0,          0,
        0, 0 },
    { ".*-[Nn]et[Bb][Ss][Dd].*",
        tc_unknown, os_netbsd,       is_unknown,
        "netbsd",       0,          0,          0,
        0, 0 },
    { ".*-[Oo]pen[Bb][Ss][Dd].*",
        tc_unknown, os_openbsd,      is_unknown,
        "openbsd",      0,          0,          0,
        0, 0 },
    { ".*-[Dd]ragonfly[Bb][Ss][Dd].*",
        tc_unknown, os_dragonflybsd, is_unknown,
        "dragonflybsd", 0,          0,          0,
        0, 0 },

    /* CPU architectures */
    { "(x86[-/_]64|amd64)-.*",
        tc_unknown, os_unknown,       is_x86,
        0,              "x86-64",   0,          0,
        IS_64_BIT, 0 },
    { "(x86[-/_]32|i[3-6]86)-.*",
        tc_unknown, os_unknown,       is_x86,
        0,              "x86-32",   0,          0,
        IS_32_BIT, 0 },
    { "([Pp]ower(pc| [Mm]acintosh)|ppc(-?32)?)-.*",
        tc_unknown, os_unknown,       is_powerpc,
        0,              "ppc-32",   0,          0,
        IS_32_BIT, 0 },
    { "(p(ower)?pc-?64)-.*",
        tc_unknown, os_unknown,       is_powerpc,
        0,              "ppc-64",   0,          0,
        IS_64_BIT, 0 },
    { "(arm-?(e[bl]|32)?)-.*",
        tc_unknown, os_unknown,       is_arm,
        0,              "arm-32",   0,          0,
        IS_32_BIT, 0 },

    { 0 } /* last pattern */
};

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
    lis = f_exist_add (sx_string_dir_prefix_c (td, uname_vendor, p), lis);
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
        (td, sx_string_dir_prefix_c (td, uname_arch, p), lis);
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
    (struct toolchain_descriptor *td, sexpr file, char *s)
{
    return find_in_permutations
        (td, str_src,
         sx_join (file, make_string (s), sx_nil));
}

static sexpr find_code_sx
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_code_with_suffix (td, file, ".sx");
}

static sexpr find_code_c
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_code_with_suffix (td, file, ".c");
}

static sexpr find_code_cpp
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_code_with_suffix (td, file, ".c++");
}

static sexpr find_code_s
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_code_with_suffix (td, file, ".s");
}

static sexpr find_code_pic_s
    (struct toolchain_descriptor *td, sexpr file)
{
    sexpr r = find_code_with_suffix (td, file, ".pic.s");

    return (falsep(r) ? find_code_s (td, file) : r);
}

static sexpr find_code_S
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_code_with_suffix (td, file, ".S");
}

static sexpr find_code_pic_S
    (struct toolchain_descriptor *td, sexpr file)
{
    sexpr r = find_code_with_suffix (td, file, ".pic.S");

    return (falsep(r) ? find_code_S (td, file) : r);
}

static sexpr find_code_def
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_code_with_suffix (td, file, ".def");
}

static sexpr find_code_rc
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_code_with_suffix (td, file, ".rc");
}

static sexpr find_header_with_suffix
    (struct toolchain_descriptor *td, sexpr name, sexpr file, char *s) 
{
    return find_in_permutations
        (td, str_include,
         sx_join (name, str_slash, sx_join (file, make_string (s), sx_nil)));
}

static sexpr find_header_h
    (struct toolchain_descriptor *td, sexpr name, sexpr file)
{
    return find_header_with_suffix (td, name, file, ".h");
}

sexpr get_build_file (struct target *t, sexpr file)
{
    switch (t->toolchain->toolchain)
    {
        case tc_msvc:
            return sx_join (t->buildbase, t->toolchain->uname,
                   sx_join (str_backslash, t->name,
                     (nilp (file) ? sx_nil
                                  : sx_join (str_backslash, file, sx_nil))));
        case tc_borland:
            return mangle_path_borland_sx
                     (sx_join (t->buildbase, t->toolchain->uname,
                      sx_join (str_backslash, t->name,
                      (nilp (file) ? sx_nil
                                   : sx_join (str_backslash, file, sx_nil)))));
        default:
        case tc_gcc:
            return sx_join (t->buildbase, t->toolchain->uname,
                   sx_join (str_slash, t->name,
                   sx_join (str_slash, file, sx_nil)));
    }

    return sx_false;
}

sexpr get_install_file (struct target *t, sexpr file)
{
    switch (t->icemake->filesystem_layout)
    {
        case fs_fhs:
            switch (t->toolchain->toolchain)
            {
                case tc_msvc:
                case tc_borland:
                    return sx_join (i_destdir, str_backslash, file);
                default:
                case tc_gcc:
                    return sx_join (i_destdir, str_slash, file);
            }
        case fs_afsl:
            switch (t->toolchain->toolchain)
            {
                case tc_msvc:
                case tc_borland:
                    return sx_join (i_destdir, str_backslash,
                             sx_join (make_string (t->toolchain->uname_os),
                               str_backslash,
                               sx_join (make_string (uname_arch), str_backslash,
                                        file)));
                default:
                case tc_gcc:
                    return sx_join (i_destdir, str_slash,
                             sx_join (make_string (t->toolchain->uname_os),
                               str_slash,
                               sx_join (make_string (uname_arch), str_slash,
                                        file)));
            }
    }

    return sx_false;
}

static sexpr generate_file_name_with_suffix
    (sexpr file, struct target *t, sexpr suffix)
{
    return get_build_file (t, sx_join (file, suffix, sx_nil));
}

static sexpr generate_object_file_name (sexpr file, struct target *t)
{
    return generate_file_name_with_suffix
        (file, t, ((t->toolchain->toolchain == tc_gcc) ? str_dot_o
                                                       : str_dot_obj));
}

static sexpr generate_resource_file_name (struct target *t)
{
    return generate_file_name_with_suffix (t->name, t, str_dot_res);
}

static sexpr generate_pic_object_file_name (sexpr file, struct target *t)
{
    return generate_file_name_with_suffix (file, t, str_dot_pic_o);
}

static sexpr find_code_highlevel (struct target *context, sexpr file)
{
    sexpr r, subfile = sx_join (file, str_dhighlevel, sx_nil);

    if (((r = find_code_cpp (context->toolchain, subfile)), stringp(r)))
    {
        context->options |= ICEMAKE_HAVE_CPP;

        return cons(sym_cpp, cons (r,
                 cons (generate_object_file_name (subfile, context),
                   sx_end_of_list)));

    }
    else if (((r = find_code_c (context->toolchain, subfile)), stringp(r)))
    {
        return cons(sym_c, cons (r,
                 cons (generate_object_file_name (subfile, context),
                   sx_end_of_list)));
    }

    return sx_false;
}

static sexpr find_code_highlevel_pic (struct target *context, sexpr file)
{
    sexpr r, subfile = sx_join (file, str_dhighlevel, sx_nil);

    if (((r = find_code_cpp (context->toolchain, subfile)), stringp(r)))
    {
        context->options |= ICEMAKE_HAVE_CPP;

        return cons(sym_cpp_pic, cons (r,
                 cons (generate_pic_object_file_name (subfile, context),
                   sx_end_of_list)));
    }
    else if (((r = find_code_c (context->toolchain, subfile)), stringp(r)))
    {
        return cons(sym_c_pic, cons (r,
                 cons (generate_pic_object_file_name (subfile, context),
                   sx_end_of_list)));
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
        sexpr ch = car (file), d = i_alternatives;

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
                    context->icemake->on_error
                        (ie_invalid_choice, sx_string (file));
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

    if ((r = find_code_S (context->toolchain, file)), stringp(r))
    {
        primus =
            cons(sym_preproc_assembly, cons (r,
              cons (generate_object_file_name (file, context),
                sx_end_of_list)));

        if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            (context->toolchain->toolchain == tc_gcc) &&
            !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            secundus =
                cons(sym_preproc_assembly_pic,
                  cons (find_code_pic_S (context->toolchain, file),
                    cons (generate_pic_object_file_name
                            (file, context), sx_end_of_list)));
        }

        tertius = find_code_highlevel (context, file);

        if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            (context->toolchain->toolchain == tc_gcc) &&
            !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            quartus = find_code_highlevel_pic (context, file);
        }
    }
    else if ((r = find_code_s (context->toolchain, file)), stringp(r))
    {
        primus =
            cons(sym_assembly, cons (r,
              cons (generate_object_file_name (file, context),
                sx_end_of_list)));

        if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
            (context->toolchain->toolchain == tc_gcc) &&
            !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
        {
            secundus =
                cons(sym_assembly_pic,
                  cons (find_code_pic_s (context->toolchain, file),
                    cons (generate_pic_object_file_name
                            (file, context), sx_end_of_list)));
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
        if (((r = find_code_cpp (context->toolchain, file)), stringp(r)))
        {
            context->options |= ICEMAKE_HAVE_CPP;

            primus = cons(sym_cpp, cons (r,
                       cons (generate_object_file_name (file, context),
                         sx_end_of_list)));

            if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
                (context->toolchain->toolchain == tc_gcc) &&
                !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
            {
                secundus = cons(sym_cpp_pic, cons (r,
                             cons (generate_pic_object_file_name
                                     (file, context), sx_end_of_list)));
            }
        }
        else if (((r = find_code_c (context->toolchain, file)), stringp(r)))
        {
            primus = cons(sym_c, cons (r,
                       cons (generate_object_file_name (file, context),
                         sx_end_of_list)));

            if ((context->icemake->options & ICEMAKE_OPTION_DYNAMIC_LINKING) &&
                (context->toolchain->toolchain == tc_gcc) &&
                !(context->options & ICEMAKE_NO_SHARED_LIBRARY))
            {
                secundus = cons (sym_c_pic, cons (r,
                             cons (generate_pic_object_file_name
                                     (file, context), sx_end_of_list)));
            }
        }
        else
        {
            context->icemake->on_error
                (ie_missing_code_file, sx_string (file));
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

    if ((r = find_header_h (context->toolchain, context->name, file)),
        stringp(r))
    {
        context->headers = cons (cons(file, cons (r, sx_end_of_list)), context->headers);
    }
    else
    {
        context->icemake->on_error (ie_missing_header, sx_string (file));
    }
}

static sexpr find_documentation_with_suffix
    (struct toolchain_descriptor *td, sexpr file, char *s)
{
    return find_in_permutations
        (td, str_documentation, sx_join (file, make_string (s), sx_nil));
}

static sexpr find_documentation_tex
    (struct toolchain_descriptor *td, sexpr file)
{
    return find_documentation_with_suffix (td, file, ".tex");
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

    if ((r = find_documentation_tex (context->toolchain, file)), stringp(r))
    {
        context->documentation
            = cons(cons(sym_tex, cons (file, r)), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".1")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man1, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".2")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man2, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".3")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man3, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".4")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man4, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".5")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man5, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".6")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man6, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".7")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man7, sx_end_of_list)))), context->documentation);
    }
    else if ((r = find_documentation_with_suffix (context->toolchain, file, ".8")), stringp(r))
    {
        context->documentation
            = cons(cons(sym_man, cons (file, cons(r, cons (sym_man8, sx_end_of_list)))), context->documentation);
    }
    else
    {
        context->icemake->on_error (ie_missing_documentation, sx_string (file));
    }
}

static sexpr find_data (struct target *context, sexpr file)
{
    sexpr r;

    if ((r = find_in_permutations (context->toolchain, str_data, file)),
        !stringp(r))
    {
        context->icemake->on_error (ie_missing_data, sx_string (file));
    }

    return r;
}

static struct target *get_context
    (struct icemake *im, struct toolchain_descriptor *td)
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
    if ((sx = find_code_sx (context->toolchain, context->name)), stringp(sx))
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

        if (nilp (in_dynamic_libraries) &&
            (context->toolchain->options & ICEMAKE_OPTION_FREESTANDING))
        {
            context->icemake->options |= ICEMAKE_OPTION_DYNAMIC_LINKING;
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
                sexpr r = find_code_rc (context->toolchain, context->name);

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

    mkdir_p (get_build_file (context, sx_nil));

    mkdir_p (sx_join (context->buildbase, context->toolchain->uname,
                      str_sinclude));

    mkdir_p (sx_join (context->buildbase, context->toolchain->uname,
                      sx_join (str_sincludes, context->name, sx_nil)));

    context->headers =
        cons (cons (str_version,
                    cons (sx_join (context->buildbase,context->toolchain->uname,
                                   sx_join (str_sincludes, context->name,
                                            str_sversiondh)),
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

static struct target *create_library
    (struct icemake *im, struct toolchain_descriptor *td, sexpr definition)
{
    struct target *context = get_context (im, td);

    context->name = car(definition);

    context->options |= ICEMAKE_LIBRARY;

    process_definition (context, cdr(definition));

    if (falsep(equalp(str_curie, context->name)))
    {
        context->libraries = cons (context->name, context->libraries);
    }
    context->deffile = find_code_def (td, context->name);

    return context;
}

static struct target *create_programme
    (struct icemake *im, struct toolchain_descriptor *td, sexpr definition)
{
    struct target *context = get_context (im, td);

    context->name = car(definition);

    context->options |= ICEMAKE_PROGRAMME;

    process_definition (context, cdr(definition));

    return context;
}

static struct target *create_test_case
    (struct icemake *im, struct toolchain_descriptor *td, sexpr definition)
{
    struct target *context = get_context (im, td);

    context->name        = car(definition);
    context->dname       = str_test_case;
    context->description = str_test_case;
    context->dversion    = str_1;

    context->options |= ICEMAKE_PROGRAMME | ICEMAKE_TEST_CASE;

    process_definition (context, cdr(definition));

    return context;
}

static struct target *create_documentation
    (struct icemake *im, struct toolchain_descriptor *td, sexpr definition)
{
    struct target *context = get_context (im, td);

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

static void print_help ()
{
    sx_write (stdio, make_symbol (
#if !defined(NOVERSION)
        icemake_version_long "\n\n"
#endif
        "Usage: icemake [options] [targets]\n"
        "\n"
        "Options:\n"
        " -h           Print help (this text) and exit.\n"
        " -v           Print version and exit.\n\n"
        " -t <chost>   Specify target CHOST\n"
        " -d <destdir> Specify the directory to install to.\n"
        " -i           Install resulting binaries\n"
        " -r           Execute runtime tests\n"
        " -f           Use the FHS layout for installation\n"
        " -l <libdir>  Use <libdir> instead of 'lib' when installing\n"
        " -s           Use the default FS layout for installation\n"
        " -L           Optimise linking.\n"
        " -c           Use gcc's -combine option for C source files.\n"
        " -o           Don't link dynamic libraries.\n"
        " -O           Do link dynamic libraries.\n"
        " -S           Enforce a static link (default).\n"
        " -R           Enforce a dynamic link.\n"
        " -j <num>     Spawn <num> processes simultaneously.\n"
        " -a <1> <2>   Use implementation <2> for code part <1>.\n"
        " -x           Build documentation (if possible).\n"
        "\n"
        "The [targets] specify a list of things to build, according to the\n"
        "icemake.sx file located in the current working directory.\n"
        "\n"));
}

static void print_version ()
{
    sx_write (stdio, make_symbol (
#if defined(NOVERSION)
        "icemake (version info not available)"
#else
        icemake_version_long "\n"
#endif
            ));
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
    p_latex = icemake_which (td, "latex", "LATEX");
    if (falsep(p_latex))
    {
        on_warning (ie_missing_tool, "latex");
    }

    p_pdflatex = icemake_which (td, "pdflatex", "PDFLATEX");
    if (falsep(p_pdflatex))
    {
        on_warning (ie_missing_tool, "pdflatex");
    }
}

static void initialise_toolchain_doxygen (struct toolchain_descriptor *td)
{
    p_doxygen = icemake_which (td, "doxygen", "DOXYGEN");
    if (falsep(p_doxygen))
    {
        on_warning (ie_missing_tool, "doxygen");
    }
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

        if (context->exitstatus != 0)
        {
            sx_write (stdio, cons (sym_failed,
                      cons (make_integer (context->exitstatus),
                            cons (sx,
                                  sx_end_of_list))));

            on_warning (ie_programme_failed, "");

            d->failures++;
        }
        else
        {
            sx_write (stdio, cons (sym_completed, cons (sx, sx_end_of_list)));
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

    sx_write (stdio, cons (sym_execute, sx));

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
        chdir (wdir);
    }

    context = execute (EXEC_CALL_NO_IO | EXEC_CALL_PURGE, ex,
                       curie_environment);

    if (wdir != (const char *)0)
    {
        chdir (odir);
    }

    pd = (struct process_data *)get_pool_mem (&pool);

    pd->command  = car(sx);
    pd->icemake  = im;
    pd->failures = fl;

    switch (context->pid)
    {
        case -1: im->on_error (ie_failed_to_spawn_subprocess, "");
                 break;
        case 0:  im->on_error (ie_failed_to_execute_binary_image, "");
                 break;
        default: (im->alive_processes)++;
                 multiplex_add_process (context, f, (void *)pd);
                 afree (exsize, ex);
    }
}

static void spawn_stack_items (struct icemake *im, int *fl)
{
    while (consp (im->workstack) && ((im->alive_processes) < max_processes))
    {
        sexpr spec = car (im->workstack);
        sexpr sca  = car (spec);

        if (truep (equalp (sym_phase, sca)) ||
            truep (equalp (sym_completed, sca)) ||
            truep (equalp (sym_targets, sca)))
        {
            sx_write (stdio, spec);
        }
        else if (truep (equalp (sym_install, sca)) ||
                 truep (equalp (sym_symlink, sca)))
        {
            if (im->install_file !=
                    (int (*)(struct icemake *, sexpr))0)
            {
                im->install_file (im, cdr (spec));
            }
        }
        else
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
    sexpr cur;

    for (cur = im->workstack; consp (cur); count++, cur = cdr (cur));

    if (count > 0)
    {
        sx_write (stdio, cons (sym_items_total,
                               cons (make_integer (count), sx_end_of_list)));
    }

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
    struct sexpr_io *io = sx_open_i (io_open_read("metadata.sx"));
    sexpr r;

    while (!eofp(r = sx_read (io)) && !nexp (r))
    {
        struct target *t = (struct target *)0;

        if (consp(r)) {
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
}

static void target_map_save_metadata (struct tree_node *node, void *i)
{
    struct sexpr_io *io = (struct sexpr_io *)i;
    struct target *t = (struct target *)node_get_value (node);

    sx_write (io, cons (t->name, cons (t->buildnumber, sx_end_of_list)));
}

static void save_metadata ( struct icemake *im )
{
    struct sexpr_io *io = sx_open_o (io_open_write("metadata.sx"));

    tree_map (&(im->targets), target_map_save_metadata, (void *)io);

    sx_close_io (io);
}

static void on_error (enum icemake_error error, const char *text)
{
    if (error != ie_problematic_signal)
    {
        sx_write (stdio,
                  cons (sym_error, cons (make_string (text), sx_end_of_list)));
    }

    cexit (error);
}

static void on_warning (enum icemake_error error, const char *text)
{
    sx_write (stdio,
              cons (sym_warning, cons (make_string (text), sx_end_of_list)));
}

int icemake_default_architecture
    (int (*with_data)(const char *, void *), void *aux)
{
    const char *toolchain = "unknown";
#if !defined(_WIN32)
    struct utsname un;
#endif

    char ar_t[UNAMELENGTH] = "generic";
    char os_t[UNAMELENGTH] = "generic";
    char ve_t[UNAMELENGTH] = "unknown";
    sexpr uname_t = sx_false;

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

int icemake_prepare_toolchain
    (const char *name,
     int (*with_data)(struct toolchain_descriptor *, void *), void *aux)
{
    struct toolchain_descriptor td =
        { tc_generic, os_generic, is_generic, make_string(name) };

    int p;

    sx_write (stdio, make_string (name));

    td.toolchain  = tc_generic;

    for (p = 0; toolchain_pattern[p].pattern != (const char *)0; p++)
    {
        sexpr rx = rx_compile (toolchain_pattern[p].pattern);

        if (truep (rx_match (rx, name)))
        {
            if (toolchain_pattern[p].toolchain != tc_unknown)
            {
                td.toolchain = toolchain_pattern[p].toolchain;
            }

            if (toolchain_pattern[p].operating_system != os_unknown)
            {
                td.operating_system = toolchain_pattern[p].operating_system;
            }

            if (toolchain_pattern[p].instruction_set != is_unknown)
            {
                td.instruction_set = toolchain_pattern[p].instruction_set;
            }

            if (toolchain_pattern[p].uname_os != (const char *)0)
            {
                td.uname_os = toolchain_pattern[p].uname_os;
            }

            if (toolchain_pattern[p].uname_arch != (const char *)0)
            {
                uname_arch = toolchain_pattern[p].uname_arch;
            }

            if (toolchain_pattern[p].uname_vendor != (const char *)0)
            {
                uname_vendor = toolchain_pattern[p].uname_vendor;
            }

            if (toolchain_pattern[p].uname_toolchain != (const char *)0)
            {
                td.uname_toolchain = toolchain_pattern[p].uname_toolchain;
            }

            if (toolchain_pattern[p].instruction_set_options != 0)
            {
                td.instruction_set_options
                    |= toolchain_pattern[p].instruction_set_options;
            }

            if (toolchain_pattern[p].instruction_set_level != 0)
            {
                td.instruction_set_level
                    = toolchain_pattern[p].instruction_set_level;
            }
        }
    }
    
    td.uname =
        sx_join (make_string (uname_arch), str_dash,
        sx_join (make_string (uname_vendor), str_dash,
        sx_join (make_string (td.uname_os), str_dash,
                 make_string (td.uname_toolchain))));

    initialise_toolchain_tex (&td);
    initialise_toolchain_doxygen (&td);

    if (td.operating_system == os_darwin)
    {
        in_dynamic_libraries = sx_false;
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
    (struct icemake *im, struct toolchain_descriptor *td, sexpr definition)
{
    struct target *t = (struct target *)0;
    sexpr sxcar = car (definition);

    if (truep(equalp(sxcar, sym_library)))
    {
        t = create_library (im, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_programme)))
    {
        t = create_programme (im, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_test_case)))
    {
        t = create_test_case (im, td, cdr (definition));
    }
    else if (truep(equalp(sxcar, sym_documentation)))
    {
        t = create_documentation (im, td, cdr (definition));
    }

    if (t != (struct target *)0)
    {
        tree_add_node_string_value
            (&(im->targets), (char *)sx_string(t->name), t);
    }
}

int icemake_prepare
    (struct icemake *im, const char *path, struct toolchain_descriptor *td,
     int (*with_data)(struct icemake *, void *), void *aux)
{
    struct icemake iml =
        { fs_afsl,
          on_error, on_warning,
          sx_end_of_list,
          stdio, TREE_INITIALISER,
          (int (*)(struct icemake *, sexpr))0,
          0, 0, sx_end_of_list };
    sexpr icemake_sx_path = make_string (path);
    struct sexpr_io *io;
    sexpr r;

    define_string (str_sicemakedsx, "/icemake.sx");
 
    if (im == (struct icemake *)0)
    {
        im = &iml;
    }
 
    icemake_prepare_operating_system_generic (im, td);
 
    if (nilp(in_dynamic_libraries))
    {
        if (td->operating_system == os_windows)
        {
            im->options |= ICEMAKE_OPTION_DYNAMIC_LINKING;
        }
        else if (!(td->options & ICEMAKE_OPTION_FREESTANDING))
        {
            im->options &= ~ICEMAKE_OPTION_DYNAMIC_LINKING;
        }
    }
    else
    {
        im->options |= (truep (in_dynamic_libraries) ?
                         ICEMAKE_OPTION_DYNAMIC_LINKING : 0);
    }

    icemake_sx_path = sx_join (icemake_sx_path, str_sicemakedsx, sx_nil);

    if (falsep (filep (icemake_sx_path)))
    {
        iml.on_error (ie_missing_description_file, "");
    }

    io = sx_open_i (io_open_read (sx_string (icemake_sx_path)));

    while (!eofp(r = sx_read (io)))
    {
        if (nexp(r)) continue;
        if (consp(r))
        {
            parse_add_definition (im, td, r);
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

int icemake
    (struct icemake *im)
{
    int failures = 0;

    read_metadata  (im);
    merge_contexts (im);

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

static int with_icemake (struct icemake *im, void *aux)
{
    struct icemake_meta *imc = (struct icemake_meta *)aux;

    im->filesystem_layout  = imc->filesystem_layout;
    im->buildtargets       = imc->buildtargets;
    im->options           |= imc->options;

    return icemake (im);
}

static int with_toolchain (struct toolchain_descriptor *td, void *aux)
{
    struct icemake_meta *im = (struct icemake_meta *)aux;

    return icemake_prepare
        ((struct icemake *)0, ".", td, with_icemake, (void *)im);
}
    
static int with_architecture (const char *arch, void *aux)
{
    return icemake_prepare_toolchain
        (arch, with_toolchain, aux);
}

int cmain ()
{
    int i = 1;
    const char *target_architecture = c_getenv("CHOST");
    struct icemake_meta im =
        { sx_end_of_list, fs_afsl,
          ICEMAKE_OPTION_STATIC | ICEMAKE_OPTION_DYNAMIC_LINKING };

    stdio = sx_open_stdout ();

    mkdir ("build", 0755);

    i_destlibdir = str_lib;

    while (curie_argv[i])
    {
        if (curie_argv[i][0] == '-')
        {
            int y = 1;
            int xn = i + 1;
            while (curie_argv[i][y] != 0)
            {
                switch (curie_argv[i][y])
                {
                    case 't':
                        if (curie_argv[xn])
                        {
                            target_architecture = curie_argv[xn];
                            xn++;
                        }
                        break;
                    case 'd':
                        if (curie_argv[xn])
                        {
                            i_destdir = make_string(curie_argv[xn]);
                            xn++;
                        }
                        break;
                    case 'i':
                        im.options |=  ICEMAKE_OPTION_INSTALL;
                        break;
                    case 'L':
                        im.options |=  ICEMAKE_OPTION_OPTIMISE_LINKING;
                        break;
                    case 'c':
                        im.options |=  ICEMAKE_OPTION_COMBINE;
                        break;
                    case 'S':
                        im.options |=  ICEMAKE_OPTION_STATIC;
                        break;
                    case 'R':
                        im.options &= ~ICEMAKE_OPTION_STATIC;
                        break;
                    case 'x':
                        do_build_documentation = sx_true;
                        break;
                    case 'f':
                        im.filesystem_layout = fs_fhs;
                        break;
                    case 'o':
                        in_dynamic_libraries = sx_false;
                        break;
                    case 'O':
                        in_dynamic_libraries = sx_true;
                        break;
                    case 's':
                        im.filesystem_layout = fs_afsl;
                        break;
                    case 'l':
                        if (curie_argv[xn])
                        {
                            i_destlibdir = make_string(curie_argv[xn]);
                            xn++;
                        }
                        break;
                    case 'a':
                        if (curie_argv[xn] && curie_argv[(xn + 1)])
                        {
                            i_alternatives
                                    = cons (cons
                                        (make_symbol(curie_argv[xn]),
                                         make_string(curie_argv[(xn + 1)])),
                                      i_alternatives);

                            xn += 2;
                        }
                        break;
                    case 'j':
                        if (curie_argv[xn])
                        {
                            char *s = curie_argv[xn];
                            unsigned int j;

                            max_processes = 0;

                            for (j = 0; s[j]; j++)
                            {
                                max_processes = 10 * max_processes + (s[j]-'0');
                            }

                            xn++;
                        }

                        if (max_processes == 0)
                        {
                            max_processes = 1;
                        }
                        break;
                    case 'r':
                        im.options |=  ICEMAKE_OPTION_TESTS;
                        break;
                    case 'v':
                        print_version ();
                        return 0;
                    case 'h':
                    case '-':
                        print_help (curie_argv[0]);
                        return 0;
                }

                y++;
            }

            i = xn;
        }
        else
        {
            im.buildtargets =
                sx_set_add (im.buildtargets, make_string (curie_argv[i]));
            i++;
        }
    }

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

    p_diff = icemake_which ((void *)0, "diff", "DIFF");

    if (target_architecture != (const char *)0)
    {
        return with_architecture
            (target_architecture, (void *)&im);
    }
    else
    {
        return icemake_default_architecture
            (with_architecture,   (void *)&im);
    }

}
