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

#include <curie/memory.h>
#include <curie/tree.h>

void *aalloc   (unsigned long size) {
    if (size < CURIE_POOL_CUTOFF) {
        struct memory_pool pool = MEMORY_POOL_INITIALISER(size);

        return get_pool_mem (&pool);
    } else {
        return get_mem (calculate_aligned_memory_size (size));
    }
}

void *arealloc (unsigned long size, void *p, unsigned long new_size) {
    unsigned long msize = calculate_aligned_memory_size (size);
    unsigned long mnew_size = calculate_aligned_memory_size (new_size);

    if (msize != mnew_size) {
        if ((msize >= CURIE_POOL_CUTOFF) &&
            (mnew_size >= CURIE_POOL_CUTOFF)) {
            return resize_mem (msize, p, mnew_size);
        } else {
            unsigned int *new_location = (unsigned int *)aalloc(mnew_size);
            if (new_location == (unsigned int *)0)
            {
                afree (msize, p);
                return (void *)0;
            }
            else
            {
                unsigned int *old_location = (unsigned int *)p;
                int copysize = (int)((msize < mnew_size) ? msize : mnew_size);

                copysize = (int)((copysize / sizeof(int))
                        + (((copysize % sizeof(int)) == 0) ? 0 : 1));

                for(int i = 0; i < copysize; i++) {
                    /* copy in chunks of ints */
                    new_location[i] = old_location[i];
                }

                afree (msize, p);

                return (void*)new_location;
            }
        }
    }

    return p;
}

void afree     (unsigned long size, void *p) {
    if (size < CURIE_POOL_CUTOFF) {
        free_pool_mem (p);
    } else {
        free_mem (calculate_aligned_memory_size (size), p);
    }
}
