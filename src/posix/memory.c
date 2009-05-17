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

#define _BSD_SOURCE

#include <curie/memory.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

typedef void *(*get_mem_recovery_t)(unsigned long int);
typedef void *(*resize_mem_recovery_t)(unsigned long int, void *, unsigned long int);

static get_mem_recovery_t get_mem_recovery = (void *)0;
static resize_mem_recovery_t resize_mem_recovery = (void *)0;

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

    rv = mmap((void *)0, msize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,
              -1, 0);

    if ((rv == (void *)-1) || (rv == (void *)0)) {
        if (get_mem_recovery != (void *)0)
        {
            return get_mem_recovery(size);
        }

        return (void *)0;
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
        int *new_location = (int *)get_mem(new_size);

        if (new_location == (int *)0)
        {
            if (resize_mem_recovery != (void *)0)
            {
                return resize_mem_recovery(size, location, new_size);
            }

            return (void *)0;
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

void free_mem(unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    /* we ignore the return value, because the only way this will return in a
       bad way is whenever people pass invalid data to this function, which in
       turn would either lead to serious issues with the kernel or other parts
       of the programme. */
    (void)munmap (location, msize);
}

void *get_mem_chunk() {
    return get_mem(LIBCURIE_PAGE_SIZE);
}

void mark_mem_ro (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    /* again we ignore the return value, because failure to mark the memory will
       only result in the memory still being writable, which is not going to be
       a critical issue */
    (void)mprotect (location, msize, PROT_READ);
}

void mark_mem_rw (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    (void)mprotect (location, msize, PROT_READ | PROT_WRITE);
}

void mark_mem_rx (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);

    (void)mprotect (location, msize, PROT_READ | PROT_EXEC);
}
