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

#include <syscall/syscall.h>
#include <curie/multiplex-system.h>

#define BITSPERBYTE 8
#define MAXCELLS 16
#define CELLSIZE (unsigned int)(sizeof(unsigned int) * BITSPERBYTE)
#define MAXFDS CELLSIZE * MAXCELLS

typedef unsigned int fdcell [MAXCELLS];

static void fdzero(fdcell *c) {
    unsigned int i = 0;

    while (i < MAXCELLS)
    {
        (*c)[i] = 0;
        i++;
    }
}

static void fdset(fdcell *c, int fd) {
    unsigned int cell = ((unsigned int)fd) / CELLSIZE,
                 cellbit = ((unsigned int)fd) % CELLSIZE;

    (*c)[cell] |= 1 << cellbit;
}

static char fdisset(fdcell *c, int fd) {
    unsigned int cell = ((unsigned int)fd) / CELLSIZE,
                 cellbit = ((unsigned int)fd) % CELLSIZE;

    return (((*c)[cell]) & (1 << cellbit)) == 0 ? (char)0 : (char)1;
}

void a_select_with_fds (int *rfds, int rnum, int *wfds, int wnum) {
    fdcell rset, wset;
    int highest = 0, r;
    int i;

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

#if defined(have_sys_newselect)
    r = sys_newselect(highest + 1, (void *)&rset, (void *)&wset, 0, 0);
#else
    /* actually, this probably wont work on most arches... */
    r = sys_select(highest + 1, (void *)&rset, (void *)&wset, 0, 0);
#endif

    if (r <= 0) {
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
