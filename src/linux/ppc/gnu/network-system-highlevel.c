/*
 *  linux-ppc-gnu/network-system-highlevel.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 17/08/2008.
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

#define _POSIX_SOURCE

#include <atomic/network-system.h>
#include <atomic/io-system.h>

#define _ASM_POWERPC_SOCKET_H
#define _LINUX_WAIT_H

#include <linux/net.h>
#include <linux/un.h>

int __a_socketcall(int, unsigned long *);

static int __a_unix_socketpair (int res[2]) {
    unsigned long a[6]= { AF_UNIX, SOCK_STREAM, 0, (unsigned long)res, 0, 0 };

    return __a_socketcall(SYS_SOCKETPAIR, a);
}

static int __a_accept (int fd) {
    unsigned long a[6] = { fd, 0, 0, 0, 0, 0 };

    return __a_socketcall(SYS_ACCEPT, a);
}

static int __a_unix_socket () {
    unsigned long a[6] = { AF_UNIX, SOCK_STREAM, 0, 0, 0, 0 };

    return __a_socketcall(SYS_SOCKET, a);
}

static int __a_bind (int fd, void *s, int size) {
    unsigned long a[6] = { fd, (unsigned long)s, size, 0, 0, 0 };

    return __a_socketcall(SYS_BIND, a);
}

static int __a_listen (int fd) {
    unsigned long a[6] = { fd, 32, 0, 0, 0, 0 };

    return __a_socketcall(SYS_LISTEN, a);
}

static int __a_connect (int fd, void *s, int size) {
    unsigned long a[6] = { fd, (unsigned long)s, size, 0, 0, 0 };

    return __a_socketcall(SYS_CONNECT, a);
}

enum io_result a_open_loop(int result[2]) {
    int r = __a_unix_socketpair(result);

    if (r < 0) {
        return io_unrecoverable_error;
    } else {
        a_make_nonblocking (result[0]);
        a_make_nonblocking (result[1]);
    }

    return io_complete;
}

enum io_result a_open_socket(int *result, const char *path) {
    int fd, i;
    struct sockaddr_un addr_un;
    char *tc = (char *)&(addr_un);

    if ((fd = __a_unix_socket()) < 0) {
        return io_unrecoverable_error;
    }

    for (i = 0; i < sizeof(struct sockaddr_un); i++) { tc[i] = (char)0; }

    addr_un.sun_family = AF_UNIX;
    for (i = 0; (i < (sizeof(addr_un.sun_path)-1)) && (path[i] != (char)0); i++) {
        addr_un.sun_path[i] = path[i];
    }

    if (__a_connect(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) < 0) {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_open_listen_socket(int *result, const char *path) {
    int fd, i;
    struct sockaddr_un addr_un;
    char *tc = (char *)&(addr_un);

    a_unlink(path);

    if ((fd = __a_unix_socket()) < 0) {
        return io_unrecoverable_error;
    }

    for (i = 0; i < sizeof(struct sockaddr_un); i++) { tc[i] = (char)0; }

    addr_un.sun_family = AF_UNIX;
    for (i = 0; (i < (sizeof(addr_un.sun_path)-1)) && (path[i] != (char)0); i++) {
        addr_un.sun_path[i] = path[i];
    }

    if (__a_bind(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) < 0) {
        a_close (fd);
        return io_unrecoverable_error;
    }

    if (__a_listen(fd) == -1) {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}


enum io_result a_accept_socket(int *result, int fd) {
    int rfd = __a_accept (fd);
    if (rfd == -1) return io_unrecoverable_error;
    if (rfd == -2) return io_incomplete;

    *result = rfd;
    return io_complete;
}
