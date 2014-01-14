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

#include <sievert/sexpr.h>

sexpr sx_set_sort_merge
    (sexpr set, sexpr (*gtp)(sexpr, sexpr, void *), void *aux)
{
    sexpr a = sx_end_of_list, b = sx_end_of_list, c = set;

    while (consp (c))
    {
        a = cons (car (c), a);

        c = cdr (c);

        if (consp (c))
        {
            b = cons (car (c), b);
            
            c = cdr (c);
        }
        else
        {
            break;
        }
    }

    if (eolp (b)) /* one or no elements in the list, since b wasnt touched */
    {
        return a;
    }
    else
    {
        sexpr rv = sx_end_of_list, aa, ba;

        a = sx_set_sort_merge (a, gtp, aux);
        b = sx_set_sort_merge (b, gtp, aux);

        aa = car (a);
        ba = car (b);

        while (consp (a) && consp (b))
        {
            if (truep (gtp (ba, aa, aux)))
            {
                rv = cons (aa, rv);
                a  = cdr (a);
                aa = car (a);
            }
            else
            {
                rv = cons (ba, rv);
                b  = cdr (b);
                ba = car (b);
            }
        }

        while (consp (a))
        {
            rv = cons (car (a), rv);
            a  = cdr (a);
        }

        while (consp (b))
        {
            rv = cons (car (b), rv);
            b  = cdr (b);
        }

        return sx_reverse (rv);
    }
}

