/*
 *  atom.c
 *  atomic-libc
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

int    a_read  (int fd, /*@out@*/ void *buf, unsigned int count)
/*@globals errno;@*/;
int    a_write (int fd, const void *buf, unsigned int count)
/*@globals errno;@*/;

int    a_open_read (const char *path)
/*@globals errno;@*/;
int    a_open_write (const char *path)
/*@globals errno;@*/;
int    a_create (const char *path, int mode)
/*@globals errno;@*/;
int    a_close (int fd)
/*@globals errno;@*/;

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>

char last_error_recoverable_p = (char)1;

static void examine_error( void )
 /*@globals errno;@*/;

static void examine_error( void ) {
  switch (errno)
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
    int rv = (int)read(fd, buf, (size_t)count);
    if (rv < 0) examine_error();
    return rv;
}

int    a_write (int fd, const void *buf, unsigned int count)
{
    int rv = (int)write(fd, buf, (size_t)count);
    if (rv < 0) examine_error();
    return rv;
}

int    a_open_read (const char *path)
{
    int rv = open(path, O_RDONLY | O_NONBLOCK);
    if (rv < 0) examine_error();
    return rv;
}

int    a_open_write (const char *path)
{
    int rv = open(path, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);
    if (rv < 0) examine_error();
    return rv;
}

int    a_create (const char *path, int mode)
{
    int rv = open(path, O_WRONLY | O_NONBLOCK | O_CREAT, (mode_t)mode);
    if (rv < 0) examine_error();
    return rv;
}

int    a_close (int fd)
{
    int rv = close (fd);
    if (rv < 0) {
        examine_error();

        if (last_error_recoverable_p == (char)1) return a_close (fd);
    }
    return rv;
}

int   a_dup (int ofd, int nfd)
{
    int rv = dup2 (ofd, nfd);
    if (rv < 0) examine_error();
    return rv;
}
