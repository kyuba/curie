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

#include <curie/multiplex.h>
#include <curie/memory.h>

#include <curie/sexpr-internal.h>

struct io_element {
    struct sexpr_io *io;
    void (*on_read)(sexpr, struct sexpr_io *, void *);
    /*@temp@*/ void *data;
};

static struct memory_pool list_pool = MEMORY_POOL_INITIALISER(sizeof (struct io_element));

void multiplex_sexpr ()
{
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_io();
        installed = (char)1;
    }
}

static void mx_on_read (/*@unused@*/ struct io *r, void *d)
{
    struct io_element *element = (struct io_element *)d;
    sexpr sx = sx_read (element->io);

    while (sx != sx_nonexistent) {
        element->on_read (sx, element->io, element->data);
        sx = sx_read (element->io);
    }
}

static void mx_on_close (/*@unused@*/ struct io *r, /*@only@*/ void *d)
{
    struct io_element *element = (struct io_element *)d;
    struct sexpr_io *io = element->io;

    if (io->in != io->out)
    {
        multiplex_del_io (io->out);
    }
    free_pool_mem (io);
    free_pool_mem (element);
}

/*@-mustfree@*/
void multiplex_add_sexpr (struct sexpr_io *io, void (*on_read)(sexpr, struct sexpr_io *, void *), void *data)
{
    struct io_element *element = get_pool_mem (&list_pool);

    if (element == (struct io_element *)0) return;

    element->io = io;
    element->on_read = on_read;
    element->data = data;

    multiplex_add_io (io->in, mx_on_read, mx_on_close, (void *)element);
    if (io->in != io->out)
    {
        multiplex_add_io_no_callback(io->out);
    }
}
/*@=mustfree@*/
