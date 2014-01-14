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

#define _POSIX_SOURCE

#include <curie/multiplex-system.h>
#include <sys/select.h>

void a_select_with_fds (int *rfds, int rnum, int *wfds, int wnum) {
    fd_set rset, wset;
    int highest = 0, r, i;

    FD_ZERO(&rset);

    for (i = 0; i < rnum; i++) {
        if (rfds[i] > highest) highest = rfds[i];
        FD_SET(rfds[i], &rset);
    }

    FD_ZERO(&wset);

    for (i = 0; i < wnum; i++) {
        if (wfds[i] > highest) highest = wfds[i];
        FD_SET(wfds[i], &wset);
    }

    r = select(highest + 1, &rset, &wset, (fd_set *)0, (void *)0);

    if (r <= 0) {
        for (i = 0; i < rnum; i++) {
            rfds[i] = -1;
        }
        for (i = 0; i < wnum; i++) {
            wfds[i] = -1;
        }
    } else {
        for (i = 0; i < rnum; i++) {
            if (!FD_ISSET(rfds[i], &rset)) {
                rfds[i] = -1;
            }
        }
        for (i = 0; i < wnum; i++) {
            if (!FD_ISSET(wfds[i], &wset)) {
                wfds[i] = -1;
            }
        }
    }
}
