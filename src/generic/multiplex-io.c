/*
 *  multiplex-io.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 07/08/2008.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
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

static enum multiplex_result mx_f_count(int *r, int *w);
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

        if (io->fd == -1)
        {
            switch (io->type) {
                case iot_special_read:
                case iot_special_write:
                    if (l->on_read != (void *)0)
                    {
                        if (io_read(io) == io_changes)
                        {
                            l->on_read (io, l->data);
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
                                l->on_read (io, l->data);
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
    }

    while (changes)
    {
        changes = 0;
        l = list;

        while (l != (struct io_list *)0) {
            struct io *io = l->io;

            if (io->fd == -1)
            {
                switch (io->type) {
                    case iot_special_read:
                    case iot_special_write:
                        if (l->on_read != (void *)0)
                        {
                            if (io_read(io) == io_changes)
                            {
                                l->on_read (io, l->data);
                                changes = 1;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            l = l->next;
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
    struct io_list *l = list, **p;

    while (l != (struct io_list *)0) {
        if (l->io == io)
        {
            if (l->on_close != (void *)0)
            {
                l->on_close (l->io, l->data);
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
            continue;
        }

        p = &(l->next);
        l = l->next;
    }

    io_close (io);
}
/*@=branchstate*/
