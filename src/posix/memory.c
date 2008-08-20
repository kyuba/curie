/*
 *  memory.c
 *  libcurie
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

/*@-nullpass@*/
/* we explicitly pass a (void*)0 as the first parameter of mmap;
   apparently splint thinks this to be illegal, but according to SUS this
   is allowed */

/*@-mustfreeonly@*/
/* this seems necessary for the free_mem() function to pass...
   problem is we can't meaningfully check for a failure in munmap() here,
   and it looks like the strictposix definition of munmap() isnt taking
   an only-annotated pointer */

#define _BSD_SOURCE

#define ATOMIC_PAGE_SIZE 0x1000

#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

/*@only@*/ void *get_mem(unsigned long int);
/*@only@*/ void *resize_mem(unsigned long int, /*@only@*/ void *, unsigned long int);
void free_mem(unsigned long int, /*@only@*/void *);
/*@only@*/ void *get_mem_chunk();

void mark_mem_ro (unsigned long int, /*@notnull@*/ void *);
void mark_mem_rw (unsigned long int, /*@notnull@*/ void *);

static size_t get_multiple_of_pagesize(unsigned long int s)
{
    if ((s % ATOMIC_PAGE_SIZE) == 0)
        return (size_t)s;
    else
        return (size_t)(((s / ATOMIC_PAGE_SIZE) + 1) * ATOMIC_PAGE_SIZE);
}

void *get_mem(unsigned long int size) {
    void *rv = (void *)-1;
    size_t msize = get_multiple_of_pagesize(size);

    retry:

    rv = mmap((void *)0, msize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,
              -1, 0);

    if ((rv == (void *)-1) || (rv == (void *)0)) {
        /* failure is not an option */
        (void)sleep (1);
        goto retry;
    }

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
        int *new_location = (int *)get_mem(new_size),
            *old_location = (int *)location;
        int i = 0,
            copysize = (int)((size < new_size) ? size : new_size);

        copysize = (int)((copysize / sizeof(int))
                         + (((copysize % sizeof(int)) == 0) ? 0 : 1));

        for(; i < copysize; i++) {
            /* copy in chunks of ints */
            new_location[i] = old_location[i];
        }

        free_mem (size, location);

        return (void*)new_location;
    } else return location;
}

void free_mem(unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    /* we ignore the return value, because the only way this will return in a
       bad way is whenever people pass invalid data to this function, which in
       turn would either lead to serious issues with the kernel or other parts
       of the programme. */
    (void)munmap (location, msize);
}

void *get_mem_chunk() {
    return get_mem(ATOMIC_PAGE_SIZE);
}

void mark_mem_ro (unsigned long int size, /*@notnull@*/ void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    /* again we ignore the return value, because failure to mark the memory will
       only result in the memory still being writable, which is not going to be
       a critical issue */
    (void)mprotect (location, msize, PROT_READ);
}

void mark_mem_rw (unsigned long int size, /*@notnull@*/ void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    (void)mprotect (location, msize, PROT_READ | PROT_WRITE);
}
