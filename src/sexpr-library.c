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

#include <curie/sexpr.h>
#include <curie/sexpr-internal.h>
#include <curie/string.h>
#include <curie/memory.h>

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
    else if (customp(a) && customp(b))
    {
        int type = sx_type (a);

        if (type == sx_type (b))
        {
            struct sexpr_type_descriptor *d = sx_get_descriptor (type);

            if ((d != (struct sexpr_type_descriptor *)0) &&
                (d->equalp != (void *)0))
            {
                return d->equalp (a, b);
            }
        }
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
    char *g;
    sexpr rv;

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

    g = aalloc (i);

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

    rv = stringp(a) ? make_string (g) : make_symbol (g);
    afree (i, g);

    return rv;
}

sexpr sx_reverse (sexpr sx)
{
    sexpr result = sx_end_of_list;
    sexpr reverse = sx;

    while (consp(reverse)) {
        sexpr ncar = car (reverse);
        if (dotp (ncar)) {
            sexpr nresult = car(result);
            result = nresult;
        } else {
            result = cons(ncar, result);
        }
        reverse = cdr (reverse);
    }

    return result;
}
