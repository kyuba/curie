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

#include <curie/memory.h>
#include <curie/multiplex.h>
#include <curie/filesystem.h>

#if defined(_WIN32)
/* windows comes with dedicated file copy functions, so let's use those... */
int icemake_install_file
    (struct icemake *im, sexpr spec)
{
    sexpr source = car (spec);
    sexpr target = cdr (spec);
    int mode = 0444;
    struct io *in, *out;

    if (integerp (source))
    {
        mode   = sx_integer (source);
        source = car (target);
        target = cdr (target);
    }

    if (truep(equalp(source, sym_symlink))) /* no symlinks on windows */
    {
/*        source = car (target);
        target = cdr (target);

        sx_write (stdio, cons (sym_symlink,
                  cons (source, cons (target, sx_end_of_list))));

        symlink (sx_string(source), sx_string(target));*/
    }
    else if (truep (filep (source)))
    {
        sx_write (stdio, cons (sym_install, cons (make_integer (mode),
                  cons (source, cons (target, sx_end_of_list)))));

        mkdir_pi (target);
        CopyFile (sx_string (source), sx_string (target), FALSE);
    }

    return 0;
}

#else

struct file_metadata
{
    struct io      *out;
    struct icemake *icemake;
};

static void install_read (struct io *in, void *aux)
{
    struct file_metadata *meta = (struct file_metadata *)aux;

    if (((in->length) - (in->position)) > 0)
    {
        io_write (meta->out,
                  (in->buffer) + (in->position),
                  (in->length) - (in->position));

        in->position = in->length;
    }
}

static void install_close (struct io *in, void *aux)
{
    struct file_metadata *meta = (struct file_metadata *)aux;

    if (((in->length) - (in->position)) > 0)
    {
        io_write (meta->out,
                  (in->buffer) + (in->position),
                  (in->length) - (in->position));

        in->position = in->length;
    }

    (meta->icemake->alive_processes)--;

    free_pool_mem (aux);
}

int icemake_install_file
    (struct icemake *im, sexpr spec)
{
    sexpr source = car (spec);
    sexpr target = cdr (spec);
    int mode = 0444;
    struct io *in, *out;
    static struct memory_pool pool =
        MEMORY_POOL_INITIALISER (sizeof (struct file_metadata));
    struct file_metadata *meta;

    if (integerp (source))
    {
        mode   = sx_integer (source);
        source = car (target);
        target = cdr (target);
    }

    if (truep(equalp(source, sym_symlink)))
    {
        source = car (target);
        target = cdr (target);

        sx_write (stdio, cons (sym_symlink,
                  cons (source, cons (target, sx_end_of_list))));

        symlink (sx_string(source), sx_string(target));
    }
    else if (truep (filep (source)))
    {
        sx_write (stdio, cons (sym_install, cons (make_integer (mode),
                  cons (source, cons (target, sx_end_of_list)))));

        mkdir_pi (target);

        meta = (struct file_metadata *)get_pool_mem (&pool);

        in  = io_open_read   (sx_string (source));
        out = io_open_create (sx_string (target), mode);

        meta->out     = out;
        meta->icemake = im;

        (im->alive_processes)++;

        multiplex_add_io (in, install_read, install_close, (void *)meta);
        multiplex_add_io_no_callback (out);
    }

    return 0;
}
#endif

static sexpr get_library_install_path (struct target *t)
{
    return get_install_file
        (t, sx_join (i_destlibdir, str_slash,
              sx_join (str_lib, t->name,
                       (uname_toolchain == tc_gcc) ? str_dot_a : str_dot_lib)));
}

static sexpr get_so_library_install_path (struct target *t)
{
    return get_install_file
        (t, sx_join (((i_os == os_windows) ? str_bin : i_destlibdir), str_slash,
              sx_join (str_lib, t->name,
                       (i_os == os_windows)
                         ? sx_join (str_dot, t->dversion, str_dot_dll)
                         : sx_join (str_dot_so_dot, t->dversion, sx_nil))));
}

static sexpr get_so_library_symlink_path (struct target *t)
{
    return get_install_file
        (t, sx_join (((i_os == os_windows) ? str_bin : i_destlibdir), str_slash,
              sx_join (str_lib, t->name,
                       (i_os == os_windows) ? str_dot_dll : str_dot_so)));
}

static sexpr get_programme_install_path (struct target *t)
{
    return get_install_file
        (t, sx_join (str_bin, str_slash,
              sx_join (t->name,
                       (i_os == os_windows) ? str_dot_exe : sx_nil,
                       sx_nil)));
}

