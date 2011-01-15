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

#include <syscall/syscall.h>
#include <curie/io-system.h>

char last_error_recoverable_p = (char)1;

static void examine_error( int errno );

static void examine_error( int errno ) {
  switch (errno)
  {
    case -4 /*EINTR*/:
    case -11 /*EAGAIN*/:
      last_error_recoverable_p = (char)1;
      break;
    default:
      last_error_recoverable_p = (char)0;
  }
}

int    a_read (int fd, void *buf, unsigned int count)
{
    int rv = (int)sys_read(fd, buf, (signed long)count);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_write (int fd, const void *buf, unsigned int count)
{
    int rv = (int)sys_write(fd, (void *)buf, (signed long)count);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_open_read (const char *path)
{
    int rv = sys_open(path, 0x800 /*O_RDONLY | O_NONBLOCK*/, 0555);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_open_write (const char *path)
{
    int rv = sys_open(path, 0x841 /*O_WRONLY | O_NONBLOCK | O_CREAT*/, 0666);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_create (const char *path, int mode)
{
    int rv = sys_open(path, 0x841 /*O_WRONLY | O_NONBLOCK | O_CREAT*/, mode);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_close (int fd)
{
    int rv = sys_close (fd);
    if (rv < 0) {
        examine_error(rv);

        if (last_error_recoverable_p == (char)1) return a_close (fd);
    }
    return rv;
}

int    a_dup (int ofd, int nfd)
{
    int rv = sys_dup2 (ofd, nfd);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_dup_n (int fd)
{
    int rv = sys_dup (fd);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_make_nonblocking (int fd) {
    int rv = sys_fcntl(fd, 0x4 /*F_SETFL*/, 0x800 /*O_NONBLOCK*/);
    if (rv < 0) examine_error(rv);
    return rv;
}

int    a_unlink (const char *path) {
    int rv = sys_unlink(path);
    if (rv < 0) examine_error(rv);
    return rv;
}

int a_stat(const char *path, void *buffer)
{
    int rv = sys_stat((char *)path, buffer);
    if (rv < 0) examine_error(rv);
    return rv;
}

int a_lstat(const char *path, void *buffer)
{
    int rv = sys_lstat((void *)path, buffer);
    if (rv < 0) examine_error(rv);
    return rv;
}
