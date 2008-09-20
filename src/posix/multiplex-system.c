/*
 *  multiplex-system.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 07/08/2008.
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

#include <curie/multiplex-system.h>
#include <sys/select.h>

#ifdef S_SPLINT_S
/* seems to be needed on my box... */

typedef unsigned int fd_set[16];
#endif

void a_select_with_fds (int *rfds, int rnum, int *wfds, int wnum) {
    fd_set rset, wset;
    int highest = 0, r, i;

    /*@-unrecog@*/
    FD_ZERO(&rset);
    /*@=unrecog@*/

    for (i = 0; i < rnum; i++) {
        if (rfds[i] > highest) highest = rfds[i];
        /*@-unrecog@*/
        FD_SET(rfds[i], &rset);
        /*@=unrecog@*/
    }

    /*@-unrecog@*/
    FD_ZERO(&wset);
    /*@=unrecog@*/

    for (i = 0; i < wnum; i++) {
        if (wfds[i] > highest) highest = wfds[i];
        /*@-unrecog@*/
        FD_SET(wfds[i], &wset);
        /*@=unrecog@*/
    }

    /*@-unrecog@*/
    r = select(highest + 1, &rset, &wset, (fd_set *)0, (void *)0);
    /*@=unrecog@*/

    if (r <= 0) {
        for (i = 0; i < rnum; i++) {
            rfds[i] = -1;
        }
        for (i = 0; i < wnum; i++) {
            wfds[i] = -1;
        }
    } else {
        for (i = 0; i < rnum; i++) {
            /*@-unrecog@*/
            if (!FD_ISSET(rfds[i], &rset)) {
                /*@=unrecog@*/
                rfds[i] = -1;
            }
        }
        for (i = 0; i < wnum; i++) {
            /*@-unrecog@*/
            if (!FD_ISSET(wfds[i], &wset)) {
                /*@=unrecog@*/
                wfds[i] = -1;
            }
        }
    }
}
