/*
 *  memory-primitives.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 08/06/2008.
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

#include "atomic/memory.h"

/* this should be larger than most any PAGESIZE out there */
#define CHUNKSIZE (int)(64*1024)
#define CHUNKSLOTS (int)(CHUNKSIZE/sizeof(int))

#define CHUNKSIZE2 (int)((2*64*1024)+3) /* wee bit meaner this way */
#define CHUNKSLOTS2 (int)(CHUNKSIZE2/sizeof(int))

int a_main(void) {
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
