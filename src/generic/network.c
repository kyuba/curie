/*
 *  network.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 06/08/2008.
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
#include <curie/network.h>
#include <curie/network-system.h>
#include <curie/io-system.h>

struct net_socket_listener
{
    int socket;
    /*@notnull@*/ void (*on_connect)(struct io *, struct io *, void *);
    /*@shared@*/ void *data;
    /*@null@*/ /*@only@*/ struct net_socket_listener *next;
};

struct mx_sx_payload {
    /*@notnull@*/ void (*on_connect)(struct sexpr_io *, void *);
    /*@shared@*/ void *data;
};

static struct memory_pool list_pool
        = MEMORY_POOL_INITIALISER(sizeof (struct net_socket_listener));
/*@null@*/ static struct net_socket_listener *list
        = (struct net_socket_listener *)0;
static struct memory_pool mx_sx_payload_pool
        = MEMORY_POOL_INITIALISER(sizeof (struct mx_sx_payload));

static void mx_f_count(int *r, int *w);
static void mx_f_augment(int *rs, int *r, int *ws, int *w);
static void mx_f_callback(int *rs, int r, int *ws, int w);

static struct multiplex_functions mx_functions = {
    .count = mx_f_count,
    .augment = mx_f_augment,
    .callback = mx_f_callback,
    .next = (struct multiplex_functions *)0
};

static void mx_f_count(int *r, /*@unused@*/ int *w) {
    struct net_socket_listener *l = list;

    while (l != (struct net_socket_listener *)0) {
        if (l->socket >= 0) {
            (*r) += 1;
        }

        l = l->next;
    }
}

static void mx_f_augment(int *rs, int *r,
                         /*@unused@*/ int *ws, /*@unused@*/ int *w)
{
    struct net_socket_listener *l = list;

    while (l != (struct net_socket_listener *)0) {
        if (l->socket >= 0) {
            int i, j = (*r);

            for (i = 0; i < j; i++) {
                if (rs[i] == l->socket) goto next;
            }
            rs[i] = l->socket;
            (*r) += 1;
        }

        next:
        l = l->next;
    }
}

/*@-branchstate -globstate -memtrans@*/
static void mx_f_callback(int *rs, int r,
                          /*@unused@*/ int *ws, /*@unused@*/ int w)
{
    struct net_socket_listener *l = list, *p = (struct net_socket_listener *)0;

    while (l != (struct net_socket_listener *)0) {
        next:

        if (l->socket >= 0) {
            int i;

            for (i = 0; i < r; i++) {
                if (rs[i] == l->socket) {
                    int fdr, fdw;
                    enum io_result res;
                    if ((res = a_accept_socket (&fdr, l->socket)) == io_complete)
                    {
                        fdw = a_dup_n (fdr);
                        if (fdw >= 0) {
                            /*@-mustfree@*/
                            struct io *in, *out;

                            in = io_open (fdr);
                            if (in == (struct io *)0) continue;
                            out = io_open (fdw);
                            if (out == (struct io *)0) {
                                io_close (in);
                                continue;
                            }

                            in->type = iot_read;
                            out->type = iot_write;

                            l->on_connect(in, out, l->data);
                            /*@=mustfree@*/
                        } else {
                            (void)a_close (fdr);
                        }
                    } else if (res == io_unrecoverable_error) {
                        (void)a_close (rs[i]);
                        /*@-nullderef -nullpass@*/
                        if (p == (struct net_socket_listener *)0) {
                            /*@-mustfree@*/
                            list = l->next;
                            /*@=mustfree@*/
                            free_pool_mem (l);
                            l = list->next;
                        } else {
                            /*@-mustfree@*/
                            p->next = l->next;
                            /*@=mustfree@*/
                            free_pool_mem (l);
                            l = p->next;
                        }
                        /*@=nullderef =nullpass@*/

                        if (l != (struct net_socket_listener *)0)
                            goto next;
                        else
                            return;
                    }
                }
            }
        }

        p = l;
        /*@-nullderef@*/
        l = l->next;
        /*@=nullderef@*/
    }
}
/*@=branchstate =globstate =memtrans@*/

