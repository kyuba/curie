/*
 *  multiplex-system-highlevel.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 10/08/2008.
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

#define _POSIX_SOURCE

#include <atomic/multiplex-system.h>

int __a_select (int, void *, void *);

#define BITSPERBYTE 8
#define MAXCELLS 16
#define CELLSIZE (sizeof(unsigned int) * BITSPERBYTE)
#define MAXFDS CELLSIZE * MAXCELLS

typedef unsigned int fdcell [MAXCELLS];

static void fdzero(fdcell *c) {
    unsigned int i;

    for (i = 0; i < MAXCELLS; i++) {
        (*c)[i] = 0;
    }
}

static void fdset(fdcell *c, int fd) {
    unsigned int cell = fd / CELLSIZE, cellbit = fd % CELLSIZE;

    (*c)[cell] |= 1 << cellbit;
}

static char fdisset(fdcell *c, int fd) {
    unsigned int cell = fd / CELLSIZE, cellbit = fd % CELLSIZE;

    return (((*c)[cell]) & (1 << cellbit)) == 0 ? (char)0 : (char)1;
}

void a_select_with_fds (int *rfds, int rnum, int *wfds, int wnum) {
    fdcell rset, wset;
    int highest = 0, r;
    unsigned int i;

    fdzero(&rset);

    for (i = 0; i < rnum; i++) {
        if (rfds[i] > highest) highest = rfds[i];
        fdset(&rset, rfds[i]);
    }

    fdzero(&wset);

    for (i = 0; i < wnum; i++) {
        if (wfds[i] > highest) highest = wfds[i];
        fdset(&wset, wfds[i]);
    }

    r = __a_select(highest + 1, (void *)&rset, (void *)&wset);

    if (r < 0) {
        for (i = 0; i < rnum; i++) {
            rfds[i] = -1;
        }
        for (i = 0; i < wnum; i++) {
            wfds[i] = -1;
        }
    } else {
        for (i = 0; i < rnum; i++) {
            if (fdisset(&rset, rfds[i]) == (char)0) {
                rfds[i] = -1;
            }
        }
        for (i = 0; i < wnum; i++) {
            if (fdisset(&wset, wfds[i]) == (char)0) {
                wfds[i] = -1;
            }
        }
    }
}
