/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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

sexpr sx_set_add (sexpr set, sexpr item)
{
    sexpr c = set;

    while (consp (c))
    {
        if (truep (equalp (car (c), item)))
        {
            return set;
        }

        c = cdr (c);
    }

    return cons (item, set);
}

sexpr sx_set_remove (sexpr set, sexpr item)
{
    sexpr rv = sx_end_of_list, c = set, t;

    while (consp (c))
    {
        t = car (c);

        if (falsep (equalp (t, item)))
        {
            rv = cons (t, rv);
        }

        c = cdr (c);
    }

    return rv;
}

sexpr sx_set_merge (sexpr a, sexpr b)
{
    sexpr c, t;

    while (consp (b))
    {
        t = car (b);
        c = a;

        while (consp (c))
        {
            if (truep (equalp (car (c), t)))
            {
                goto next;
            }

            c = cdr (c);
        }

        a = cons (t, a);

      next:
        b = cdr (b);
    }

    return a;
}

sexpr sx_set_intersect (sexpr a, sexpr b)
{
    sexpr rv = sx_end_of_list, c, t;

    while (consp (b))
    {
        t = car (b);
        c = a;

        while (consp (c))
        {
            if (truep (equalp (car (c), t)))
            {
                rv = cons (t, rv);
                goto next;
            }

            c = cdr (c);
        }

      next:
        b = cdr (b);
    }

    return rv;
}

sexpr sx_set_difference (sexpr a, sexpr b)
{
    sexpr rv = sx_end_of_list, c, t, ob = b;

    while (consp (b))
    {
        t = car (b);
        c = a;

        while (consp (c))
        {
            if (truep (equalp (car (c), t)))
            {
                goto next_a;
            }

            c = cdr (c);
        }

        rv = cons (t, rv);

      next_a:
        b = cdr (b);
    }

    while (consp (a))
    {
        t = car (a);
        c = ob;

        while (consp (c))
        {
            if (truep (equalp (car (c), t)))
            {
                goto next_b;
            }

            c = cdr (c);
        }

        rv = cons (t, rv);

      next_b:
        a = cdr (a);
    }

    return rv;
}

sexpr sx_set_memberp (sexpr set, sexpr item)
{
    while (consp (set))
    {
        if (truep (equalp (car (set), item)))
        {
            return sx_true;
        }

        set = cdr (set);
    }

    return sx_false;
}
