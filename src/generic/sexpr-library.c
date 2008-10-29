/*
 *  sexpr-library.c
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

#include <curie/sexpr.h>

#if 0
/* not implemented yet */

void *sx_list_map (sexpr sx, void (*callback)(sexpr, void *), void *p);
sexpr sx_list_fold (sexpr sx, void (*callback)(sexpr ));
#endif

sexpr equalp (sexpr a, sexpr b) {
    if (a == b) return sx_true;

    if (!pointerp(a) || !pointerp(b)) return sx_false;

    if ((stringp(a) && stringp(b)) || (symbolp(a) && symbolp(b)))
    {
        struct sexpr_string_or_symbol
                *sa = (struct sexpr_string_or_symbol *)sx_pointer(a),
                *sb = (struct sexpr_string_or_symbol *)sx_pointer(b);
        int i;

        for (i = 0; (sa->character_data[i] == sb->character_data[i]) &&
                    (sa->character_data[i] != (char)0); i++);

        return (sa->character_data[i] == (char)0) ? sx_true : sx_false;
    }
    else if (consp(a) && consp(b))
    {
        return ((truep(equalp(car(a), car(b))) &&
                 truep(equalp(cdr(a), cdr(b))))) ?
                sx_true : sx_false;
    }

    return sx_false;
}
