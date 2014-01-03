/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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

#include <curie/multiplex.h>
#include <curie/memory.h>

#include <curie/sexpr-internal.h>

struct io_element {
    struct sexpr_io *io;
    void (*on_read)(sexpr, struct sexpr_io *, void *);
    void *data;
};

void multiplex_sexpr ()
{
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_io();
        installed = (char)1;
    }
}

static void mx_on_read (struct io *r, void *d)
{
    struct io_element *element = (struct io_element *)d;
    sexpr sx = sx_read (element->io);

    while (sx != sx_nonexistent)
    {
        if (element->on_read != (void (*)(sexpr, struct sexpr_io *, void *))0)
        {
            element->on_read (sx, element->io, element->data);
        }

        if (sx == sx_end_of_file)
        {
            return;
        }

        sx = sx_read (element->io);
    }
}

static void mx_on_close (struct io *r, void *d)
{
    struct io_element *element = (struct io_element *)d;
    struct sexpr_io *io = element->io;

    if (element->on_read != (void (*)(sexpr, struct sexpr_io *, void *))0)
    {
        element->on_read (sx_end_of_file, element->io, element->data);
    }

    if ((io->out != (struct io*)0) && (io->in != io->out))
    {
        multiplex_del_io (io->out);
    }

    free_pool_mem (io);
    free_pool_mem (element);
}

static void mx_on_close_out (struct io *r, void *d)
{
    struct io_element *element = (struct io_element *)d;
    struct sexpr_io *io = element->io;

    if (element->on_read != (void (*)(sexpr, struct sexpr_io *, void *))0)
    {
        element->on_read (sx_end_of_file, element->io, element->data);
    }

    if ((io->in != (struct io*)0) && (io->in != io->out))
    {
        multiplex_del_io (io->in);
    }

    free_pool_mem (io);
    free_pool_mem (element);
}

void multiplex_add_sexpr
        (struct sexpr_io *io, void (*on_read)(sexpr, struct sexpr_io *, void *),
         void *data)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct io_element));
    struct io_element *element = get_pool_mem (&pool);

    element->io = io;
    element->on_read = on_read;
    element->data = data;

    if (io->in == (struct io *)0)
    {
        multiplex_add_io (io->out, (void *)0, mx_on_close_out, (void *)element);
    }
    else if (io->out == (struct io *)0)
    {
        multiplex_add_io (io->in, mx_on_read, mx_on_close, (void *)element);
    }
    else if (
#if defined(_WIN32)
        (io->in->handle == (void *)0)
#else
        (io->in->fd == -1)
#endif
        && (io->in->type != iot_special_read)
        && (io->in->type != iot_special_write))
    {
        multiplex_add_io (io->out, (void *)0, mx_on_close_out, (void *)element);
    }
    else
    {
        multiplex_add_io (io->in, mx_on_read, mx_on_close, (void *)element);

        if (io->in != io->out)
        {
            multiplex_add_io_no_callback(io->out);
        }
    }
}

void multiplex_del_sexpr (struct sexpr_io *io)
{
    if (io->in != (struct io *)0)
    {
        io->in->status = io_end_of_file;
    }

    if (io->out != (struct io *)0)
    {
        io->out->status = io_end_of_file;
    }
}

