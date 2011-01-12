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

#include <curie/main.h>
#include <curie/sexpr.h>
#include <curie/memory.h>
#include <curie/multiplex.h>
#include <sievert/io.h>
#include <sievert/cpio.h>

define_string (str_trailer,          "TRAILER!!!");
define_string (str_test_data,        "test-data/");
define_string (str_test_data_file_1, "test-data/file-1");
define_string (str_test_data_file_2, "test-data/file-2");

define_symbol (sym_hello,            "hello");
define_symbol (sym_world,            "world");

static struct sexpr_io *stdio;
static int num_files = 0;

static void on_new_file
    (struct io *io, const char *name, struct metadata *metadata, void *aux)
{
    sexpr n = make_string (name);
    struct sexpr_io *sxio = sx_open_i (io);

    if (truep (equalp (n, str_trailer)))
    {
        cexit (2);
    }

    if (truep (equalp (n, str_test_data)) &&
        falsep (equalp (sx_end_of_file, sx_read(sxio))))
    {
        cexit (3);
    }

    if (truep (equalp (n, str_test_data_file_1)) &&
        falsep (equalp (sym_hello, sx_read(sxio))))
    {
        cexit (4);
    }

    if (truep (equalp (n, str_test_data_file_2)) &&
        falsep (equalp (sym_world, sx_read(sxio))))
    {
        cexit (5);
    }

    num_files++;

    sx_close_io (sxio);
}

static void on_end_of_archive (void *aux)
{
    num_files++;
}

static void on_archive_buffered
    (void *data, unsigned int size, void *aux)
{
    struct io *f = io_open_buffer (data, size);

    cpio_read_archive (f, ".*", on_new_file, on_end_of_archive, (void *)0);

    f = io_open_buffer (data, size);

    cpio_read_archive (f, ".*1", on_new_file, on_end_of_archive, (void *)0);

    free_mem (size, data);
}

int cmain()
{
    struct io *out = io_open_write ("build/test-archive.cpio");
    struct io *s;
    struct cpio *cpio;

    stdio = sx_open_stdio();

    multiplex_io ();
    multiplex_cpio ();
    multiplex_sexpr ();

    io_get_file_contents
        ("tests/data/test-data.cpio", on_archive_buffered, (void *)0);

    while (multiplex() != mx_nothing_to_do);

    cpio = cpio_create_archive (out);

    s = io_open_special ();

    cpio_next_file (cpio, "file-1", (struct metadata *)0, s);

    io_write (s, "whee\n", 5);
    io_write (s, "whoo", 4);

    cpio_next_file (cpio, "file-2", (struct metadata *)0,
                    io_open_buffer ("boing", 5));

    cpio_close (cpio);

    return (num_files == 6) ? 0 : 1;
}