void net_open_loop (struct io **in, struct io **out) {
    struct io *iin, *iout;
    int fds[2];

    if (a_open_loop (fds) == io_unrecoverable_error) {
        fds[0] = -1;
        fds[1] = -1;
    }

    iin = io_open (fds[0]);
    if (iin == (struct io *)0) {
        (*in) = (struct io *)0;
        (*out) = (struct io *)0;
        return;
    }

    iout = io_open (fds[1]);
    if (iout == (struct io *)0) {
        io_close (iin);
        (*in) = (struct io *)0;
        (*out) = (struct io *)0;
        return;
    }

    iin->type = iot_read;
    iout->type = iot_write;

    (*in) = iin;
    (*out) = iout;
}

void net_open_socket (const char *path, struct io **in, struct io **out) {
    int fdr, fdw;
    struct io *iin, *iout;

    if ((a_open_socket(&fdr, path) == io_complete) &&
        (fdw = a_dup_n (fdr), fdw >= 0)) {
    } else {
        (void)a_close (fdr);
        fdr = -1;
        fdw = -1;
    }

    iin = io_open (fdr);
    if (iin == (struct io *)0) {
        (*in) = (struct io *)0;
        (*out) = (struct io *)0;
        return;
    }

    iout = io_open (fdw);
    if (iout == (struct io *)0) {
        io_close (iin);
        (*in) = (struct io *)0;
        (*out) = (struct io *)0;
        return;
    }

    iin->type = iot_read;
    iout->type = iot_write;

    (*in) = iin;
    (*out) = iout;
}

void multiplex_network() {
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_add (&mx_functions);
        installed = (char)1;
    }
}

/*@-memtrans -branchstate@*/
void multiplex_add_socket (const char *path, void (*on_connect)(struct io *, struct io *, void *), void *data) {
    int fd;

    if (a_open_listen_socket (&fd, path) == io_complete) {
        struct net_socket_listener *l = get_pool_mem (&list_pool);

        if (l == (struct net_socket_listener *)0) return;

        l->socket = fd;
        l->on_connect = on_connect;
        l->data = data;

        /*@-mustfree@*/
        l->next = list;
        /*@=mustfree@*/

        list = l;
    }
}
/*@=memtrans =branchstate@*/

static void mx_sx_on_connect
        (/*@only@*/ struct io *in,
         /*@only@*/ struct io *out,
         void *d)
{
    struct mx_sx_payload *p = (struct mx_sx_payload *)d;
    struct sexpr_io *io = sx_open_io (in, out);

    if (io != (struct sexpr_io *)0)
    {
        /*@-mustfree@*/
        p->on_connect(io, p->data);
        /*@=mustfree@*/
    }
}

/*@-memtrans -branchstate@*/
void multiplex_add_socket_listener_sx (const char *path, void (*on_connect)(struct sexpr_io *, void *), void *data) {
    struct mx_sx_payload *d = (struct mx_sx_payload *)get_pool_mem (&mx_sx_payload_pool);

    if (d != (struct mx_sx_payload *)0)
    {
        d->on_connect = on_connect;
        d->data = data;

        /*@-mustfree@*/
        multiplex_add_socket (path, mx_sx_on_connect, (void *)d);
        /*@=mustfree@*/
    }
}
/*@=memtrans =branchstate@*/

struct sexpr_io *sx_open_socket (const char *path) {
    struct io *in, *out;

    net_open_socket (path, &in, &out);
    if ((in == (struct io *)0) || (out == (struct io *)0))
    {
        if (in != (struct io *)0) io_close (in);
        if (out != (struct io *)0) io_close (out);

        return (struct sexpr_io *)0;
    }

    return sx_open_io (in, out);
}

void multiplex_add_socket_sx (const char *path, void (*on_read)(struct sexpr *, struct sexpr_io *, void *), void *d) {
    struct sexpr_io *io = sx_open_socket(path);
    if (io != (struct sexpr_io *)0)
        multiplex_add_sexpr (io, on_read, d);
}
