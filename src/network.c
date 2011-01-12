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

#include <curie/multiplex-system.h>
#include <curie/memory.h>
#include <curie/network.h>
#include <curie/network-system.h>
#include <curie/io-system.h>

struct net_socket_listener
{
    int socket;
    void (*on_connect)(struct io *, struct io *, void *);
    void *data;
    struct net_socket_listener *next;
};

struct mx_sx_payload
{
    void (*on_connect)(struct sexpr_io *, void *);
    void *data;
};

static struct net_socket_listener *list
        = (struct net_socket_listener *)0;

static enum multiplex_result mx_f_count(int *r, int *w);
static void mx_f_augment(int *rs, int *r, int *ws, int *w);
static void mx_f_callback(int *rs, int r, int *ws, int w);

static struct multiplex_functions mx_functions =
{
    .count = mx_f_count,
    .augment = mx_f_augment,
    .callback = mx_f_callback,
    .next = (struct multiplex_functions *)0
};

static enum multiplex_result mx_f_count(int *r, int *w) {
    struct net_socket_listener *l = list;

    while (l != (struct net_socket_listener *)0)
    {
        if (l->socket >= 0)
        {
            (*r) += 1;
        }

        l = l->next;
    }

    return mx_ok;
}

static void mx_f_augment(int *rs, int *r, int *ws, int *w)
{
    struct net_socket_listener *l = list;

    while (l != (struct net_socket_listener *)0)
    {
        if (l->socket >= 0)
        {
            int i, j = (*r);

            for (i = 0; i < j; i++)
            {
                if (rs[i] == l->socket) 
                {
                    goto next;
                }
            }
            rs[i] = l->socket;
            (*r) += 1;
        }

        next:
        l = l->next;
    }
}

static void mx_f_callback(int *rs, int r, int *ws, int w)
{
    struct net_socket_listener *l = list, *p = (struct net_socket_listener *)0;

    while (l != (struct net_socket_listener *)0)
    {
        next:

        if (l->socket >= 0)
        {
            int i;

            for (i = 0; i < r; i++)
            {
                if (rs[i] == l->socket)
                {
                    int fdr, fdw;
                    enum io_result res;
                    if ((res = a_accept_socket (&fdr, l->socket))== io_complete)
                    {
                        fdw = a_dup_n (fdr);
                        if (fdw >= 0)
                        {
                            struct io *in, *out;

                            in = io_open (fdr);
                            out = io_open (fdw);

                            in->type = iot_read;
                            out->type = iot_write;

                            l->on_connect(in, out, l->data);
                        }
                        else
                        {
                            (void)a_close (fdr);
                        }
                    }
                    else if (res == io_unrecoverable_error)
                    {
                        (void)a_close (rs[i]);
                        if (p == (struct net_socket_listener *)0)
                        {
                            list = l->next;
                            free_pool_mem (l);
                            l = list->next;
                        }
                        else
                        {
                            p->next = l->next;
                            free_pool_mem (l);
                            l = p->next;
                        }

                        if (l != (struct net_socket_listener *)0)
                        {
                            goto next;
                        }
                        else
                        {
                            return;
                        }
                    }
                }
            }
        }

        p = l;
        l = l->next;
    }
}

void net_open_loop (struct io **in, struct io **out)
{
    struct io *iin, *iout;
    int fds[2];

    if (a_open_loop (fds) == io_unrecoverable_error)
    {
        fds[0] = -1;
        fds[1] = -1;
    }

    iin = io_open (fds[0]);
    iout = io_open (fds[1]);

    iin->type = iot_read;
    iout->type = iot_write;

    (*in) = iin;
    (*out) = iout;
}

static void net_open_tail (int fdr, struct io **in, struct io **out)
{
    int fdw;
    struct io *iin, *iout;

    if ((fdr == -1) || !(fdw = a_dup_n (fdr), fdw >= 0))
    {
        (void)a_close (fdr);
        fdr = -1;
        fdw = -1;
    }

    iin = io_open (fdr);
    iout = io_open (fdw);

    iin->type = iot_read;
    iout->type = iot_write;

    (*in) = iin;
    (*out) = iout;
}

void net_open_socket (const char *path, struct io **in, struct io **out)
{
    int fd;

    if (a_open_socket (&fd, path) != io_complete)
    {
        fd = -1;
    }

    net_open_tail (fd, in, out);
}

void net_open_ip4 (int_32 addr, int_16 port, struct io **in, struct io **out)
{
    int fd;

    if (a_open_ip4 (&fd, addr, port) != io_complete)
    {
        fd = -1;
    }

    net_open_tail (fd, in, out);
}