static sexpr get_documentation_install_path
    (struct target *t, sexpr name, sexpr file, sexpr version,
     const char *suffix)
{
    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            return sx_join (i_destdir, str_ssharesdocs,
                     sx_join (name, str_dash,
                       sx_join (version, str_slash,
                         sx_join (file, str_dot, make_string (suffix)))));
        case fs_afsl:
            return sx_join (i_destdir, str_sgenericsdocs,
                     sx_join (name, str_dash,
                       sx_join (version, str_slash,
                         sx_join (file, str_dot, make_string (suffix)))));
    }

    return sx_false;
}

static sexpr get_documentation_man_install_path
    (struct target *t, sexpr file, sexpr section)
{
    const char *s = sx_symbol (section);

    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            return sx_join (i_destdir, str_ssharesmans,
                     sx_join (make_string (s), str_slash,
                       sx_join (file, str_dot, make_string (s + 3))));
        case fs_afsl:
            return sx_join (i_destdir, str_sgenericsmans,
                     sx_join (make_string (s), str_slash,
                       sx_join (file, str_dot, make_string (s + 3))));
    }

    return sx_false;
}

static sexpr get_header_install_path
    (struct target *t, sexpr file)
{
    return get_install_file
        (t, sx_join (str_includes, t->name,
              sx_join (str_slash, file, str_dot_h)));
}

static sexpr get_data_install_path
    (struct target *t, sexpr name, sexpr file)
{
    switch (i_fsl)
    {
        case fs_fhs:
        case fs_fhs_binlib:
            return sx_join (i_destdir, str_setcs,
                     sx_join (name, str_slash, file));
        case fs_afsl:
            return sx_join (i_destdir, str_sgenericsconfigurations,
                     sx_join (name, str_slash, file));
    }

    return sx_false;
}

static void install_library_dynamic_common (sexpr name, struct target *t)
{
    if (truep (i_dynamic_libraries) &&
        !(t->options & ICEMAKE_NO_SHARED_LIBRARY) &&
        (!(t->options & ICEMAKE_HAVE_CPP) || (i_os != os_darwin)))
    {
        sexpr fname;

        switch (i_os)
        {
            case os_windows:
                fname = get_build_file
                          (t, sx_join (str_lib, name,
                                sx_join (str_dot, t->dversion, str_dot_dll)));
                break;
            default:
                fname = get_build_file
                          (t, sx_join (str_lib, name,
                                sx_join (str_dot_so_dot, t->dversion, sx_nil)));
        }

        if (truep(filep(fname)))
        {
            t->icemake->workstack = sx_set_add (t->icemake->workstack,
                        cons (sym_install,
                            cons (make_integer(0555),
                                cons (fname, get_so_library_install_path(t)))));

            switch (i_os)
            {
                case os_windows:
                    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                                cons (sym_install,
                                  cons (get_build_file
                                    (t, sx_join (str_lib, name,
                                                 str_dot_dll)),
                                  get_so_library_symlink_path (t))));
                    break;
                default:
                    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                                cons (sym_symlink,
                                      cons (sx_join (str_lib, name,
                                            sx_join (str_dot_so_dot,
                                                     t->dversion, sx_nil)),
                                            get_so_library_symlink_path (t))));
            }
        }
    }
}

static void install_library_gcc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                cons (sym_install,
                   cons (get_build_file (t, sx_join (str_lib, name, str_dot_a)),
                      get_library_install_path (t))));

    install_library_dynamic_common (name, t);
}

static void install_library_borland (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                cons (sym_install,
                   cons (get_build_file (t, sx_join (str_lib, name,
                                                     str_dot_lib)),
                      get_library_install_path (t))));

    install_library_dynamic_common (name, t);
}

static void install_library_msvc (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                cons (sym_install,
                   cons (get_build_file (t, sx_join (str_lib, name,
                                                     str_dot_lib)),
                      get_library_install_path (t))));

    install_library_dynamic_common (name, t);
}

static void install_programme_common (sexpr name, struct target *t)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
         cons (sym_install, cons (make_integer (0555),
               cons (get_build_file (t, sx_join (name, (i_os == os_windows 
                                                         ? str_dot_exe
                                                         : sx_nil), sx_nil)),
                     get_programme_install_path(t)))));
}

static void install_programme_gcc (sexpr name, struct target *t)
{
    install_programme_common (name, t);
}

static void install_programme_borland (sexpr name, struct target *t)
{
    install_programme_common (name, t);
}

static void install_programme_msvc (sexpr name, struct target *t)
{
    install_programme_common (name, t);
}

