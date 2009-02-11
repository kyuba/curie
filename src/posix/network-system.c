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

#define _POSIX_SOURCE

#include <curie/network-system.h>
#include <curie/io-system.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>

#include <sys/un.h>
#include <errno.h>

#ifdef S_SPLINT_S
/* fixes for splint... */

struct sockaddr;
struct sockaddr_un
{
    int sun_family;
    char sun_path[128];
};
typedef unsigned int socklen_t;
#endif

enum io_result a_open_loop(int result[]) {
    /*@-unrecog@*/
    int r = socketpair(AF_UNIX, SOCK_STREAM, 0, result);
    /*@=unrecog@*/

    /*@-usedef -mustdefine@*/
    if (r < 0) {
        return io_unrecoverable_error;
    } else {
        (void)a_make_nonblocking (result[0]);
        (void)a_make_nonblocking (result[1]);
    }

    return io_complete;
    /*@=usedef =mustdefine@*/
}

enum io_result a_open_socket(int *result, const char *path) {
    int fd, i;
    struct sockaddr_un addr_un;
    char *tc = (char *)&(addr_un);

    /*@-unrecog@*/
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    /*@=unrecog@*/
    *result = fd;

    if (fd == -1) {
        return io_unrecoverable_error;
    }

    for (i = 0;
         i < (int)sizeof(struct sockaddr_un);
         i++)
    {
        tc[i] = (char)0;
    }

    /*@-unrecog@*/
    addr_un.sun_family = AF_UNIX;
    /*@=unrecog@*/
    for (i = 0;
         (i < (int)(sizeof(addr_un.sun_path)-1))
         && (path[i] != (char)0);
         i++)
    {
        addr_un.sun_path[i] = path[i];
    }

    /*@-unrecog@*/
    if (connect(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) == -1) {
        /*@=unrecog@*/
        (void)a_close (fd);
        return io_unrecoverable_error;
    }

    return io_complete;
}

enum io_result a_open_listen_socket(int *result, const char *path) {
    int fd, i;
    struct sockaddr_un addr_un;
    char *tc = (char *)&(addr_un);

    (void)a_unlink(path);

    /*@-unrecog@*/
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    /*@=unrecog@*/
    *result = fd;

    if (fd == -1) {
        return io_unrecoverable_error;
    }

    for (i = 0;
         i < (int)sizeof(struct sockaddr_un); i++)
    {
        tc[i] = (char)0;
    }

    /*@-unrecog@*/
    addr_un.sun_family = AF_UNIX;
    /*@=unrecog@*/
    for (i = 0;
         (i < (int)(sizeof(addr_un.sun_path)-1)) &&
         (path[i] != (char)0);
         i++)
    {
        addr_un.sun_path[i] = path[i];
    }

    /*@-unrecog@*/
    if (bind(fd, (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un)) == -1) {
        /*@=unrecog@*/
        (void)a_close (fd);
        return io_unrecoverable_error;
    }

    /*@-unrecog@*/
    if (listen(fd, 32) == -1) {
        /*@=unrecog@*/
        (void)a_close (fd);
        return io_unrecoverable_error;
    }

    return io_complete;
}

enum io_result a_accept_socket(int *result, int fd) {
    /*@-unrecog@*/
    int rfd = accept (fd, (struct sockaddr *)0, (socklen_t *)0);
    /*@=unrecog@*/
    *result = rfd;

    if (rfd < 0) {
        /*@-checkstrictglobs@*/
        switch (errno) {
        /*@=checkstrictglobs@*/
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

    return io_complete;
}
