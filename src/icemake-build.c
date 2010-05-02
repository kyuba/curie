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

#include <icemake/icemake.h>

#include <sievert/tree.h>

#include <stdlib.h>
#include <stdio.h>

static sexpr f_exist_add (sexpr f, sexpr lis)
{
    struct stat st;

    if (stat (sx_string (f), &st) == 0)
    {
        return cons (f, lis);
    }

    return lis;
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

static sexpr prepend_includes_common (sexpr x)
{
    sexpr include_paths = permutate_paths (str_include);
    sexpr cur = include_paths;
    char buffer [BUFFERSIZE];

    if (stringp (i_destdir))
    {
        switch (i_fsl)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                snprintf (buffer, BUFFERSIZE, "-I%s/include", sx_string(i_destdir));
                break;
            case fs_afsl:
                snprintf (buffer, BUFFERSIZE, "-I%s/%s/%s/include", sx_string(i_destdir), uname_os, uname_arch);
                break;
        }
        
        x = cons (make_string (buffer), x);
    }

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        snprintf (buffer, BUFFERSIZE, "-I%s", sx_string(sxcar));

        x = cons (make_string (buffer), x);

        cur = cdr (cur);
    }

    snprintf (buffer, BUFFERSIZE, "-Ibuild/%s/include", archprefix);
    x = cons (make_string (buffer), x);

    return x;
}

static sexpr prepend_includes_gcc (sexpr x)
{
    return prepend_includes_common (x);
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

    if (falsep (t->allow_exceptions))
    {
        x = cons (str_dfno_rtti, cons (str_dfno_exceptions, x));
    }

    return prepend_ccflags_gcc(x);
}

static sexpr prepend_includes_borland (sexpr x)
{
    return prepend_includes_common (x);
}

static sexpr prepend_cflags_borland (sexpr x)
{
    return prepend_flags_from_environment (x, "CFLAGS");
}

static sexpr prepend_cxxflags_borland (sexpr x)
{
    return prepend_flags_from_environment (x, "CXXFLAGS");
}

static sexpr prepend_includes_msvc (sexpr x)
{
    sexpr include_paths = permutate_paths (str_include);
    sexpr cur = include_paths;
    char buffer[BUFFERSIZE];

    if (stringp (i_destdir))
    {
        switch (i_fsl)
        {
            case fs_fhs:
            case fs_fhs_binlib:
                snprintf (buffer, BUFFERSIZE, "/I%s\\include", sx_string(i_destdir));
                break;
            case fs_afsl:
                snprintf (buffer, BUFFERSIZE, "/I%s\\%s\\%s\\include", sx_string(i_destdir), uname_os, uname_arch);
                break;
        }
        
        x = cons (make_string (buffer), x);
    }

    while (consp (cur))
    {
        sexpr sxcar = car(cur);

        snprintf (buffer, BUFFERSIZE, "/I%s", sx_string(sxcar));

        x = cons (make_string (buffer), x);

        cur = cdr (cur);
    }

    snprintf (buffer, BUFFERSIZE, "/Ibuild\\%s\\include", archprefix);
    x = cons (make_string (buffer), x);

    return x;
}

static sexpr prepend_cflags_msvc (sexpr x)
{
    return prepend_flags_from_environment (x, "CFLAGS");
}

static sexpr prepend_cxxflags_msvc (sexpr x)
{
    return prepend_flags_from_environment (x, "CXXFLAGS");
}

static void build_object_gcc_assembly (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_assembler,
/*                  cons (str_dc,*/
                    cons (make_string (source),
                      cons (str_do,
                        cons (make_string(target), sx_end_of_list))))/*)*/);
}

static void build_object_gcc_preproc_assembly (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_c_compiler,
                  prepend_includes_gcc (
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list)))))));
}

static void build_object_gcc_c (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_c_compiler,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (
                        prepend_includes_gcc (
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target), sx_end_of_list))))))))))));
}

static void build_object_gcc_c_combine (sexpr sources, const char *target,
                                        struct target *t)
{
    sexpr item = cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                         prepend_cflags_gcc (
                         prepend_includes_gcc (
                           cons (str_do,
                             cons (make_string (target), sx_end_of_list))))))));
    sexpr cur, sx;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    sx = cons (str_dcombine, cons (str_dc, item));

