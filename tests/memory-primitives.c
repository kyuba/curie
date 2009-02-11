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

#include "curie/memory.h"

/* this should be larger than most any PAGESIZE out there */
#define CHUNKSIZE (int)(64*1024)
#define CHUNKSLOTS (int)(CHUNKSIZE/sizeof(int))

#define CHUNKSIZE2 (int)((2*64*1024)+3) /* wee bit meaner this way */
#define CHUNKSLOTS2 (int)(CHUNKSIZE2/sizeof(int))

int cmain(void) {
    int *chunk = (int *)get_mem(CHUNKSIZE);
	int i;

	for (i = 0; i < CHUNKSLOTS; i++) {
		chunk[i] = i;
	}

	for (i = 0; i < CHUNKSLOTS; i++) {
	    if (chunk[i] != i) {
            free_mem(CHUNKSIZE, chunk);
		    return 1;
		}
	}

	chunk = resize_mem(CHUNKSIZE, chunk, CHUNKSIZE2);

	for (i = 0; i < CHUNKSLOTS; i++) {
	    if (chunk[i] != i) {
            free_mem(CHUNKSIZE2, chunk);
		    return 2;
        }
	}

	for (i = 0; i < CHUNKSLOTS2; i++) {
		chunk[i] = i;
	}

	for (i = 0; i < CHUNKSLOTS2; i++) {
	    if (chunk[i] != i) {
            free_mem(CHUNKSIZE2, chunk);
		    return 3;
        }
	}

	free_mem(CHUNKSIZE2, chunk);

	return 0;
}
