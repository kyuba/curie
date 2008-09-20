/*
 *  io-system.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 27/05/2008.
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

#include <curie/io-system.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <errno.h>

char last_error_recoverable_p = (char)1;

static void examine_error( void );

static void examine_error( void ) {
  /*@-checkstrictglobs@*/
  switch (errno)
  /*@=checkstrictglobs@*/
  {
    case EINTR:
    case EAGAIN:
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
    case EWOULDBLOCK:
#endif
      last_error_recoverable_p = (char)1;
      break;
    default:
      last_error_recoverable_p = (char)0;
  }
}

int    a_read (int fd, void *buf, unsigned int count)
{
    /*@-checkstrictglobs@*/
    int rv = (int)read(fd, buf, (size_t)count);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_write (int fd, const void *buf, unsigned int count)
{
    /*@-checkstrictglobs@*/
    int rv = (int)write(fd, buf, (size_t)count);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_open_read (const char *path)
{
    /*@-checkstrictglobs@*/
    int rv = open(path, O_RDONLY | O_NONBLOCK);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_open_write (const char *path)
{
    /*@-checkstrictglobs@*/
    int rv = open(path, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_create (const char *path, int mode)
{
    /*@-checkstrictglobs@*/
    int rv = open(path, O_WRONLY | O_NONBLOCK | O_CREAT, (mode_t)mode);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_close (int fd)
{
    /*@-checkstrictglobs@*/
    int rv = close (fd);
    /*@=checkstrictglobs@*/
    if (rv < 0) {
        examine_error();

        if (last_error_recoverable_p == (char)1) return a_close (fd);
    }
    return rv;
}

int    a_dup (int ofd, int nfd)
{
    /*@-checkstrictglobs@*/
    int rv = dup2 (ofd, nfd);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_dup_n (int fd)
{
    /*@-checkstrictglobs@*/
    int rv = dup (fd);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_make_nonblocking (int fd) {
    /*@-checkstrictglobs@*/
    int rv = fcntl(fd, F_SETFL, O_NONBLOCK);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}

int    a_unlink (const char *path) {
    /*@-checkstrictglobs@*/
    int rv = unlink(path);
    /*@=checkstrictglobs@*/
    if (rv < 0) examine_error();
    return rv;
}
