/*
 *  benchmarks/posix-malloc-random-resize.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 23/06/2008.
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

#include <atomic/io.h>
#include "benchmark.h"
#include <stdlib.h>

unsigned int random_numbers[RANDOM_INTEGERS_S];

int read_random_integers_file() {
    unsigned int i;

    struct io *r = io_open_read (RANDOM_INTEGERS_FILE);

    while (r->length < (unsigned int)(RANDOM_INTEGERS_S * sizeof(unsigned int))) {
        if (io_read(r) == io_unrecoverable_error) {
            io_close (r);
            return 1;
        }
    }

    for (i = 0; i < RANDOM_INTEGERS_S; i++) {
        random_numbers[i] = ((unsigned int *)(r->buffer))[i];
    }

    io_close (r);

    return 0;
}

#define RANDOM_INTEGERS_H (unsigned int)(RANDOM_INTEGERS_S / 2)

int test_malloc_random_sizes_resize () {
    unsigned int i, **d = malloc(RANDOM_INTEGERS_H * sizeof (void *));
    int rv = 0;

    if (d == (void *)0) return 1;

    for (i = 0; i < RANDOM_INTEGERS_S; i++) {
        d[i] = malloc (random_numbers[i]);

        if (d[i] == (void *)0) return 2;
    }

    for (i = 0; i < RANDOM_INTEGERS_H; i++) {
        int j;

        for (j = 0; j < (unsigned int)(random_numbers[i] / sizeof(unsigned int)); j++) {
            d[i][j] = random_numbers[i];
        }
    }

    for (i = 0; i < RANDOM_INTEGERS_H; i++) {
        int j;

        for (j = 0; j < (unsigned int)(random_numbers[i] / sizeof(unsigned int)); j++) {
            if (d[i][j] != random_numbers[i]) {
                rv = 3;
                goto quit;
            }
        }
    }

    for (i = 0; i < RANDOM_INTEGERS_H; i++) {
        d[i] = realloc (d[i], random_numbers[i + RANDOM_INTEGERS_H]);
    }

    for (i = 0; i < RANDOM_INTEGERS_H; i++) {
        int j;
        unsigned int max = ((unsigned int)(random_numbers[i] / sizeof(unsigned int)) <
		                    (unsigned int)(random_numbers[i + RANDOM_INTEGERS_H] / sizeof(unsigned int))) ?
							(unsigned int)(random_numbers[i] / sizeof(unsigned int)) :
							(unsigned int)(random_numbers[i + RANDOM_INTEGERS_H] / sizeof(unsigned int));

        for (j = 0; j < max; j++) {
            if (d[i][j] != random_numbers[i]) {
                rv = 4;
                goto quit;
            }
        }
    }

    for (i = 0; i < RANDOM_INTEGERS_H; i++) {
        int j;

        for (j = 0; j < (unsigned int)(random_numbers[i + RANDOM_INTEGERS_H] / sizeof(unsigned int)); j++) {
            d[i][j] = random_numbers[i];
        }
    }

    for (i = 0; i < RANDOM_INTEGERS_H; i++) {
        int j;

        for (j = 0; j < (unsigned int)(random_numbers[i + RANDOM_INTEGERS_H] / sizeof(unsigned int)); j++) {
            if (d[i][j] != random_numbers[i]) {
                rv = 5;
                goto quit;
            }
        }
    }

    quit:

    for (i = 0; i < RANDOM_INTEGERS_H; i++) {
        free (d[i]);
    }

    free (d);

    return rv;
}

int main(int argc, char *argv[]) {
    int r;

    if ((r = read_random_integers_file()) != 0) return r;
    if ((r = test_malloc_random_sizes_resize()) != 0) return r;

    return 0;
}

