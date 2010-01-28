/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#include <curie/memory.h>

#include <windows.h>

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

    rv = VirtualAlloc ((void *)0, msize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if ((rv == (void *)-1) || (rv == (void *)0)) {
        return get_mem_recovery(size);
    }

    return rv;
}

void *resize_mem(unsigned long int size, void *location, unsigned long int new_size) {
    size_t msize = get_multiple_of_pagesize(size);
    size_t mnew_size = get_multiple_of_pagesize(new_size);

    if (msize != mnew_size) {
        int *new_location = (int *)get_mem(new_size);

        if (new_location == (int *)0)
        {
            return resize_mem_recovery(size, location, new_size);
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
/*    size_t msize = get_multiple_of_pagesize(size); */

    (void)VirtualFree (location, 0, MEM_RELEASE);
}

void *get_mem_chunk() {
    return get_mem(LIBCURIE_PAGE_SIZE);
}

void mark_mem_ro (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);
    unsigned long p;

    (void)VirtualProtect (location, msize, PAGE_READONLY, &p);
}

void mark_mem_rw (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);
    unsigned long p;

    (void)VirtualProtect (location, msize, PAGE_READWRITE, &p);
}

void mark_mem_rx (unsigned long int size, void *location) {
    size_t msize = get_multiple_of_pagesize(size);
    unsigned long p;

    (void)VirtualProtect (location, msize, PAGE_EXECUTE_READ, &p);
}