static void install_headers_common (sexpr name, struct target *t)
{
    sexpr c = t->headers;

    while (consp (c))
    {
        sexpr c2 = car(c);
        sexpr c3 = car(c2);
        sexpr c4 = car(cdr(c2));

        t->icemake->workstack = sx_set_add (t->icemake->workstack,
             cons (sym_install, cons (c4, get_header_install_path (t, c3))));

        c = cdr (c);
    }
}

static void install_headers_gcc (sexpr name, struct target *t)
{
    install_headers_common (name, t);
}

static void install_headers_borland (sexpr name, struct target *t)
{
    install_headers_common (name, t);
}

static void install_headers_msvc (sexpr name, struct target *t)
{
    install_headers_common (name, t);
}

static void install_support_files (sexpr name, struct target *t)
{
    sexpr cur = t->data, dname;

    if (truep(equalp(name, str_curie)))
    {
        t->icemake->workstack = sx_set_add (t->icemake->workstack,
                           cons (sym_install,
                             cons (get_build_file (t, str_libcuriedsx),
                                get_install_file (t, sx_join (i_destlibdir,
                                                  str_slibcuriedsx, sx_nil)))));
    }

    while (consp (cur))
    {
        sexpr item = car (cur);
        sexpr ccur = cdr (item);

        dname = car (item);

        while (consp(ccur))
        {
            sexpr s = car (ccur);

            t->icemake->workstack = sx_set_add (t->icemake->workstack,
                      cons (sym_install, cons (cdr (s),
                      get_data_install_path (t, dname, car (s)))));

            ccur = cdr (ccur);
        }

        cur = cdr (cur);
    }
}

static void install_library (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_library_gcc     (name, t); break;
        case tc_borland:
            install_library_borland (name, t); break;
        case tc_msvc:
            install_library_msvc    (name, t); break;
    }
}

static void install_headers (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_headers_gcc     (name, t); break;
        case tc_borland:
            install_headers_borland (name, t); break;
        case tc_msvc:
            install_headers_msvc    (name, t); break;
    }
}

static void install_documentation_with_suffix
    (sexpr name, struct target *t, sexpr c4, const char *suffix)
{
    sexpr fn = get_build_file (t, sx_join (c4, str_dot, make_string (suffix)));

    if (truep(filep(fn)))
    {
        t->icemake->workstack = sx_set_add (t->icemake->workstack,
                        cons (sym_install, cons (fn,
                              get_documentation_install_path
                                (t, name, c4, t->dversion, suffix))));
    }
}

static void install_documentation_man
    (sexpr name, struct target *t, sexpr file, sexpr abbr, sexpr section)
{
    t->icemake->workstack = sx_set_add (t->icemake->workstack,
                      cons (sym_install, cons (file,
                            get_documentation_man_install_path
                                (t, abbr, section))));
}

static void install_documentation (sexpr name, struct target *t)
{
    sexpr c = t->documentation;

    while (consp (c))
    {
        sexpr c2 = car(c);
        sexpr c3 = cdr(c2);
        sexpr c4 = car(c3);

        if (truep(equalp(car(c2), sym_man)))
        {
            sexpr c5 = cdr(c3);
            sexpr filename = car(c5);
            sexpr c7 = cdr(c5);
            sexpr section = car(c7);

            install_documentation_man (name, t, filename, c4, section);
        }
        else
        {
            install_documentation_with_suffix (name, t, c4, "pdf");
            install_documentation_with_suffix (name, t, c4, "dvi");
            install_documentation_with_suffix (name, t, c4, "ps");
            install_documentation_with_suffix (name, t, c4, "eps");
            install_documentation_with_suffix (name, t, c4, "html");
        }
        c = cdr (c);
    }
}

static void install_programme (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            install_programme_gcc     (name, t); break;
        case tc_borland:
            install_programme_borland (name, t); break;
        case tc_msvc:
            install_programme_msvc    (name, t); break;
    }
}

static void do_install_target(struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        install_library (t->name, t);
    }
    else if (t->options & ICEMAKE_PROGRAMME)
    {
        install_programme (t->name, t);
    }

    install_headers (t->name, t);
    install_support_files (t->name, t);
    install_documentation (t->name, t);
}

int icemake_install (struct icemake *im)
{
    sexpr cursor = im->buildtargets;

    if (falsep(i_destdir))
    {
        return 0;
    }
    
    sx_write (stdio, cons (sym_phase, cons (sym_install, sx_end_of_list)));

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);
        struct tree_node *node =
            tree_get_node_string (&(im->targets), (char *)target);

        if (node != (struct tree_node *)0)
        {
            do_install_target (node_get_value(node));
        }

        cursor = cdr(cursor);
    }

    return icemake_loop_processes (im);
}
