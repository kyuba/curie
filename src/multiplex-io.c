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

#include <curie/multiplex-system.h>
#include <curie/memory.h>

static enum multiplex_result mx_f_count(int *r, int *w);
static void mx_f_augment(int *rs, int *r, int *ws, int *w);
static void mx_f_callback(int *rs, int r, int *ws, int w);

static struct multiplex_functions mx_functions = {
    mx_f_count,
    mx_f_augment,
    mx_f_callback,
    (struct multiplex_functions *)0
};

enum io_list_status
{
    ils_nominal = 0x0,
    ils_active  = 0x1,
    ils_kill    = 0x2
};

struct io_list {
    struct io *io;
    void (*on_read)(struct io *, void *);
    void (*on_close)(struct io *, void *);
    void *data;
    enum io_list_status status;
    struct io_list *next;
};

static struct io_list *list = (struct io_list *)0;

static enum multiplex_result mx_f_count(int *r, int *w) {
    struct io_list *l = list;

    while (l != (struct io_list *)0) {
        struct io *io = l->io;

        if ((io->fd == -1))
        {
            switch (io->type) {
                case iot_special_read:
                case iot_special_write:
                    if (io->status == io_changes)
                    {
                        return mx_immediate_action;
                    }
                    break;
                default:
                    break;
            }
        } else if ((io->status != io_end_of_file) &&
                   (io->status != io_unrecoverable_error))
        {
            switch (io->type) {
                case iot_read:
                    (*r) += 1;
                    break;
                case iot_write:
                    if (io->length == 0) goto next;

                    (*w) += 1;
                    break;
                default:
                    break;
            }
        }

        next:
        l = l->next;
    }

    return mx_ok;
}

static void mx_f_augment(int *rs, int *r, int *ws, int *w) {
    struct io_list *l = list;

    while (l != (struct io_list *)0) {
        struct io *io = l->io;

        if ((io->fd != -1) &&
            (io->status != io_end_of_file) &&
            (io->status != io_unrecoverable_error))
        {
            int i, t, fd = io->fd;

            switch (io->type) {
                case iot_read:
                    t = *r;
                    for (i = 0; i < t; i++) {
                        if (rs[i] == fd) {
                            goto next;
                        }
                    }
                    rs[i] = fd;
                    (*r) += 1;
                    break;
                case iot_write:
                    if (io->length == 0) goto next;

                    t = *w;
                    for (i = 0; i < t; i++) {
                        if (ws[i] == fd) {
                            goto next;
                        }
                    }
                    ws[i] = fd;
                    (*w) += 1;
                    break;
                default:
                    break;
            }
        }

        next:
        l = l->next;
    }
}

static void mx_f_callback(int *rs, int r, int *ws, int w) {
    struct io_list *l = list;
    char changes = 1;

    while (l != (struct io_list *)0) {
        struct io *io = l->io;

        if (io->status & ils_active)
        {
            goto next;
        }

        if (io->fd == -1)
        {
            switch (io->type) {
                case iot_special_read:
                case iot_special_write:
                    if (l->on_read != (void *)0)
                    {
                        if (io_read(io) == io_changes)
                        {
                            io->status |= ils_active;
                            l->on_read (io, l->data);
                            io->status &= ~ils_active;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        else if ((io->status != io_end_of_file) &&
                 (io->status != io_unrecoverable_error))
        {
            int i, fd = io->fd;

            switch (io->type) {
                case iot_read:
                    for (i = 0; i < r; i++) {
                        if (rs[i] == fd) {
                            (void)io_read (io);
                            if (l->on_read != (void *)0)
                            {
                                io->status |= ils_active;
                                l->on_read (io, l->data);
                                io->status &= ~ils_active;
                            }
                        }
                    }
                    break;
                case iot_write:
                    if (io->length == 0) goto next;

                    for (i = 0; i < w; i++) {
                        if (ws[i] == fd) {
                            (void)io_commit (io);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        next:
        l = l->next;

        if (io->status == ils_kill)
        {
            multiplex_del_io (io);
        }
    }

    while (changes)
    {
        changes = 0;
        l = list;

        while (l != (struct io_list *)0) {
            struct io *io = l->io;

            if (!(io->status & ils_active) &&
                (io->fd == -1))
            {
                switch (io->type) {
                    case iot_special_read:
                    case iot_special_write:
                        if (l->on_read != (void *)0)
                        {
                            if (io_read(io) == io_changes)
                            {
                                io->status |= ils_active;
                                l->on_read (io, l->data);
                                changes = 1;
                                io->status &= ~ils_active;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            l = l->next;
            
            if (io->status == ils_kill)
            {
                multiplex_del_io (io);
            }
        }
    }

    retry:
    l = list;

    while (l != (struct io_list *)0) {
        struct io *io = l->io;

        if (((io->fd == -1) &&
             (io->type != iot_special_read) &&
             (io->type != iot_special_write)) ||
            (io->status == io_end_of_file) ||
            (io->status == io_unrecoverable_error))
        {
            multiplex_del_io (io);
            goto retry;
        }

        l = l->next;
    }
}

void multiplex_io () {
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_add (&mx_functions);
        installed = (char)1;
    }
}

void multiplex_add_io (struct io *io, void (*on_read)(struct io *, void *), void (*on_close)(struct io *, void *), void *data) {
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct io_list));

    struct io_list *list_element = get_pool_mem (&pool);

    list_element->next = (void *)0;
    list_element->io = io;
    list_element->on_read = on_read;
    list_element->on_close = on_close;
    list_element->status = ils_nominal;
    list_element->data = data;

    if (list == (void *)0)
    {
        list = list_element;
    }
    else
    {
        struct io_list *cx = list;

        while ((cx->next) != (struct io_list *)0)
        {
            cx = cx->next;
        }

        cx->next = list_element;
    }
}

void multiplex_add_io_no_callback (struct io *io)
{
    multiplex_add_io (io, (void *)0, (void *)0, (void *)0);
}

void multiplex_del_io (struct io *io)
{
    struct io_list *l = list, **p;
    char av = (char)0;

    if (io->status & ils_active)
    {
        io->status |= ils_kill;
        return;
    }

    while (l != (struct io_list *)0) {
        if (l->io == io)
        {
            if (l->on_close != (void (*)(struct io *, void *))0)
            {
                void (*f)(struct io *, void *) = l->on_close;
                l->on_close = (void (*)(struct io *, void *))0;
                f (l->io, l->data);
            }
        }

        l = l->next;
    }

    l = list;
    p = &list;

    while (l != (struct io_list *)0) {
        if (l->io == io)
        {
            struct io_list *t = l;

            *p = l->next;
            p = &(l->next);
            l = *p;

            free_pool_mem (t);

            av = (char)1;
            continue;
        }

        p = &(l->next);
        l = l->next;
    }

    if (av == (char)1)
    {
        io_close (io);
    }
}
