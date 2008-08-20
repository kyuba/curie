/*
 *  curie-allocator.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 15/06/2008.
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

int a_main(void) {
    unsigned int i;

    for (i = 1; i < MAXSIZE; i++) {
        unsigned int rv;

        rv = test_aalloc(i * 2);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