/*
    if (truep (t->programme) && eolp (cdr (t->code)))
    {
        sx = cons (str_dfwhole_program, sx);
    }
*/

    item = cons (p_c_compiler, sx);

    workstack = sx_set_add (workstack, item);
}

static void build_object_gcc_cpp
    (const char *source, const char *target, struct target *t)
{
    workstack = sx_set_add (workstack, cons (p_cpp_compiler,
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (t,
                    prepend_includes_gcc (
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list)))))))));
}

static void build_object_gcc_assembly_pic (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_c_compiler,
                  prepend_includes_gcc (
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))));
}

static void build_object_gcc_preproc_assembly_pic (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_c_compiler,
                  prepend_includes_gcc (
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))));
}

static void build_object_gcc_c_pic (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_c_compiler,
                  cons (str_dfpic,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (
                        prepend_includes_gcc (
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target), sx_end_of_list)))))))))))));
}

static void build_object_gcc_c_pic_combine (sexpr sources, const char *target,
                                            struct target *t)
{
    sexpr item = cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                         prepend_cflags_gcc (
                         prepend_includes_gcc (
                           cons (str_do,
                             cons (make_string (target), sx_end_of_list))))))));
    sexpr cur, sx;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    sx = cons (str_dcombine, cons (str_dc, item));

/*
    if (truep (t->programme) && eolp (cdr (t->code)))
    {
        sx = cons (str_dfwhole_program, sx);
    }
*/

    item = cons (p_c_compiler, cons (str_dfpic, sx));

    workstack = sx_set_add (workstack, item);
}

static void build_object_gcc_cpp_pic
    (const char *source, const char *target, struct target *t)
{
    workstack
        = cons (cons (p_cpp_compiler,
                  cons (str_dfpic,
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (t,
                    prepend_includes_gcc (
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list))))))))),
                workstack);
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
        build_object_gcc_assembly (sx_string(source), sx_string(target));
    }
    else if (truep(equalp(type, sym_preproc_assembly)))
    {
        build_object_gcc_preproc_assembly (sx_string(source),sx_string(target));
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
            build_object_gcc_c (sx_string(source), sx_string(target));
        }
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_gcc_cpp (sx_string(source), sx_string(target), t);
    }
    else if (truep(equalp(type, sym_assembly_pic)))
    {
        build_object_gcc_assembly_pic (sx_string(source), sx_string(target));
    }
    else if (truep(equalp(type, sym_preproc_assembly_pic)))
    {
        build_object_gcc_preproc_assembly_pic (sx_string(source), sx_string(target));
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
            build_object_gcc_c_pic (sx_string(source), sx_string(target));
        }
    }
    else if (truep(equalp(type, sym_cpp_pic)))
    {
        build_object_gcc_cpp_pic (sx_string(source), sx_string(target), t);
    }
    else
    {
        fprintf (stderr, "Unknown code file type: %s\n", sx_symbol(type));
        exit (20);
    }
}

static void build_object_borland_generic (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_c_compiler,
                  cons (str_dAT,
                  cons (str_dq,
                    cons (str_dw,
                      prepend_cflags_borland (
                      prepend_includes_borland (
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list)))))))))));
}

static void build_object_borland_cpp (const char *source, const char *target)
{
    workstack = sx_set_add (workstack, cons (p_c_compiler,
                  cons (str_dAT,
                  cons (str_dq,
                  cons (str_dP,
                    cons (str_dw,
                      prepend_cxxflags_borland (
                      prepend_includes_borland (
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list))))))))))));
}

static void build_object_borland (sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_resource)))
    {
        /* STUB */
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_borland_cpp (sx_string(source), sx_string(target));
    }
    else
    {
        build_object_borland_generic (sx_string(source), sx_string(target));
    }
}

static void build_object_msvc_generic (const char *source, const char *target)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "/Fo%s", target);

    workstack = sx_set_add (workstack, cons (p_c_compiler,
                      cons (str_sc,
                      cons (str_snologo,
                      cons (str_sTC,
                        prepend_cflags_msvc (
                        prepend_includes_msvc (
                          cons (make_string (source),
                              cons (make_string(buffer), sx_end_of_list)))))))));
}

static void build_object_msvc_cpp (const char *source, const char *target)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "/Fo%s", target);

    workstack = sx_set_add (workstack, cons (p_c_compiler,
                      cons (str_sc,
                      cons (str_snologo,
                      cons (str_sTP,
                      cons (str_sEHsc,
                        prepend_cxxflags_msvc (
                        prepend_includes_msvc (
                          cons (make_string (source),
                              cons (make_string(buffer), sx_end_of_list))))))))));
}

