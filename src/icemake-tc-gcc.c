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

static sexpr prepend_includes_common (struct target *t, sexpr x)
{
    sexpr include_paths = icemake_permutate_paths (t->toolchain, str_include);
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

static sexpr prepend_ccflags_gcc (struct toolchain_descriptor *td, sexpr x)
{
    if (td->options & ICEMAKE_OPTION_FREESTANDING)
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

static sexpr prepend_cflags_gcc (struct toolchain_descriptor *td, sexpr x)
{
    define_string (str_ffreestanding, "-ffreestanding");

    x = prepend_flags_from_environment (x, "CFLAGS");

    if (td->options & ICEMAKE_OPTION_FREESTANDING)
    {
        x = cons (str_ffreestanding, x);
    }

    return prepend_ccflags_gcc(td, x);
}

static sexpr prepend_cxxflags_gcc (struct target *t, sexpr x)
{
    define_string (str_dfno_exceptions, "-fno-exceptions");
    define_string (str_dfno_rtti,       "-fno-rtti");

    x = prepend_flags_from_environment (x, "CXXFLAGS");

    x = cons (str_dfno_rtti, cons (str_dfno_exceptions, x));

    return prepend_ccflags_gcc(t->toolchain, x);
}

static void build_object_gcc_assembly
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.as,
                    cons (make_string (source),
                      cons (str_do,
                        cons (make_string(target), sx_end_of_list)))));
}

static void build_object_gcc_preproc_assembly
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
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
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (t->toolchain,
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
                         prepend_cflags_gcc (t->toolchain,
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

    item = cons (t->toolchain->meta_toolchain.gcc.gcc, sx);

    t->icemake->workstack = sx_set_add (t->icemake->workstack, item);
}

static void build_object_gcc_cpp
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gpp,
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
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
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
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
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
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gcc,
                  cons (str_dfpic,
                  cons (str_dgcc,
                  cons (str_stdc99,
                    cons (str_wall,
                      cons (str_pedantic,
                        prepend_cflags_gcc (t->toolchain,
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
                         prepend_cflags_gcc (t->toolchain,
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

    item = cons (t->toolchain->meta_toolchain.gcc.gcc, cons (str_dfpic, sx));

    t->icemake->workstack = sx_set_add (t->icemake->workstack, item);
}

static void build_object_gcc_cpp_pic
    (const char *source, const char *target, struct target *t)
{
    t->icemake->workstack =
        sx_set_add (t->icemake->workstack,
            cons (t->toolchain->meta_toolchain.gcc.gpp,
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
    (struct target *t, sexpr type, sexpr source, sexpr target)
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

int icemake_prepare_toolchain_gcc (struct toolchain_descriptor *td)
{
    td->meta_toolchain.gcc.gcc = icemake_which (td, "gcc");
    td->meta_toolchain.gcc.ld  = icemake_which (td, "ld");
    td->meta_toolchain.gcc.gpp = icemake_which (td, "g++");
    td->meta_toolchain.gcc.as  = icemake_which (td, "as");
    td->meta_toolchain.gcc.ar  = icemake_which (td, "ar");

    td->build_object = build_object_gcc;

    return (falsep (td->meta_toolchain.gcc.gcc) ? 1 : 0) + 
           (falsep (td->meta_toolchain.gcc.ld)  ? 1 : 0) + 
           (falsep (td->meta_toolchain.gcc.gpp) ? 1 : 0) + 
           (falsep (td->meta_toolchain.gcc.as)  ? 1 : 0) +
           (falsep (td->meta_toolchain.gcc.ar)  ? 1 : 0);
}
