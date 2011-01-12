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
#include <sievert/string.h>

sexpr sx_split (sexpr item, sexpr separator)
{
    sexpr rv = sx_end_of_list;
    const char *s;
    char **set;
    int sep;

    if (symbolp (separator))
    {
        s = sx_symbol (separator);
        sep = (int)(s[0]);
    }
    else if (stringp (separator))
    {
        s = sx_string (separator);
        sep = (int)(s[0]);
    }
    else
    {
        sep = sx_integer (separator);
    }

    if (symbolp (item))
    {
        s = sx_symbol (item);
        set = str_split (s, sep);

        while (*set != (char *)0)
        {
            rv = cons (make_symbol (*set), rv);

            set++;
        }

        return rv;
    }
    else if (stringp (item))
    {
        s = sx_string (item);
        set = str_split (s, sep);

        while (*set != (char *)0)
        {
            rv = cons (make_string (*set), rv);

            set++;
        }

        return rv;
    }
        
    return sx_end_of_list;
}

sexpr sx_merge (sexpr set, sexpr glue)
{
    sexpr t;
    const char *s;
    char **sset = (char **)0;

    while (consp (set))
    {
        t = car (set);

        if (stringp (t))
        {
            sset = str_set_add (sset, sx_string (t));
        }
        else if (symbolp (t))
        {
            sset = str_set_add (sset, sx_symbol (t));
        }

        set = cdr (set);
    }

    if (sset == (char **)0)
    {
        return sx_nil;
    }

    if (stringp (glue))
    {
        s = sx_string (glue);

        return make_string (str_merge (sset, (int)(s[0])));
    }
    else if (symbolp (glue))
    {
        s = sx_symbol (glue);

        return make_symbol (str_merge (sset, (int)(s[0])));
    }

    return make_string (str_merge (sset, sx_integer(glue)));
}
