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

#define _POSIX_SOURCE

#include <syscall/syscall.h>
#include <curie/network-system.h>
#include <curie/io-system.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <linux/un.h>

enum io_result a_open_loop(int result[])
{
    int r = sys_socketpair(AF_UNIX, SOCK_STREAM, 0, result);

    if (r < 0)
    {
        return io_unrecoverable_error;
    }
    else
    {
        a_make_nonblocking (result[0]);
        a_make_nonblocking (result[1]);
    }

    return io_complete;
}

enum io_result a_open_socket(int *result, const char *path)
{
    int fd, i;
    struct sockaddr_un addr_un;
    char *tc = (char *)&(addr_un);

    if ((fd = sys_socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        return io_unrecoverable_error;
    }

    for (i = 0; i < sizeof(struct sockaddr_un); i++)
    {
        tc[i] = (char)0;
    }

    addr_un.sun_family = AF_UNIX;
    for (i = 0; (i < (sizeof(addr_un.sun_path)-1)) && (path[i] != (char)0); i++)
    {
        addr_un.sun_path[i] = path[i];
    }

    if (sys_connect(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) < 0)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_open_listen_socket(int *result, const char *path)
{
    int fd, i;
    struct sockaddr_un addr_un;
    char *tc = (char *)&(addr_un);

    a_unlink(path);

    if ((fd = sys_socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        return io_unrecoverable_error;
    }

    for (i = 0; i < sizeof(struct sockaddr_un); i++)
    {
        tc[i] = (char)0;
    }

    addr_un.sun_family = AF_UNIX;
    for (i = 0; (i < (sizeof(addr_un.sun_path)-1)) && (path[i] != (char)0); i++)
    {
        addr_un.sun_path[i] = path[i];
    }

    if (sys_bind(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) < 0)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    if (sys_listen(fd, 32) == -1)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_open_ip4 (int *result, int_32 addr, int_16 port)
{
    int fd;
    struct sockaddr_in addr_in;
    union { int_16 i; int_8 c[2]; } a;
    union { int_32 i; int_8 c[4]; } b;

    if ((fd = sys_socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return io_unrecoverable_error;
    }

    a.c[0] = (port >> 8) & 0xff;
    a.c[1] = (port)      & 0xff;

    b.c[0] = (addr >> 24) & 0xff;
    b.c[1] = (addr >> 16) & 0xff;
    b.c[2] = (addr >> 8)  & 0xff;
    b.c[3] = (addr)       & 0xff;

    addr_in.sin_family      = AF_INET;
    addr_in.sin_port        = a.i;
    addr_in.sin_addr.s_addr = b.i;

    if (sys_connect(fd, (struct sockaddr *) &addr_in, sizeof(struct sockaddr_in)) < 0)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_open_listen_ip4 (int *result, int_32 addr, int_16 port)
{
    int fd;
    struct sockaddr_in addr_in;
    union { int_16 i; int_8 c[2]; } a;
    union { int_32 i; int_8 c[4]; } b;

    if ((fd = sys_socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return io_unrecoverable_error;
    }

    a.c[0] = (port >> 8) & 0xff;
    a.c[1] = (port)      & 0xff;

    b.c[0] = (addr >> 24) & 0xff;
    b.c[1] = (addr >> 16) & 0xff;
    b.c[2] = (addr >> 8)  & 0xff;
    b.c[3] = (addr)       & 0xff;

    addr_in.sin_family      = AF_INET;
    addr_in.sin_port        = a.i;
    addr_in.sin_addr.s_addr = b.i;

    if (sys_bind(fd, (struct sockaddr *) &addr_in, sizeof(struct sockaddr_in)) < 0)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    if (sys_listen(fd, 32) == -1)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_open_ip6 (int *result, int_8 addr[16], int_16 port)
{
    int fd;
    struct sockaddr_in6 addr_in;
    union { int_16 i; int_8 c[2]; } a;

    if ((fd = sys_socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        return io_unrecoverable_error;
    }

    a.c[0] = (port >> 8) & 0xff;
    a.c[1] = (port)      & 0xff;

    addr_in.sin6_family           = AF_INET6;
    addr_in.sin6_port             = a.i;
    addr_in.sin6_addr.s6_addr[0]  = addr[0];
    addr_in.sin6_addr.s6_addr[1]  = addr[1];
    addr_in.sin6_addr.s6_addr[2]  = addr[2];
    addr_in.sin6_addr.s6_addr[3]  = addr[3];
    addr_in.sin6_addr.s6_addr[4]  = addr[4];
    addr_in.sin6_addr.s6_addr[5]  = addr[5];
    addr_in.sin6_addr.s6_addr[6]  = addr[6];
    addr_in.sin6_addr.s6_addr[7]  = addr[7];
    addr_in.sin6_addr.s6_addr[8]  = addr[8];
    addr_in.sin6_addr.s6_addr[9]  = addr[9];
    addr_in.sin6_addr.s6_addr[10] = addr[10];
    addr_in.sin6_addr.s6_addr[11] = addr[11];
    addr_in.sin6_addr.s6_addr[12] = addr[12];
    addr_in.sin6_addr.s6_addr[13] = addr[13];
    addr_in.sin6_addr.s6_addr[14] = addr[14];
    addr_in.sin6_addr.s6_addr[15] = addr[15];

    if (sys_connect(fd, (struct sockaddr *) &addr_in, sizeof(struct sockaddr_in6)) < 0)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_open_listen_ip6 (int *result, int_8 addr[16], int_16 port)
{
    int fd;
    struct sockaddr_in6 addr_in;
    union { int_16 i; int_8 c[2]; } a;

    if ((fd = sys_socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        return io_unrecoverable_error;
    }

    a.c[0] = (port >> 8) & 0xff;
    a.c[1] = (port)      & 0xff;

    addr_in.sin6_family           = AF_INET6;
    addr_in.sin6_port             = a.i;
    addr_in.sin6_addr.s6_addr[0]  = addr[0];
    addr_in.sin6_addr.s6_addr[1]  = addr[1];
    addr_in.sin6_addr.s6_addr[2]  = addr[2];
    addr_in.sin6_addr.s6_addr[3]  = addr[3];
    addr_in.sin6_addr.s6_addr[4]  = addr[4];
    addr_in.sin6_addr.s6_addr[5]  = addr[5];
    addr_in.sin6_addr.s6_addr[6]  = addr[6];
    addr_in.sin6_addr.s6_addr[7]  = addr[7];
    addr_in.sin6_addr.s6_addr[8]  = addr[8];
    addr_in.sin6_addr.s6_addr[9]  = addr[9];
    addr_in.sin6_addr.s6_addr[10] = addr[10];
    addr_in.sin6_addr.s6_addr[11] = addr[11];
    addr_in.sin6_addr.s6_addr[12] = addr[12];
    addr_in.sin6_addr.s6_addr[13] = addr[13];
    addr_in.sin6_addr.s6_addr[14] = addr[14];
    addr_in.sin6_addr.s6_addr[15] = addr[15];

    if (sys_bind(fd, (struct sockaddr *) &addr_in, sizeof(struct sockaddr_in6)) < 0)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    if (sys_listen(fd, 32) == -1)
    {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_accept_socket(int *result, int fd)
{
    int rfd = sys_accept (fd, 0, 0);
    if      (rfd == -4)  { return io_incomplete; } /* EINTR */
    else if (rfd == -11) { return io_incomplete; } /* EAGAIN */
    else if (rfd < 0)    { return io_unrecoverable_error; }

    *result = rfd;
    return io_complete;
}
