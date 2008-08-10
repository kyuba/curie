/*
 *  network-system.c
 *  atomic-libc
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

#define _POSIX_SOURCE

#include <atomic/network-system.h>
#include <atomic/io-system.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/un.h>
#include <errno.h>

enum io_result a_open_loop(int result[2]) {
    int r = socketpair(AF_UNIX, SOCK_STREAM, 0, result);

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

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        return io_unrecoverable_error;
    }

    for (i = 0; i < sizeof(struct sockaddr_un); i++) { tc[i] = (char)0; }

    addr_un.sun_family = AF_UNIX;
    for (i = 0; (i < (sizeof(addr_un.sun_path)-1)) && (path[i] != (char)0); i++) {
        addr_un.sun_path[i] = path[i];
    }

    if (connect(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) == -1) {
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

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        return io_unrecoverable_error;
    }

    for (i = 0; i < sizeof(struct sockaddr_un); i++) { tc[i] = (char)0; }

    addr_un.sun_family = AF_UNIX;
    for (i = 0; (i < (sizeof(addr_un.sun_path)-1)) && (path[i] != (char)0); i++) {
        addr_un.sun_path[i] = path[i];
    }

    if (bind(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) == -1) {
        a_close (fd);
        return io_unrecoverable_error;
    }

    if (listen(fd, 32) == -1) {
        a_close (fd);
        return io_unrecoverable_error;
    }

    *result = fd;

    return io_complete;
}

enum io_result a_accept_socket(int *result, int fd) {
    int rfd = accept (fd, (struct sockaddr *)0, (socklen_t *)0);
    if (rfd < 0) {
        switch (errno) {
            case EINTR:
            case EAGAIN:
#if EWOULDBLOCK && (EWOULDBLOCK != EAGAIN)
            case EWOULDBLOCK:
#endif
                return io_incomplete;
            default:
                return io_unrecoverable_error;
        }
    }

    *result = rfd;
    return io_complete;
}