static void build_object_msvc_resource (const char *source, const char *target)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "/fo%s", target);

    workstack = sx_set_add (workstack, cons (p_resource_compiler,
                      cons (make_string (buffer),
                            cons (make_string(source), sx_end_of_list))));
}

static void build_object_msvc (sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_resource)))
    {
        build_object_msvc_resource (sx_string(source), sx_string(target));
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_msvc_cpp (sx_string(source), sx_string(target));
    }
    else
    {
        build_object_msvc_generic (sx_string(source), sx_string(target));
    }
}

static void build_object(sexpr desc, struct target *t)
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
            build_object_gcc     (type, source, target, t); break;
        case tc_borland:
            build_object_borland (type, source, target);    break;
        case tc_msvc:
            build_object_msvc    (type, source, target);    break;
    }
}

static void create_special_files (struct target *t)
{
    char buffer[BUFFERSIZE];
    char nbuffer[BUFFERSIZE];
    int i = 0;
    struct io *io;
    char c;

    snprintf (buffer, BUFFERSIZE, "build/%s/include/%s/version.h",
              archprefix, sx_string(t->name));

    io = io_open_write (buffer);

    io_collect (io, "/* this file was generated by icemake */\n\n", 42);

    snprintf (nbuffer, BUFFERSIZE, "%s", sx_string(t->name));

    for (i = 0; nbuffer[i]; i++)
    {
        c = buffer[i];

        if (c == '+')
        {
            c = 'x';
        }
        else if ((c < 'a') && (c > 'z') && (c < 'A') && (c > 'Z'))
        {
            buffer[i] = '_';
        }
    }

    snprintf   (buffer, BUFFERSIZE, "#define %s_name \"%s\"\n",
                nbuffer, sx_string (t->name));
    for        (i = 0; buffer[i]; i++);
    io_collect (io, buffer, i);

    if (stringp (t->durl))
    {
        snprintf   (buffer, BUFFERSIZE, "#define %s_url \"%s\"\n",
                    nbuffer, sx_string (t->durl));
        for        (i = 0; buffer[i]; i++);
        io_collect (io, buffer, i);
    }

    snprintf   (buffer, BUFFERSIZE, "#define %s_version %s\n",
                nbuffer, sx_string (t->dversion));
    for        (i = 0; buffer[i]; i++);
    io_collect (io, buffer, i);

    snprintf   (buffer, BUFFERSIZE, "#define %s_version_s \"%s\"\n",
                nbuffer, sx_string (t->dversion));
    for        (i = 0; buffer[i]; i++);
    io_collect (io, buffer, i);

    snprintf   (buffer, BUFFERSIZE, "#define %s_build_number %li\n",
                nbuffer, sx_integer (t->buildnumber));
    for        (i = 0; buffer[i]; i++);
    io_collect (io, buffer, i);

    snprintf   (buffer, BUFFERSIZE, "#define %s_build_number_s \"%li\"\n",
                nbuffer, sx_integer (t->buildnumber));
    for        (i = 0; buffer[i]; i++);
    io_collect (io, buffer, i);

    snprintf   (buffer, BUFFERSIZE, "#define %s_version_long \"%s-%s.%li\"\n",
                nbuffer, sx_string (t->name), sx_string(t->dversion),
                sx_integer (t->buildnumber));
    for        (i = 0; buffer[i]; i++);
    io_collect (io, buffer, i);

    io_flush   (io);
    io_close   (io);
}

static void do_build_target (struct target *t)
{
    sexpr c = t->code;

    t->buildnumber = make_integer (sx_integer (t->buildnumber) + 1);

    create_special_files (t);

    while (consp (c))
    {
        build_object(car(c), t);

        c = cdr (c);
    }

    c = t->bootstrap;

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
}

static void build_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_build_target (node_get_value(node));
    }
}

static void target_map_build (struct tree_node *node, void *u)
{
    do_build_target(node_get_value(node));
}

void build (sexpr buildtargets)
{
    sexpr cursor = buildtargets;

    sx_write (stdio, cons (sym_phase, cons (sym_build, sx_end_of_list)));

    if (eolp(cursor))
    {
        tree_map (&targets, target_map_build, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);

        build_target (target);
        cursor = cdr(cursor);
    }

    loop_processes();
}
