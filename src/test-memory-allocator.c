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

#include <curie/memory.h>

#define MAXSIZE 2048

unsigned int test_aalloc (unsigned int size) {
    unsigned int *p = (unsigned int *)aalloc (size * sizeof (int));
    unsigned int i;

    for (i = 0; i < size; i++) {
        p[i] = i;
    }

    for (i = 0; i < size; i++) {
        if (p[i] != i) {
            afree (size * sizeof(int), (void *)p);
            return 1;
        }
    }

    p = (unsigned int *)arealloc (size * sizeof (int), p, size*sizeof (int)*2);

    for (i = 0; i < size; i++) {
        if (p[i] != i) {
            afree ((size * sizeof(int) *2), (void *)p);
            return 2;
        }
    }

    for (i = 0; i < (size*2); i++) {
        p[i] = i;
    }

    for (i = 0; i < (size*2); i++) {
        if (p[i] != i) {
            afree ((size * sizeof(int) *2), (void *)p);
            return 3;
        }
    }

    afree ((size * sizeof(int) *2), (void *)p);

    return 0;
}

int cmain(void) {
    unsigned int i;

    for (i = 1; i < MAXSIZE; i++) {
        unsigned int rv;

        rv = test_aalloc(i * 2);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