void net_open_ip6 (int_8 addr[16], int_16 port, struct io **in, struct io **out)
{
    int fd;

    if (a_open_ip6 (&fd, addr, port) != io_complete)
    {
        fd = -1;
    }

    net_open_tail (fd, in, out);
}

void multiplex_network() {
    static char installed = (char)0;

    if (installed == (char)0) {
        multiplex_add (&mx_functions);
        installed = (char)1;
    }
}

static void multiplex_add_tail
        (int fd, void (*on_connect)(struct io *, struct io *, void *),
         void *aux)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct net_socket_listener));
    struct net_socket_listener *l = get_pool_mem (&pool);

    l->socket = fd;
    l->on_connect = on_connect;
    l->data = aux;

    l->next = list;

    list = l;
}

void multiplex_add_socket
        (const char *path, void (*on_connect)(struct io *, struct io *, void *),
         void *aux)
{
    int fd;

    if (a_open_listen_socket (&fd, path) == io_complete)
    {
        multiplex_add_tail (fd, on_connect, aux);
    }
}

void multiplex_add_ip4
        (int_32 addr, int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux)
{
    int fd;

    if (a_open_listen_ip4 (&fd, addr, port) == io_complete)
    {
        multiplex_add_tail (fd, on_connect, aux);
    }
}

void multiplex_add_ip6
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux)
{
    int fd;

    if (a_open_listen_ip6 (&fd, addr, port) == io_complete)
    {
        multiplex_add_tail (fd, on_connect, aux);
    }
}

static void mx_sx_on_connect
        (struct io *in, struct io *out, void *d)
{
    struct mx_sx_payload *p = (struct mx_sx_payload *)d;
    struct sexpr_io *io = sx_open_io (in, out);

    if (io != (struct sexpr_io *)0)
    {
        p->on_connect(io, p->data);
    }
}

void multiplex_add_socket_sx
        (const char *path, void (*on_connect)(struct sexpr_io *, void *),
         void *aux)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct mx_sx_payload));
    struct mx_sx_payload *d = (struct mx_sx_payload *)get_pool_mem (&pool);

    if (d != (struct mx_sx_payload *)0)
    {
        d->on_connect = on_connect;
        d->data = aux;

        multiplex_add_socket (path, mx_sx_on_connect, (void *)d);
    }
}

void multiplex_add_ip4_sx
        (int_32 addr, int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct mx_sx_payload));
    struct mx_sx_payload *d = (struct mx_sx_payload *)get_pool_mem (&pool);

    if (d != (struct mx_sx_payload *)0)
    {
        d->on_connect = on_connect;
        d->data = aux;

        multiplex_add_ip4 (addr, port, mx_sx_on_connect, (void *)d);
    }
}

void multiplex_add_ip6_sx
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct mx_sx_payload));
    struct mx_sx_payload *d = (struct mx_sx_payload *)get_pool_mem (&pool);

    if (d != (struct mx_sx_payload *)0)
    {
        d->on_connect = on_connect;
        d->data = aux;

        multiplex_add_ip6 (addr, port, mx_sx_on_connect, (void *)d);
    }
}

static struct sexpr_io *sx_open_tail (struct io *in, struct io *out)
{
    return sx_open_io (in, out);
}

struct sexpr_io *sx_open_socket (const char *path)
{
    struct io *in, *out;

    net_open_socket (path, &in, &out);

    return sx_open_tail (in, out);
}

struct sexpr_io *sx_open_ip4 (int_32 addr, int_16 port)
{
    struct io *in, *out;

    net_open_ip4 (addr, port, &in, &out);

    return sx_open_tail (in, out);
}

struct sexpr_io *sx_open_ip6 (int_8 addr[16], int_16 port)
{
    struct io *in, *out;

    net_open_ip6 (addr, port, &in, &out);

    return sx_open_tail (in, out);
}

void multiplex_add_socket_client_sx
        (const char *path, void (*on_read)(sexpr, struct sexpr_io *, void *),
         void *aux)
{
    struct sexpr_io *io = sx_open_socket(path);
    if (io != (struct sexpr_io *)0)
    {
        multiplex_add_sexpr (io, on_read, aux);
    }
}

void multiplex_add_ip4_client_sx
        (int_32 addr, int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux)
{
    struct sexpr_io *io = sx_open_ip4(addr, port);
    if (io != (struct sexpr_io *)0)
    {
        multiplex_add_sexpr (io, on_read, aux);
    }
}

void multiplex_add_ip6_client_sx
        (int_8 addr[16], int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux)
{
    struct sexpr_io *io = sx_open_ip6(addr, port);
    if (io != (struct sexpr_io *)0)
    {
        multiplex_add_sexpr (io, on_read, aux);
    }
}
