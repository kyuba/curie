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

#include <curie/filesystem.h>
#include <icemake/icemake.h>

static sexpr f_exist_add (sexpr f, sexpr lis)
{
    return truep (filep (f)) ? cons (f, lis) : lis;
}

static sexpr permutate_paths_vendor (sexpr p, sexpr lis)
{
    lis = f_exist_add (sx_string_dir_prefix_c (uname_vendor, p), lis);
    lis = f_exist_add (p, lis);

    return lis;
}

static sexpr permutate_paths_toolchain (sexpr p, sexpr lis)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            lis = permutate_paths_vendor (sx_string_dir_prefix_c ("gnu", p), lis);
            break;
        case tc_borland:
            lis = permutate_paths_vendor (sx_string_dir_prefix_c ("borland", p), lis);
            break;
        case tc_msvc:
            lis = permutate_paths_vendor (sx_string_dir_prefix_c ("msvc", p), lis);
            break;
    }
    lis = permutate_paths_vendor (p, lis);

    return lis;
}

static sexpr permutate_paths_arch (sexpr p, sexpr lis)
{
    lis = permutate_paths_toolchain (sx_string_dir_prefix_c (uname_arch, p), lis);
    lis = permutate_paths_toolchain (p, lis);

    return lis;
}

static sexpr permutate_paths_os (sexpr p, sexpr lis)
{
    lis = permutate_paths_arch (sx_string_dir_prefix_c (uname_os, p), lis);
    lis = permutate_paths_arch (p, lis);
    lis = permutate_paths_arch (sx_string_dir_prefix_c ("posix", p), lis);
    lis = permutate_paths_arch (sx_string_dir_prefix_c ("ansi", p), lis);
    lis = permutate_paths_arch (sx_string_dir_prefix_c ("generic", p), lis);

    return lis;
}

static sexpr permutate_paths (sexpr p)
{
    sexpr lis = sx_end_of_list;

    lis = permutate_paths_os (p, lis);
    lis = permutate_paths_os (sx_string_dir_prefix_c ("internal", p), lis);
    lis = permutate_paths_os (sx_string_dir_prefix_c ("debug", p), lis);

    return lis;
}

static sexpr prepend_includes_common (struct target *t, sexpr x)
{
    sexpr include_paths = permutate_paths (str_include);
    sexpr cur = include_paths;

    if (stringp (i_destdir))
    {
        x = cons (sx_join (str_dI,
                           get_install_file (t, str_include), sx_nil), x);
    }

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        x = cons (sx_join (str_dI, sxcar, sx_nil), x);

        cur = cdr (cur);
    }
        
    return cons (sx_join (str_dIbuilds, architecture, str_sinclude), x);
}

static sexpr prepend_includes_gcc (struct target *t, sexpr x)
{
    return prepend_includes_common (t, x);
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
    define_string (str_ffreestanding, "-ffreestanding");

    x = prepend_flags_from_environment (x, "CFLAGS");

    if (truep (co_freestanding))
    {
        x = cons (str_ffreestanding, x);
    }

    return prepend_ccflags_gcc(x);
}

static sexpr prepend_cxxflags_gcc (struct target *t, sexpr x)
{
    define_string (str_dfno_exceptions, "-fno-exceptions");
    define_string (str_dfno_rtti,       "-fno-rtti");

    x = prepend_flags_from_environment (x, "CXXFLAGS");

    x = cons (str_dfno_rtti, cons (str_dfno_exceptions, x));

    return prepend_ccflags_gcc(x);
}

static sexpr prepend_includes_borland (struct target *t, sexpr x)
{
    return prepend_includes_common (t, x);
}

static sexpr prepend_cflags_borland (sexpr x)
{
    return prepend_flags_from_environment (x, "CFLAGS");
}

static sexpr prepend_cxxflags_borland (sexpr x)
{
    return prepend_flags_from_environment (x, "CXXFLAGS");
}

static sexpr prepend_includes_msvc (struct target *t, sexpr x)
{
    sexpr include_paths = permutate_paths (str_include);
    sexpr cur = include_paths;

    if (stringp (i_destdir))
    {
        x = cons (sx_join (str_sI,
                           get_install_file (t, str_include), sx_nil), x);
    }

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        x = cons (sx_join (str_sI, sxcar, sx_nil), x);

        cur = cdr (cur);
    }

    return cons (sx_join (str_sIbuilds, architecture, str_sinclude), x);
}

static sexpr prepend_cflags_msvc (sexpr x)
{
    return prepend_flags_from_environment (x, "CFLAGS");
}

static sexpr prepend_cxxflags_msvc (sexpr x)
{
    return prepend_flags_from_environment (x, "CXXFLAGS");
}

static void build_object_gcc_assembly
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_assembler,
                    cons (make_string (source),
                      cons (str_do,
                        cons (make_string(target), sx_end_of_list)))));
}

