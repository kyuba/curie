/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2013, Kyuba Project Members
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
            im->options &= ~ICEMAKE_OPTION_DYNAMIC_LINKING;
        }
    }

    sx_close_io (io);

    return sx_true;
}

static void initialise_libcurie
    (struct icemake *im, struct toolchain_descriptor *td)
{
    if (truep (initialise_libcurie_filename
         (td, im, sx_join (str_slash, make_string (td->uname_os),
            sx_join (str_slash, make_string (td->uname_arch),
                str_slibslibcuriedsx)))))
    {
        return;
    }
}

int icemake_prepare_operating_system_generic
    (struct icemake *im, struct toolchain_descriptor *td)
{
    if (im != (struct icemake *)0)
    {
        initialise_libcurie (im, td);
    }

    return 0;
}
