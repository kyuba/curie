/*
 *  multiplex-io.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 07/08/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <curie/multiplex.h>
#include <curie/memory.h>

static void mx_f_count(int *r, int *w);
static void mx_f_augment(int *rs, int *r, int *ws, int *w);
static void mx_f_callback(int *rs, int r, int *ws, int w);

static struct multiplex_functions mx_functions = {
    .count = mx_f_count,
    .augment = mx_f_augment,
    .callback = mx_f_callback,
    .next = (struct multiplex_functions *)0
};

struct io_list {
    struct io *io;
    void (*on_read)(struct io *, void *);
    void (*on_close)(struct io *, void *);
    /*@temp@*/ void *data;
    /*@only@*/ struct io_list *next;
};

static struct memory_pool list_pool
         = MEMORY_POOL_INITIALISER(sizeof (struct io_list));
/*@only@*/ /*@null@*/ static struct io_list *list = (struct io_list *)0;

/*@-branchstate@*/
static void mx_f_count(int *r, int *w) {
    struct io_list *l = list, **p = &list;

    while (l != (struct io_list *)0) {
        if ((l->io->fd != -1) &&
             (l->io->status != io_end_of_file) &&
             (l->io->status != io_unrecoverable_error))
        {
            switch (l->io->type) {
                case iot_read:
                    (*r) += 1;
                    break;
                case iot_write:
                    if (l->io->length == 0) goto next;

                    (*w) += 1;
                    break;
                default:
                    break;
            }
        } else {
            struct io_list *t = l;

            *p = l->next;
            p = &(l->next);
            l = *p;

            if (t->on_close != (void *)0)
            {
                t->on_close (t->io, t->data);
            }

            io_close (t->io);

            free_pool_mem (t);
            continue;
        }

        next:
        p = &(l->next);
        l = l->next;
    }
}

static void mx_f_augment(int *rs, int *r, int *ws, int *w) {
    struct io_list *l = list, **p = &list;

    while (l != (struct io_list *)0) {
        if ((l->io->fd != -1) &&
            (l->io->status != io_end_of_file) &&
            (l->io->status != io_unrecoverable_error))
        {
            int i, t, fd = l->io->fd;

            switch (l->io->type) {
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
                    if (l->io->length == 0) goto next;

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
        } else {
            struct io_list *t = l;

            *p = l->next;
            p = &(l->next);
            l = *p;

            if (t->on_close != (void *)0)
            {
                t->on_close (t->io, t->data);
            }

            io_close (t->io);

            free_pool_mem (t);
            continue;
        }

        next:
        p = &(l->next);
        l = l->next;
    }
}

static void mx_f_callback(int *rs, int r, int *ws, int w) {
    struct io_list *l = list, **p = &list;

    while (l != (struct io_list *)0) {
        if ((l->io->fd != -1) &&
            (l->io->status != io_end_of_file) &&
            (l->io->status != io_unrecoverable_error))
        {
            int i, fd = l->io->fd;

            switch (l->io->type) {
                case iot_read:
                    for (i = 0; i < r; i++) {
                        if (rs[i] == fd) {
                            (void)io_read (l->io);
                            if (l->on_read != (void *)0)
                                l->on_read (l->io, l->data);
                        }
                    }
                    break;
                case iot_write:
                    if (l->io->length == 0) goto next;

                    for (i = 0; i < w; i++) {
                        if (ws[i] == fd) {
                            (void)io_commit (l->io);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        if ((l->io->fd == -1) ||
            (l->io->status == io_end_of_file) ||
            (l->io->status == io_unrecoverable_error))
        {
            struct io_list *t = l;

            *p = l->next;
            p = &(l->next);
            l = *p;

            if (t->on_close != (void *)0)
            {
                t->on_close (t->io, t->data);
            }

            io_close (t->io);

            free_pool_mem (t);
            continue;
        }

        next:
        p = &(l->next);
        l = l->next;
    }
}
/*@=branchstate@*/

void multiplex_io () {
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_add (&mx_functions);
        installed = (char)1;
    }
}

/*@-nullstate -mustfree@*/
void multiplex_add_io (struct io *io, void (*on_read)(struct io *, void *), void (*on_close)(struct io *, void *), void *data) {
    struct io_list *list_element = get_pool_mem (&list_pool);

    if (list_element == (struct io_list *)0) return;

    list_element->next = (void *)0;
    list_element->io = io;
    list_element->on_read = on_read;
    list_element->on_close = on_close;
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
/*@=nullstate =mustfree@*/

void multiplex_add_io_no_callback (/*@notnull@*/ /*@only@*/ struct io *io)
{
    multiplex_add_io (io, (void *)0, (void *)0, (void *)0);
}

/*@-branchstate*/
void multiplex_del_io (struct io *io) {
    struct io_list *l = list, *p = (struct io_list *)0;

    while (l != (struct io_list *)0) {
        if (l->io == io)
        {
            struct io_list *t = l;

            /*@-mustfree@*/
            if (p == (struct io_list *)0)
            {
                list = l->next;
            }
            else
            {
                p->next = l->next;
            }
            /*@=mustfree@*/
            l = l->next;

            if (t->on_close != (void *)0)
            {
                t->on_close (t->io, t->data);
            }

            free_pool_mem (t);
            continue;
        }

        p = l;
        l = l->next;
    }

    io_close (io);
}
/*@=branchstate*/
