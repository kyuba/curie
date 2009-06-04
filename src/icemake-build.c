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

#include <curie/tree.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>

#include <icemake/icemake.h>

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
    lis = permutate_paths_os (sx_string_dir_prefix_c ("valgrind", p), lis);

    return lis;
}

static sexpr prepend_includes_gcc (sexpr x)
{
    sexpr include_paths = permutate_paths (str_include);
    sexpr cur = include_paths;

    while (consp (cur))
    {
        sexpr sxcar = car(cur);
        char buffer [BUFFERSIZE];

        snprintf (buffer, BUFFERSIZE, "-I%s", sx_string(sxcar));

        x = cons (make_string (buffer), x);

        cur = cdr (cur);
    }

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
    define_string (str_ffreestanding, "-ffreestanding");

    char *f = getenv ("CFLAGS");

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

    return prepend_ccflags_gcc(x);
}

static sexpr prepend_includes_borland (sexpr x)
{
    sexpr include_paths = permutate_paths (str_include);
    sexpr cur = include_paths;

    while (consp (cur))
    {
        sexpr sxcar = car(cur);
        char buffer [BUFFERSIZE];

        snprintf (buffer, BUFFERSIZE, "-I%s", sx_string(sxcar));

        x = cons (make_string (buffer), x);

        cur = cdr (cur);
    }

    return x;
}

static sexpr prepend_ccflags_borland (sexpr x)
{
    return x;
}

static sexpr prepend_cflags_borland (sexpr x)
{
    char *f = getenv ("CFLAGS");

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

    return prepend_ccflags_borland(x);
}

static sexpr prepend_cxxflags_borland (sexpr x)
{
    char *f = getenv ("CXXFLAGS");

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

    return prepend_ccflags_borland(x);
}

static void build_object_gcc_assembly (const char *source, const char *target)
{
    workstack
        = cons (cons (p_assembler,
/*                  cons (str_dc,*/
                    cons (make_string (source),
                      cons (str_do,
                        cons (make_string(target), sx_end_of_list))))/*)*/
                , workstack);
}

static void build_object_gcc_preproc_assembly (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
                  prepend_includes_gcc (
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list)))))),
                workstack);
}

static void build_object_gcc_c (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
#if defined(POSIX)
                  cons (str_dposix,
#endif
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (
                        prepend_includes_gcc (
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target), sx_end_of_list)))))))))))
#if defined(POSIX)
                  )
#endif
                  ,
                workstack);
}

static void build_object_gcc_c_combine (sexpr sources, const char *target)
{
    sexpr item =
#if defined(POSIX)
                 cons (str_dposix,
#endif
                   cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                         prepend_cflags_gcc (
                         prepend_includes_gcc (
                           cons (str_do,
                             cons (make_string (target), sx_end_of_list))))))))
#if defined(POSIX)
                 )
#endif
                 ;
    sexpr cur;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    item = cons (p_c_compiler, cons (str_dcombine, cons (str_dc, item)));

    workstack = cons (item, workstack);
}

static void build_object_gcc_cpp (const char *source, const char *target)
{
    workstack
        = cons (cons (p_cpp_compiler,
#if defined(POSIX)
                  cons (str_dposix,
#endif
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (
                    prepend_includes_gcc (
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list))))))))
#if defined(POSIX)
                  )
#endif
                  ,
                workstack);
}

static void build_object_gcc_assembly_pic (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
                  prepend_includes_gcc (
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))),
                workstack);
}

static void build_object_gcc_preproc_assembly_pic (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
                  prepend_includes_gcc (
                    cons (str_dfpic,
                    cons (str_dc,
                      cons (make_string (source),
                        cons (str_do,
                          cons (make_string(target), sx_end_of_list))))))),
                workstack);
}

static void build_object_gcc_c_pic (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
                  cons (str_dfpic,
#if defined(POSIX)
                  cons (str_dposix,
#endif
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (
                        prepend_includes_gcc (
                          cons (str_dc,
                            cons (make_string (source),
                              cons (str_do,
                                cons (make_string(target), sx_end_of_list))))))))))))
