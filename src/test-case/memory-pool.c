/**\file
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
*/

#include "curie/memory.h"

#define MAXSIZE 23
#define MAXENTITIES 1026

unsigned int counter = 0;

static unsigned int test_pool(unsigned int poolentitysize, unsigned int usepoolentities) {
    struct memory_pool *pool = create_memory_pool(poolentitysize*sizeof(unsigned int));
    unsigned int i, j, rv = 0, **entities;

    entities = (unsigned int **)get_mem(sizeof(unsigned int *)*usepoolentities);

    for (i = 0; i < usepoolentities; i++) {
        entities[i] = (unsigned int *)get_pool_mem(pool);
    }

    for (i = 0; i < usepoolentities; i++) {
        for (j = 0; j < poolentitysize; j++) {
            entities[i][j] = counter + i + j;
        }
    }

    for (i = 0; i < usepoolentities; i++) {
        for (j = 0; j < poolentitysize; j++) {
            if (entities[i][j] != (counter + i + j)) {
                rv = 2;
                goto do_return;
            }
        }
    }

    counter += usepoolentities + poolentitysize;

    do_return:

    for (i = 0; i < usepoolentities; i++) {
        free_pool_mem ((void *)(entities[i]));
    }

    free_mem(sizeof(unsigned int *)*usepoolentities, (void *)entities);

    free_memory_pool(pool);

    return rv;
}

int cmain(void) {
    unsigned int i, j;

    for (i = 1; i < MAXSIZE; i++) {
        for (j = 1; j < MAXENTITIES; j++) {
            unsigned int rv = 0;

            rv = test_pool(i, j);

            if (rv != (unsigned int)0) return (int)rv;
        }
    }

    return 0;
}
