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

#include <curie/multiplex.h>
#include <curie/multiplex-system.h>

static struct multiplex_functions *mx_func_list =
    (struct multiplex_functions *)0;

enum multiplex_result multiplex () {
    struct multiplex_functions *cur = mx_func_list;
    int rnum = 0, wnum = 0;

    for (cur = mx_func_list;
         cur != (struct multiplex_functions *)0;
         cur = cur->next)
    {
        if (cur->count (&rnum, &wnum) == mx_immediate_action)
        {
            for (cur = mx_func_list;
                 cur != (struct multiplex_functions *)0;
                 cur = cur->next)
            {
                cur->callback ((int *)0, 0, (int *)0, 0);
            }

            return mx_ok;
        }
    }

    if ((rnum == 0) && (wnum == 0)) {
        return mx_nothing_to_do;
    } else {
        int rfds[rnum], wfds[wnum];

        rnum = 0;
        wnum = 0;

        for (cur = mx_func_list;
             cur != (struct multiplex_functions *)0;
             cur = cur->next) {
            cur->augment (rfds, &rnum, wfds, &wnum);
        }

        if ((rnum == 0) && (wnum == 0)) {
            return mx_nothing_to_do;
        }

        a_select_with_fds (rfds, rnum, wfds, wnum);

        for (cur = mx_func_list;
             cur != (struct multiplex_functions *)0;
             cur = cur->next) {
            cur->callback (rfds, rnum, wfds, wnum);
        }

        return mx_ok;
    }
}

void multiplex_add (struct multiplex_functions *mx) {
    mx->next = mx_func_list;

    mx_func_list = mx;
}
