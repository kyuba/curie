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

#include <sievert/sexpr.h>

sexpr sx_alist_get (sexpr alist, sexpr key)
{
    while (consp (alist))
    {
        sexpr a = car (alist);

        if (truep (equalp (car (a), key)))
        {
            return cdr (a);
        }

        alist = cdr (alist);
    }

    return sx_nonexistent;
}

sexpr sx_alist_add (sexpr alist, sexpr key, sexpr value)
{
    return cons (cons (key, value), alist);
}

sexpr sx_alist_remove (sexpr alist, sexpr key)
{
    sexpr r = sx_end_of_list;

    while (consp (alist))
    {
        sexpr a = car (alist);

        if (falsep (equalp (car (a), key)))
        {
            r = cons (a, r);
        }

        alist = cdr (alist);
    }

    return r;
}

sexpr sx_alist_merge (sexpr alist1, sexpr alist2)
{
    sexpr r = alist1;

    while (consp (alist2))
    {
        sexpr a = car (alist2);

        r = sx_alist_remove (r, car (a));
        r = cons (a, r);

        alist2 = cdr (alist2);
    }

    return r;
}
