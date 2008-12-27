/*
 *  generic/directory-common.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 27/12/2008.
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


#include <curie/directory.h>

sexpr read_directory_sx (sexpr rx)
{
    sexpr r = sx_end_of_list;

    if (consp (rx)) {
        struct graph *g = sexpr_to_graph (rx);
        r = read_directory_rx (sx_false, g);
        graph_destroy (g);
    } else if (stringp (rx)) {
        r = read_directory (sx_string (rx));
    } else if (symbolp (rx)) {
        r = read_directory (sx_symbol (rx));
    }

    return r;
}

sexpr read_directory    (const char *p)
{
    sexpr r = sx_end_of_list;
    unsigned int l = 0, s = 0;

    while (p[l]) {
        if (p[l] == '/') s++;

        l++;
    }

    l++;

    char *map [s];
    char mapd [l];

    map[0] = mapd;

    for (l = s = 0; p[l]; l++)
    {
        if (p[l] == '/') {
            mapd[l] = 0;
            s++;
        } else {
            mapd[l] = p[l];
        }

        l++;
    }

    mapd[l] = 0;

    return r;
}
