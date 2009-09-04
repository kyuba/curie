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

#include <curie/multiplex-system.h>
#include <curie/memory.h>
#include <curie/network.h>
#include <curie/network-system.h>
#include <curie/io-system.h>
#include <windows.h>

void net_open_loop (struct io **in, struct io **out) {
    struct io *iin, *iout;
    void *ihandle, *ohandle;
    SECURITY_ATTRIBUTES s;
    
    s.nLength              = sizeof (s);
    s.lpSecurityDescriptor = (void *)0;
    s.bInheritHandle       = TRUE;

    if (CreatePipe (&ihandle, &ohandle, &s, 0x1000) == FALSE)
    {
        ihandle = (void *)0;
        ohandle = (void *)0;
    }


    iin = io_open (ihandle);
    if (iin == (struct io *)0) {
        (*in) = (struct io *)0;
        (*out) = (struct io *)0;
        return;
    }

    iout = io_open (ohandle);
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

void net_open_socket
        (const char *path, struct io **in, struct io **out)
{
}

void net_open_ip4 (int_32 addr, int_16 port, struct io **in, struct io **out)
{
}

void net_open_ip6 (int_8 addr[16], int_16 port, struct io **in, struct io **out)
{
}

void multiplex_network
        ()
{
}

void multiplex_add_socket
        (const char *path, void (*on_connect)(struct io *, struct io *, void *), void *data)
{
}

void multiplex_add_ip4
        (int_32 addr, int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux)
{
}

void multiplex_add_ip6
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux)
{
}

void multiplex_add_socket_sx
        (const char *path, void (*on_connect)(struct sexpr_io *, void *), void *data)
{
}

void multiplex_add_ip4_sx
        (int_32 addr, int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux)
{
}

void multiplex_add_ip6_sx
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux)
{
}

struct sexpr_io *sx_open_socket
        (const char *path)
{
    return (struct sexpr_io *)0;
}

struct sexpr_io *sx_open_ip4 (int_32 addr, int_16 port)
{
    return (struct sexpr_io *)0;
}

struct sexpr_io *sx_open_ip6 (int_8 addr[16], int_16 port)
{
    return (struct sexpr_io *)0;
}

void multiplex_add_socket_client_sx
        (const char *path, void (*on_read)(sexpr, struct sexpr_io *, void *), void *d)
{
}

void multiplex_add_ip4_client_sx
        (int_32 addr, int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux)
{
}

void multiplex_add_ip6_client_sx
        (int_8 addr[16], int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux)
{
}
