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

/*! \internal
 *
 * @{
 */

/*! \file
 *  \brief Glue Code Header for network.h
 *
 */

#ifndef LIBCURIE_NETWORK_SYSTEM_H
#define LIBCURIE_NETWORK_SYSTEM_H

#include <curie/io.h>

#define _ASM_POWERPC_SOCKET_H
#define _LINUX_WAIT_H

#include <linux/net.h>
#include <linux/un.h>

enum io_result a_open_loop (int result[]);
enum io_result a_open_socket (int *result, const char *path);
enum io_result a_open_listen_socket (int *result, const char *path);
enum io_result a_open_ip4 (int *result, int_32 addr, int_16 port);
enum io_result a_open_listen_ip4 (int *result, int_32 addr, int_16 port);
enum io_result a_open_ip6 (int *result, int_8 addr[16], int_16 port);
enum io_result a_open_listen_ip6 (int *result, int_8 addr[16], int_16 port);
enum io_result a_accept_socket (int *result, int fd);

int __a_socketcall(int, unsigned long *);

static inline int __a_unix_socketpair (int res[2]) {
    unsigned long a[6]= { AF_UNIX, SOCK_STREAM, 0, (unsigned long)res, 0, 0 };

    return __a_socketcall(SYS_SOCKETPAIR, a);
}

static inline int __a_accept (int fd) {
    unsigned long a[6] = { fd, 0, 0, 0, 0, 0 };

    return __a_socketcall(SYS_ACCEPT, a);
}

static inline int __a_unix_socket () {
    unsigned long a[6] = { AF_UNIX, SOCK_STREAM, 0, 0, 0, 0 };

    return __a_socketcall(SYS_SOCKET, a);
}

static inline int __a_ip4_socket () {
    unsigned long a[6] = { PF_INET, SOCK_STREAM, 0, 0, 0, 0 };

    return __a_socketcall(SYS_SOCKET, a);
}

static inline int __a_ip6_socket () {
    unsigned long a[6] = { PF_INET6, SOCK_STREAM, 0, 0, 0, 0 };

    return __a_socketcall(SYS_SOCKET, a);
}

static inline int __a_bind (int fd, void *s, int size) {
    unsigned long a[6] = { fd, (unsigned long)s, size, 0, 0, 0 };

    return __a_socketcall(SYS_BIND, a);
}

static inline int __a_listen (int fd) {
    unsigned long a[6] = { fd, 32, 0, 0, 0, 0 };

    return __a_socketcall(SYS_LISTEN, a);
}

static inline int __a_connect (int fd, void *s, int size) {
    unsigned long a[6] = { fd, (unsigned long)s, size, 0, 0, 0 };

    return __a_socketcall(SYS_CONNECT, a);
}

#endif

/*! @} */