static void build_object_gcc_preproc_assembly
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                  prepend_includes_gcc (t,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list)))))));
}

static void build_object_gcc_c
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (
                        prepend_includes_gcc (t,
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target),
                                  sx_end_of_list))))))))))));
}

static void build_object_gcc_c_combine
    (sexpr sources, const char *target, struct target *t)
{
    sexpr item = cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                         prepend_cflags_gcc (
                         prepend_includes_gcc (t,
                           cons (str_do,
                             cons (make_string (target), sx_end_of_list))))))));
    sexpr cur, sx;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    sx = cons (str_dcombine, cons (str_dc, item));

/*
    if ((t->options & ICEMAKE_PROGRAMME) && eolp (cdr (t->code)))
    {
        sx = cons (str_dfwhole_program, sx);
    }
*/

    item = cons (p_c_compiler, sx);

    t->icemake->workstack = sx_set_add (t->icemake->workstack, item);
}

static void build_object_gcc_cpp
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_cpp_compiler,
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (t,
                    prepend_includes_gcc (t,
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list)))))))));
}

static void build_object_gcc_assembly_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                  prepend_includes_gcc (t,
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))));
}

static void build_object_gcc_preproc_assembly_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                  prepend_includes_gcc (t,
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))));
}

static void build_object_gcc_c_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                  cons (str_dfpic,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (
                        prepend_includes_gcc (t,
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target),
                                  sx_end_of_list)))))))))))));
}

static void build_object_gcc_c_pic_combine
    (sexpr sources, const char *target, struct target *t)
{
    sexpr item = cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                         prepend_cflags_gcc (
                         prepend_includes_gcc (t,
                           cons (str_do,
                             cons (make_string (target), sx_end_of_list))))))));
    sexpr cur, sx;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    sx = cons (str_dcombine, cons (str_dc, item));

/*
    if ((t->options & ICEMAKE_PROGRAMME) && eolp (cdr (t->code)))
    {
        sx = cons (str_dfwhole_program, sx);
    }
*/

    item = cons (p_c_compiler, cons (str_dfpic, sx));

    t->icemake->workstack = sx_set_add (t->icemake->workstack, item);
}

static void build_object_gcc_cpp_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
                cons (p_cpp_compiler,
                  cons (str_dfpic,
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (t,
                    prepend_includes_gcc (t,
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list))))))))));
}

static void build_object_gcc
    (sexpr type, sexpr source, sexpr target, struct target *t)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_resource)))
    {
        /* STUB */
    }
    else if (truep(equalp(type, sym_assembly)))
    {
        build_object_gcc_assembly
            (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_preproc_assembly)))
    {
        build_object_gcc_preproc_assembly
            (sx_string(source),sx_string(target), t);
    }
    else if (truep(equalp(type, sym_c)))
    {
        if (consp (source))
        {
            build_object_gcc_c_combine
                (source, sx_string(target), t);
        }
        else
        {
            build_object_gcc_c (sx_string(source), sx_string(target), t);
        }
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_gcc_cpp (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_assembly_pic)))
    {
        build_object_gcc_assembly_pic
            (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_preproc_assembly_pic)))
    {
        build_object_gcc_preproc_assembly_pic
            (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_c_pic)))
    {
        if (consp (source))
        {
            build_object_gcc_c_pic_combine
                (source, sx_string(target), t);
        }
        else
        {
            build_object_gcc_c_pic
                (sx_string(source), sx_string(target), t);
        }
    }
    else if (truep(equalp(type, sym_cpp_pic)))
    {
        build_object_gcc_cpp_pic
            (sx_string(source), sx_string(target), t);
    }
    else
    {
        on_error (ie_unknown_code_file_type, sx_symbol (type));
    }
}

static void build_object_borland_generic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                  cons (str_dAT,
                  cons (str_dq,
                    cons (str_dw,
                      prepend_cflags_borland (
                      prepend_includes_borland (t,
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list)))))))))));
}

static void build_object_borland_cpp
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                  cons (str_dAT,
                  cons (str_dq,
                  cons (str_dP,
                    cons (str_dw,
                      prepend_cxxflags_borland (
                      prepend_includes_borland (t,
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list))))))))))));
}

static void build_object_borland
    (sexpr type, sexpr source, sexpr target, struct target *t)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_resource)))
    {
        /* STUB */
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_borland_cpp (sx_string(source), sx_string(target), t);
    }
    else
    {
        build_object_borland_generic (sx_string(source), sx_string(target), t);
    }
}

static void build_object_msvc_generic
    (const char *source, const char *target, struct target *t)
{
    sexpr b = sx_join (str_sFo, make_string (target), sx_nil);

    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                      cons (str_sc,
                      cons (str_snologo,
                      cons (str_sTC,
                        prepend_cflags_msvc (
                        prepend_includes_msvc (t,
                          cons (make_string (source),
                              cons (b, sx_end_of_list)))))))));
}

