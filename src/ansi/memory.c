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

#include <stdlib.h>

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

    rv = calloc (1, msize);

    if ((rv == (void *)-1) || (rv == (void *)0)) {
        if (get_mem_recovery != (void *)0)
        {
            return get_mem_recovery(size);
        }

        return (void *)0;
    }

    return rv;
}

void *resize_mem(unsigned long int size, void *location, unsigned long int new_size) {
    void *rv = (void *)-1;

    rv = realloc (location, get_multiple_of_pagesize(new_size));

    if ((rv == (void *)-1) || (rv == (void *)0)) {
        if (get_mem_recovery != (void *)0)
        {
            return resize_mem_recovery (size, location, new_size);
        }

        return (void *)0;
    }

    return rv;
}

void free_mem(unsigned long int size, void *location) {
    free (location);
}

void *get_mem_chunk() {
    return get_mem(LIBCURIE_PAGE_SIZE);
}

/* the following functions are not supported with generic POSIX functions */

void mark_mem_ro (unsigned long int size, void *location) {
}

void mark_mem_rw (unsigned long int size, void *location) {
}

void mark_mem_rx (unsigned long int size, void *location) {
}