#if defined(POSIX)
                  )
#endif
                  ,
                workstack);
}

static void build_object_gcc_c_pic_combine (sexpr sources, const char *target)
{
    sexpr item = 
#if defined(POSIX)
                 cons (str_dposix,
#endif
                   cons (str_dgcc,
                   cons (str_stdc99,
                     cons (str_wall,
                       cons (str_pedantic,
                         prepend_cflags_gcc (
                         prepend_includes_gcc (
                           cons (str_do,
                             cons (make_string (target), sx_end_of_list))))))))
#if defined(POSIX)
                 )
#endif
                 ;
    sexpr cur;

    for (cur = sources; consp (cur); cur = cdr (cur))
    {
        item = cons (car (cur), item);
    }

    item = cons (p_c_compiler, cons (str_dfpic,
                 cons (str_dcombine, cons (str_dc, item))));

    workstack = cons (item, workstack);
}

static void build_object_gcc_cpp_pic (const char *source, const char *target)
{
    workstack
        = cons (cons (p_cpp_compiler,
                  cons (str_dfpic,
#if defined(POSIX)
                  cons (str_dposix,
#endif
                  cons (str_dgcc,
                    prepend_cxxflags_gcc (
                    prepend_includes_gcc (
                      cons (str_dc,
                        cons (make_string (source),
                          cons (str_do,
                            cons (make_string(target), sx_end_of_list)))))))))
#if defined(POSIX)
                  )
#endif
                  ,
                workstack);
}

static void build_object_gcc (sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_assembly)))
    {
        build_object_gcc_assembly (sx_string(source), sx_string(target));
    }
    else if (truep(equalp(type, sym_preproc_assembly)))
    {
        build_object_gcc_preproc_assembly (sx_string(source), sx_string(target));
    }
    else if (truep(equalp(type, sym_c)))
    {
        if (consp (source))
        {
            build_object_gcc_c_combine (source, sx_string(target));
        }
        else
        {
            build_object_gcc_c (sx_string(source), sx_string(target));
        }
    }
    else if (truep(equalp(type, sym_cpp)))
    {
        build_object_gcc_cpp (sx_string(source), sx_string(target));
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
            build_object_gcc_c_pic_combine (source, sx_string(target));
        }
        else
        {
            build_object_gcc_c_pic (sx_string(source), sx_string(target));
        }
    }
    else if (truep(equalp(type, sym_cpp_pic)))
    {
        build_object_gcc_cpp_pic (sx_string(source), sx_string(target));
    }
    else
    {
        fprintf (stderr, "Unknown code file type: %s\n", sx_symbol(type));
        exit (20);
    }
}

static void build_object_borland_generic (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
                  cons (str_dAT,
                  cons (str_dq,
                    cons (str_dw,
                      prepend_cflags_borland (
                      prepend_includes_borland (
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list)))))))))),
                workstack);
}

static void build_object_borland_cpp (const char *source, const char *target)
{
    workstack
        = cons (cons (p_c_compiler,
                  cons (str_dAT,
                  cons (str_dq,
                  cons (str_dP,
                    cons (str_dw,
                      prepend_cflags_borland (
                      prepend_includes_borland (
                        cons (str_do,
                          cons (make_string (target),
                            cons (str_dc,
                              cons (make_string(source), sx_end_of_list))))))))))),
                workstack);
}

static void build_object_borland (sexpr type, sexpr source, sexpr target)
{
    if (truep(equalp(type, sym_link))) return;

    if (truep(equalp(type, sym_cpp)))
    {
        build_object_borland_cpp (sx_string(source), sx_string(target));
    }
    else
    {
        build_object_borland_generic (sx_string(source), sx_string(target));
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
        case tc_borland:
            build_object_borland (type, source, target); break;
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
    sexpr use_objects = sx_end_of_list;

    sx_write (stdio, cons (sym_phase, cons (sym_build, sx_end_of_list)));

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

    loop_processes();
}