static void build_object_msvc_cpp
    (const char *source, const char *target, struct target *t)
{
    sexpr b = sx_join (str_sFo, make_string (target), sx_nil);

    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_c_compiler,
                      cons (str_sc,
                      cons (str_snologo,
                      cons (str_sTP,
                      cons (str_sEHsc,
                        prepend_cxxflags_msvc (
                        prepend_includes_msvc (t,
                          cons (make_string (source),
                              cons (b, sx_end_of_list))))))))));
}

static void build_object_msvc_resource
    (const char *source, const char *target, struct target *t)
{
    sexpr b = sx_join (str_sfo, make_string (target), sx_nil);

    t->icemake->workstack =
        sx_set_add (t->icemake->workstack, cons (p_resource_compiler,
                      cons (b, cons (make_string(source), sx_end_of_list))));
}

static void build_object_msvc
    (sexpr type, sexpr source, sexpr target, struct target *t)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_resource)))
    {
        build_object_msvc_resource (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_msvc_cpp (sx_string(source), sx_string(target), t);
    }
    else
    {
        build_object_msvc_generic (sx_string(source), sx_string(target), t);
    }
}

static void build_object(sexpr desc, struct target *t)
{
    sexpr type = car(desc);
    sexpr source = car(cdr(desc));
    sexpr target = car(cdr(cdr(desc)));

    switch (uname_toolchain)
    {
        case tc_gcc:
            build_object_gcc     (type, source, target, t); break;
        case tc_borland:
            build_object_borland (type, source, target, t); break;
        case tc_msvc:
            build_object_msvc    (type, source, target, t); break;
    }
}

static void create_special_files (struct target *t)
{
    char buffer[BUFFERSIZE];
    int i = 0;
    struct io *io;
    char c;
    sexpr b = sx_join (t->buildbase, architecture,
                sx_join (str_sincludes, t->name, str_sversiondh)),
          n, o;
    const char *nx = sx_string (t->name), *ox;

    io = io_open_write (sx_string (b));

    io_collect (io, "/* this file was generated by icemake */\n\n", 42);

    for (i = 0; (i < (BUFFERSIZE - 2)) && nx[i]; i++)
    {
        c = nx[i];

        if (c == '+')
        {
            buffer[i] = 'x';
        }
        else if ((c < 'a') && (c > 'z') && (c < 'A') && (c > 'Z'))
        {
            buffer[i] = '_';
        }
        else
        {
            buffer[i] = c;
        }
    }

    buffer[i] = 0;

    n = make_string (buffer);

    o = sx_join (str_hdefines, n,
          sx_join (str_uname, str_sq,
            sx_join (t->name, str_qnl, sx_nil)));

    if (stringp (t->durl))
    {
        o = sx_join (o, str_hdefines,
              sx_join (n, str_uurl,
                sx_join (str_sq, t->durl, str_qnl)));
    }
        
    o = sx_join (o, str_hdefines,
          sx_join (n, str_uversion,
            sx_join (str_space, t->dversion, str_nl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_uversion_s,
            sx_join (str_sq, t->dversion, str_qnl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_ubuild_number,
            sx_join (str_space, t->buildnumber, str_nl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_ubuild_number_s,
            sx_join (str_sq, t->buildnumber, str_qnl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_uversion_long,
            sx_join (str_sq, t->name,
              sx_join (str_dash, t->dversion,
                sx_join (str_dot, t->buildnumber, str_qnl)))));

    ox = sx_string (o);
    for        (i = 0; ox[i]; i++);
    io_collect (io, ox, i);

    io_flush   (io);
    io_close   (io);
}

static int do_build_target (struct target *t)
{
    sexpr c = t->code;

    if (t->icemake->toolchain->build != (int (*)(struct target *))0)
    {
        return t->icemake->toolchain->build (t);
    }

    t->buildnumber = make_integer (sx_integer (t->buildnumber) + 1);

    create_special_files (t);

    while (consp (c))
    {
        build_object(car(c), t);

        c = cdr (c);
    }

    if (truep(do_tests))
    {
        c = t->test_cases;

        while (consp (c))
        {
            build_object(car(c), t);

            c = cdr (c);
        }
    }

    return 0;
}

int icemake_build (struct icemake *im)
{
    sexpr cursor = im->buildtargets;

    sx_write (stdio, cons (sym_phase, cons (sym_build, sx_end_of_list)));

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);
        struct tree_node *node =
            tree_get_node_string (&(im->targets), (char *)target);

        if (node != (struct tree_node *)0)
        {
            do_build_target (node_get_value(node));
        }

        cursor = cdr(cursor);
    }

    return icemake_loop_processes (im);
}
