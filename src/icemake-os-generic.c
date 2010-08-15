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
static int install_file
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

static int install_file
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

static sexpr initialise_libcurie_filename
    (struct toolchain_descriptor *td, struct icemake *im, sexpr f)
{
    struct sexpr_io *io;
    sexpr r;
    const char *filename = sx_string (f);

    if (falsep (filep (f))) return sx_false;

    io = sx_open_i (io_open_read(filename));

    while (!eofp(r = sx_read (io)))
    {
        if (truep(equalp(r, sym_freestanding)))
        {
            td->options |= ICEMAKE_OPTION_FREESTANDING;
        }
        else if (truep(equalp(r, sym_hosted)))
        {
            td->options &= ~ICEMAKE_OPTION_FREESTANDING;

            if (td->operating_system != os_windows)
            {
                im->options &= ~ICEMAKE_OPTION_DYNAMIC_LINKING;
            }
        }
    }

    sx_close_io (io);

    return sx_true;
}

static void initialise_libcurie
    (struct icemake *im, struct toolchain_descriptor *td)
{
    if (!falsep(i_destdir))
    {
        if (truep (initialise_libcurie_filename
                       (td, im, sx_join (i_destdir, str_slash,
                          sx_join (i_destlibdir, str_slibcuriedsx, sx_nil)))))
        {
            return;
        }

        switch (im->filesystem_layout)
        {
            case fs_fhs:
                if (truep (initialise_libcurie_filename
                               (td, im, sx_join (i_destdir, str_susrs,
                                  sx_join (i_destlibdir, str_slibcuriedsx,
                                           sx_nil)))))
                {
                    return;
                }
                break;
            case fs_afsl:
                if (truep (initialise_libcurie_filename
                     (td, im, sx_join (i_destdir, str_slash,
                        sx_join (make_string (td->uname_os),
                          str_slash,
                          sx_join (make_string (td->uname_arch),
                            str_slibslibcuriedsx, sx_nil))))))
                {
                    return;
                }
        }
    }

    if (truep (initialise_libcurie_filename
                   (td, im,
                    sx_join (str_slash, i_destlibdir, str_slibcuriedsx))))
    {
        return;
    }

    switch (im->filesystem_layout)
    {
        case fs_fhs:
            if (truep (initialise_libcurie_filename
                 (td, im, sx_join (str_susrs, i_destlibdir, str_slibcuriedsx))))
            {
                return;
            }
            if (truep (initialise_libcurie_filename
                 (td, im, sx_join (str_slash, i_destlibdir, str_slibcuriedsx))))
            {
                return;
            }
            break;
        case fs_afsl:
            if (truep (initialise_libcurie_filename
                 (td, im, sx_join (str_slash, make_string (td->uname_os),
                    sx_join (str_slash, make_string (td->uname_arch),
                        str_slibslibcuriedsx)))))
            {
                return;
            }
    }
}

sexpr icemake_decorate_file
    (struct target *t, enum file_type ft, enum file_expansion_type fet,
     sexpr file)
{
    define_string (str_share, "share");
    char empty = 0;

    sexpr g;

    switch (t->toolchain->operating_system)
    {
        case os_windows: g = str_backslash; break;
        default:         g = str_slash;     break;
    }

    if (!stringp (file))
    {
        file  = str_blank;
        empty = 1;
    }
    else switch (ft)
    {
        case ft_programme:
            switch (t->toolchain->operating_system)
            {
                case os_windows:
                    file = sx_join (file, str_dot_exe, sx_nil);
                    break;
                default:
                    break;
            }
            break;
        case ft_static_library:
            switch (t->toolchain->toolchain)
            {
                case tc_msvc:
                case tc_borland:
                    file = sx_join (str_lib, file, str_dot_lib);
                    break;
                default:
                    file = sx_join (str_lib, file, str_dot_a);
                    break;
            }
            break;
        case ft_shared_library:
            switch (t->toolchain->operating_system)
            {
                case os_windows:
                    file = sx_join (str_lib, file, str_dot_dll);
                    break;
                default:
                    file = sx_join (str_lib, file, str_dot_so);
                    break;
            }
        case ft_shared_library_full:
            switch (t->toolchain->operating_system)
            {
                case os_windows:
                    file = sx_join (str_lib, file,
                           sx_join (str_dot, t->dversion, str_dot_dll));
                    break;
                default:
                    file = sx_join (str_lib, file,
                           sx_join (str_dot_so_dot, t->dversion, sx_nil));
                    break;
            }
        case ft_library_options:
            file = sx_join (str_lib, file, str_dot_sx);
        case ft_header:
            file = sx_join (t->name, g, sx_join (file, str_dot_h, sx_nil));
            break;
        case ft_object:
            switch (t->toolchain->toolchain)
            {
                case tc_borland:
                case tc_msvc:
                    file = sx_join (file, str_dot_obj, sx_nil);
                    break;
                default:
                    file = sx_join (file, str_dot_o, sx_nil);
                    break;
            }
            break;
        case ft_object_pic:
            switch (t->toolchain->toolchain)
            {
                case tc_borland:
                case tc_msvc:
                    file = sx_join (file, str_dot_obj, sx_nil);
                    break;
                default:
                    file = sx_join (file, str_dot_pic_o, sx_nil);
                    break;
            }
            break;
        case ft_resource:
            file = sx_join (file, str_dot_res, sx_nil);
            break;
        default:
            break;
    }

    switch (fet)
    {
        case fet_build_file:
            file = sx_join (t->buildbase, t->toolchain->uname,
                   sx_join (g, t->name,
                            (empty ? sx_nil : sx_join (g, file, sx_nil))));

            if (t->toolchain->toolchain == tc_borland)
            {
                file = mangle_path_borland_sx (file);
            }

            break;

        case fet_install_file:
            switch (ft)
            {
                case ft_programme:
                    file = sx_join (str_bin, g, file);
                    break;
                case ft_static_library:
                case ft_library_options:
                    file = sx_join (i_destlibdir, g, file);
                    break;
                case ft_shared_library:
                case ft_shared_library_full:
                    switch (t->toolchain->operating_system)
                    {
                        case os_windows:
                            file = sx_join (str_bin, g, file);
                            break;
                        default:
                            file = sx_join (i_destlibdir, g, file);
                            break;
                    }
                    break;
                case ft_header:
                    file = sx_join (str_include, g, file);
                    break;
                default:
                    file = sx_join (str_share, g, sx_join (t->name, g, file));
                    break;
            }

            switch (t->icemake->filesystem_layout)
            {
                case fs_fhs:
                     file = sx_join (i_destdir, g, file);
                     break;
                case fs_afsl:
                     file = sx_join (i_destdir, g,
                            sx_join (make_string (t->toolchain->uname_os), g,
                            sx_join (make_string (t->toolchain->uname_arch), g,
                                     file)));
                     break;
            }
            break;
        default:
            break;
    }

    return file;
}

int icemake_prepare_operating_system_generic
    (struct icemake *im, struct toolchain_descriptor *td)
{
/*    if (td != (struct toolchain_descriptor *)0)
    {
        td->meta_operating_system.generic.diff
            = icemake_which (td, "diff", "DIFF");
    }*/

    if (im != (struct icemake *)0)
    {
        if (im->install_file == (int (*)(struct icemake *, sexpr))0)
        {
            im->install_file = install_file;
        }

        initialise_libcurie (im, td);
    }

    return 0;
}
