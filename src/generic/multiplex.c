/*
 *  multiplex.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 03/06/2008.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
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

#include <curie/multiplex.h>
#include <curie/multiplex-system.h>

/*@null@*/ static struct multiplex_functions *mx_func_list =
    (struct multiplex_functions *)0;

/*@-compdef@*/
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
/*@=compdef@*/

void multiplex_add (struct multiplex_functions *mx) {
    /*@-mustfree -memtrans@*/
    mx->next = mx_func_list;
    /*@=mustfree =memtrans@*/

    mx_func_list = mx;
}
