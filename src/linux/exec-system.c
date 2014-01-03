/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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

#include <syscall/syscall.h>
#include <curie/exec-system.h>

enum wait_return a_wait(int pid, int *status) {
    int st = 0;
    enum wait_return r;
    int st_masked;

    (void)sys_wait4((signed int)pid, &st, 0x1 /*WNOHANG*/, (void *)0);

    st_masked = st & 0x7f;

    if (st_masked == 0) { /* mask = 0 means it exited normally*/
        r = wr_exited;
        *status = (int)((st & 0xff00) >> 8);
    }
    else if (((signed char)(st_masked + 1) >> 1) > 0) /* signal */
    {
        r = wr_killed;
        *status = (int)-st_masked;
    }
    else /* otherwise */
    {
        r = wr_running;
        *status = (int)0;
    }

    return r;
}

int a_wait_all(int *status) {
    int st, r;

    r = sys_wait4((signed int)-1, &st, 0x1 /*WNOHANG*/, (void *)0);

    if ((st & 0x7f) == 0) /* see above */
    {
        *status = ((st & 0xff00) >> 8);
    }

    return r;
}
