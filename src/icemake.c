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

#include <curie/main.h>
#include <curie/memory.h>
#include <curie/filesystem.h>
#include <curie/multiplex.h>
#include <icemake/icemake.h>

define_string (str_icemake_sx, "icemake.sx");

sexpr workstack = sx_end_of_list;

static void on_icemake_sx_read (sexpr sx, struct sexpr_io *io, void *p)
{
}

int cmain ()
{
    terminate_on_allocation_errors();
    initialise_targets();

    multiplex_sexpr ();

    if (truep (filep (str_icemake_sx)))
    {
        struct sexpr_io *io = sx_open_io (io_open_read ("icemake.sx"),
                                          io_open_null);

        multiplex_add_sexpr (io, on_icemake_sx_read, (void *)0);
    }

    while (multiplex() != mx_nothing_to_do);

    return 0;
}
