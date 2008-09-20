/*
 *  memory.c
 *  libcurie
 *
 *  this is a valgrind/memcheck-aware version of the posix memory.c file.
 *
 *  Created by Magnus Deininger on 01/06/2008.
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

#define _BSD_SOURCE

#include <curie/memory.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <valgrind/memcheck.h>

typedef void *(*get_mem_recovery_t)(unsigned long int);
typedef void *(*resize_mem_recovery_t)(unsigned long int, void *, unsigned long int);

/*@null@*/ static get_mem_recovery_t get_mem_recovery = (void *)0;
/*@null@*/ static resize_mem_recovery_t resize_mem_recovery = (void *)0;

void set_get_mem_recovery_function (void *(*handler)(unsigned long int))
{
    get_mem_recovery = handler;
}

void set_resize_mem_recovery_function (void *(*handler)(unsigned long int, void *, unsigned long int))
{
    resize_mem_recovery = handler;
}

static size_t get_multiple_of_pagesize(unsigned long int s)
{
    if ((s % LIBCURIE_PAGE_SIZE) == 0)
        return (size_t)s;
    else
        return (size_t)(((s / LIBCURIE_PAGE_SIZE) + 1) * LIBCURIE_PAGE_SIZE);
}

void *get_mem(unsigned long int size) {
    void *rv = (void *)-1;
    size_t msize = get_multiple_of_pagesize(size);

    /*@-unrecog@*/
    rv = mmap((void *)0, msize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,
              -1, 0);
    /*@=unrecog@*/

    if ((rv == (void *)-1) || (rv == (void *)0)) {
        if (get_mem_recovery != (void *)0)
        {
            return get_mem_recovery(size);
        }

        return (void *)0;
    }

    VALGRIND_MALLOCLIKE_BLOCK(rv, msize, 0, 0);

    return rv;
}

/* this function will reallocate a new chunk of memory and copy the contents of
   the old chunk to the new chunk;
   it's copying in chunks of ints, and it should do so in a way that wont cause
   aligning issues on most systems, however it might cause unaligned access
   errors on anything powered by an sh4 cpu, so this function will need to be
   revised there. on the other hand, about the only unix-ish OS i've heard
   people use on sh4s is linux, and on linux we'll be using mremap() anyway, so
   meh... */

void *resize_mem(unsigned long int size, void *location, unsigned long int new_size) {
/* with plain posix calls we can't implement a resize that doesn't
   need to copy a lot around */
    size_t msize = get_multiple_of_pagesize(size);
    size_t mnew_size = get_multiple_of_pagesize(new_size);

    if (msize != mnew_size) {
        int *new_location = (int *)get_mem(new_size);

        if (new_location == (int *)0)
        {
            /*@-mustfreeonly@*/
            if (resize_mem_recovery != (void *)0)
            {
                return resize_mem_recovery(size, location, new_size);
            }

            return (void *)0;
            /*@=mustfreeonly@*/
        } else {
            int *old_location = (int *)location;
            int i = 0,
                copysize = (int)((size < new_size) ? size : new_size);

            copysize = (int)((copysize / sizeof(int))
                             + (((copysize % sizeof(int)) == 0) ? 0 : 1));

            while (i < copysize)
            {
                /* copy in chunks of ints */
                new_location[i] = old_location[i];
                i++;
            }

            free_mem (size, location);

            return (void*)new_location;
        }
    }

    return location;
}

/*@-mustfreeonly@*/
void free_mem(unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    /* we ignore the return value, because the only way this will return in a
       bad way is whenever people pass invalid data to this function, which in
       turn would either lead to serious issues with the kernel or other parts
       of the programme. */
    /*@-unrecog@*/
    (void)munmap (location, msize);
    /*@=unrecog@*/

    VALGRIND_FREELIKE_BLOCK(location, 0);
}
/*@=mustfreeonly@*/

void *get_mem_chunk() {
    return get_mem(LIBCURIE_PAGE_SIZE);
}

void mark_mem_ro (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    /* again we ignore the return value, because failure to mark the memory will
       only result in the memory still being writable, which is not going to be
       a critical issue */
    /*@-unrecog@*/
    (void)mprotect (location, msize, PROT_READ);
    /*@=unrecog@*/
}

void mark_mem_rw (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    /*@-unrecog@*/
    (void)mprotect (location, msize, PROT_READ | PROT_WRITE);
    /*@=unrecog@*/
}
