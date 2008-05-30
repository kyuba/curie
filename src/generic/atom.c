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

#include <atomic/nucleus.h>
#define _POSIX_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void   _atomic_exit (int status)
{
    return exit (status);
}

int    _atomic_read (int fd, void *buf, int count)
{
    return read(fd, buf, count);
}

int    _atomic_write (int fd, const void *buf, int count)
{
    return write(fd, buf, count);
}

int    _atomic_open_read (const char *path)
{
    return creat(path, O_RDONLY | O_NONBLOCK);
}

int    _atomic_open_write (const char *path)
{
    return creat(path, O_WRONLY | O_NONBLOCK);
}

int    _atomic_create (const char *path, int mode)
{
    return creat(path, mode);
}

int    _atomic_creat (const char *path, int mode)
{
    return creat (path, mode);
}

int    _atomic_close (int fd)
{
    return close (fd);
}

void * _atomic_mmap (void *start, int length, int prot, int flags, int fd,
                   int offset)
{
    return mmap (start, length, prot, flags, fd, offset);
}
int    _atomic_munmap (void *start, int length)
{
    return munmap (start, length);
}

int   _atomic_kill (int pid, int sig)
{
    return kill (pid, sig);
}

int   main ()
{
    return atomic_main();
}
