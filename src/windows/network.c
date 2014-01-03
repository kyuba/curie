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

#include <curie/multiplex-system.h>
#include <curie/memory.h>
#include <curie/network.h>
#include <curie/network-system.h>
#include <curie/io-system.h>
#include <windows.h>

void net_open_loop (struct io **in, struct io **out) {
    struct io *iin, *iout;
    void *ihandle = 0, *ohandle = 0;
    SECURITY_ATTRIBUTES s;
    char namebuffer[] = "\\\\.\\Pipe\\CurieNetLoop.00000000.00000000";
    DWORD pid = GetCurrentProcessId();
    static DWORD fnum = 0;
    int n = 0;

    while (n < 8)
    {
        namebuffer[(22 + n)] = '0' + pid % 10;
        pid /= 10;
        n++;
    }

    pid = fnum++;
    n = 0;

    while (n < 8)
    {
        namebuffer[(31 + n)] = '0' + pid % 10;
        pid /= 10;
        n++;
    }

    s.nLength              = sizeof (s);
    s.lpSecurityDescriptor = (void *)0;
    s.bInheritHandle       = TRUE;

    ihandle =
        CreateNamedPipeA
                (namebuffer, PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
                 PIPE_TYPE_BYTE | PIPE_WAIT, 1, 0x1000, 0x1000, 30000,
                 &s);

    if (ihandle)
    {
        ohandle =
            CreateFileA
                    (namebuffer, GENERIC_WRITE, 0, &s, OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
    }

    iin = io_open (ihandle);
    iout = io_open (ohandle);

    iin->type = iot_read;
    iout->type = iot_write;

    (*in) = iin;
    (*out) = iout;
}

void net_open_socket
        (const char *path, struct io **in, struct io **out)
{
#pragma message ("net_open_socket() incomplete")
}

void net_open_ip4 (int_32 addr, int_16 port, struct io **in, struct io **out)
{
#pragma message ("net_open_ip4() incomplete")
}

void net_open_ip6 (int_8 addr[16], int_16 port, struct io **in, struct io **out)
{
#pragma message ("net_open_ip6() incomplete")
}

void multiplex_network ()
{
#pragma message ("network_multiplex() incomplete")
}

void multiplex_add_socket
        (const char *path, void (*on_connect)(struct io *, struct io *, void *), void *data)
{
#pragma message ("network_add_socket() incomplete")
}

void multiplex_add_ip4
        (int_32 addr, int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux)
{
#pragma message ("network_add_ip4() incomplete")
}

void multiplex_add_ip6
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct io *, struct io *, void *), void *aux)
{
#pragma message ("network_add_ip6() incomplete")
}

void multiplex_add_socket_sx
        (const char *path, void (*on_connect)(struct sexpr_io *, void *), void *data)
{
#pragma message ("network_add_socket_sx() incomplete")
}

void multiplex_add_ip4_sx
        (int_32 addr, int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux)
{
#pragma message ("network_add_ip4_sx() incomplete")
}

void multiplex_add_ip6_sx
        (int_8 addr[16], int_16 port,
         void (*on_connect)(struct sexpr_io *, void *), void *aux)
{
#pragma message ("network_add_ip6_sx() incomplete")
}

struct sexpr_io *sx_open_socket
        (const char *path)
{
#pragma message ("sx_open_socket() incomplete")
    return (struct sexpr_io *)0;
}

struct sexpr_io *sx_open_ip4 (int_32 addr, int_16 port)
{
#pragma message ("sx_open_ip4() incomplete")
    return (struct sexpr_io *)0;
}

struct sexpr_io *sx_open_ip6 (int_8 addr[16], int_16 port)
{
#pragma message ("sx_open_ip6() incomplete")
    return (struct sexpr_io *)0;
}

void multiplex_add_socket_client_sx
        (const char *path, void (*on_read)(sexpr, struct sexpr_io *, void *), void *d)
{
#pragma message ("multiplex_add_socket_client_sx() incomplete")
}

void multiplex_add_ip4_client_sx
        (int_32 addr, int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux)
{
#pragma message ("multiplex_add_ip4_client_sx() incomplete")
}

void multiplex_add_ip6_client_sx
        (int_8 addr[16], int_16 port,
         void (*on_read)(sexpr, struct sexpr_io *, void *), void *aux)
{
#pragma message ("multiplex_add_ip6_client_sx() incomplete")
}
