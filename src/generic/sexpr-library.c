/*
 *  sexpr-library.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 15/06/2008.
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

#include <curie/sexpr.h>
#include <curie/string.h>

sexpr equalp (sexpr a, sexpr b)
{
    if (a == b) return sx_true;

    if (!pointerp(a) || !pointerp(b)) return sx_false;

    if ((stringp(a) && stringp(b)) || (symbolp(a) && symbolp(b)))
    {
        struct sexpr_string_or_symbol
                *sa = (struct sexpr_string_or_symbol *)sx_pointer(a),
                *sb = (struct sexpr_string_or_symbol *)sx_pointer(b);
        unsigned long i;

        return (str_hash(sa->character_data, &i) ==
                str_hash(sb->character_data, &i))
                ? sx_true : sx_false;
    }
    else if (consp(a) && consp(b))
    {
        return ((truep(equalp(car(a), car(b))) &&
                 truep(equalp(cdr(a), cdr(b))))) ?
                sx_true : sx_false;
    }

    return sx_false;
}

void sx_list_map (sexpr list, void (*f)(sexpr))
{
    while (consp (list))
    {
        f(car(list));

        list = cdr (list);
    }
}

sexpr sx_list_fold (sexpr list, sexpr (*f)(sexpr, sexpr), sexpr seed)
{
    while (consp (list))
    {
        seed = f(car(list), seed);

        list = cdr (list);
    }

    return seed;
}

sexpr sx_join (sexpr a, sexpr b, sexpr c)
{
    unsigned int i = 0, j = 0;
    const char *s;

    if (stringp (a) || symbolp(a))
    {
        s = stringp (a) ? sx_string (a) : sx_symbol(a);
        for (j = 0; s[j]; j++) i++;
    }
    else
    {
        return sx_nil;
    }
    if (stringp (b) || symbolp(b))
    {
        s = stringp (b) ? sx_string (b) : sx_symbol(b);
        for (j = 0; s[j]; j++) i++;
    }
    if (stringp (c) || symbolp(c))
    {
        s = stringp (c) ? sx_string (c) : sx_symbol(c);
        for (j = 0; s[j]; j++) i++;
    }

    i++;

    char g[i];

    i = 0;
    s = stringp (a) ? sx_string (a) : sx_symbol(a);
    for (j = 0; s[j]; j++) {
        g[i] = s[j];
        i++;
    }
    if (stringp (b) || symbolp(b))
    {
        s = stringp (b) ? sx_string (b) : sx_symbol(b);
        for (j = 0; s[j]; j++) {
            g[i] = s[j];
            i++;
        }
    }
    if (stringp (c) || symbolp(c))
    {
        s = stringp (c) ? sx_string (c) : sx_symbol(c);
        for (j = 0; s[j]; j++) {
            g[i] = s[j];
            i++;
        }
    }
    g[i] = 0;

    return stringp(a) ? make_string (g) : make_symbol (g);
}
