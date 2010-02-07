/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#include <curie/memory.h>
#include <curie/regex.h>
#include <sievert/immutable.h>
#include <sievert/string.h>

/* note that throughout the code, we're not adding a (char *)0 as the last
 * element, because immutable() will already effectively do the same. this
 * might, however, not work on architectures where (char *)0 is not an all-zero
 * bit pattern... we don't care about this for now, since lately those are
 * somewhat rare. in case curie would get ported to such an architecture, we
 * would thus have to make a special version of this file. */

char **str_set_add (char **set, const char *string)
{
    string = str_immutable (string);

    if (set == (char **)0)
    {
        const char *rv[] = { string };

        return (char **)immutable ((const void *)rv, sizeof (const char *));
    }
    else
    {
        char **cursor = set, **rv, **rvo;
        unsigned int items = 0, have_string = 0, size;

        while ((*cursor) != (char *)0)
        {
            if (have_string == 0)
            {
                if (str_immutable (*cursor) == string)
                {
                    have_string = ~0;
                }
            }

            items++;
            cursor++;
        }

        if (have_string != 0)
        {
            return set;
        }

        size = sizeof (const char *) * (items + 1);
        rvo = aalloc (size);
        cursor = rvo;

        for (cursor = rvo; (*set) != (char *)0; set++, cursor++)
        {
            *cursor = (char *)str_immutable(*set);
        }

        *cursor = (char*)string;

        rv = (char **)immutable ((const void *)rvo, size);

        afree (size, (void *)rvo);

        return rv;
    }
}

char **str_set_remove (char **set, const char *string)
{
    if (set == (char **)0)
    {
        return (char **)0;
    }
    else
    {
        char **cursor = set, **rv, **rvo;
        const char *t;
        unsigned int items = 0, have_string = 0, size;

        string = str_immutable (string);

        while ((*cursor) != (char *)0)
        {
            if (str_immutable (*cursor) == string)
            {
                have_string++;
            }
            else
            {
                items++;
            }

            cursor++;
        }

        if (have_string == 0)
        {
            return set;
        }

        if (items == 0)
        {
            return (char **)0;
        }

        size = sizeof (const char *) * items;
        rvo = aalloc (size);
        cursor = rvo;

        for (cursor = rvo; (*set) != (char *)0; set++)
        {
            t = str_immutable (*set);

            if (t != string)
            {
                *cursor = (char *)t;
                cursor++;
            }
        }

        rv = (char **)immutable ((const void *)rvo, size);

        afree (size, (void *)rvo);

        return rv;
    }
}

char **str_set_merge (char **a, char **b)
{
    if (a == (char **)0)
    {
        return b;
    }
    else if (b == (char **)0)
    {
        return a;
    }
    else
    {
        char **cursor, **rvo, **rv;
        unsigned int items = 0, size;
        const char *t;

        for (cursor = a; *cursor != (char *)0; cursor++)
        {
            items++;
        }
        for (cursor = b; *cursor != (char *)0; cursor++)
        {
            items++;
        }

        size = sizeof(const void *) * items;

        rvo = aalloc (size);

        rv = rvo;
        items = 0;

      repeat:
        while ((*a) != (char *)0)
        {
            t = str_immutable (*a);

            for (cursor = rvo; cursor < rv; cursor++)
            {
                if ((*cursor) == t)
                {
                    goto skip_add;
                }
            }

            *rv = (char *)t;

            rv++;
            items++;

          skip_add:
            a++;
        }

        if (b != (char **)0)
        {
            a = b;
            b = (char **)0;
            goto repeat;
        }

        rv = (char **)immutable ((const void *)rvo,
                                 sizeof (const void *) * items);

        afree (size, rvo);

        return rv;
    }
}

char **str_set_intersect (char **a, char **b)
{
    if ((a == (char **)0) || (b == (char **)0))
    {
        return (char **)0;
    }
    else
    {
        char **cursor, **tc, **rv, **rvo;
        unsigned int items = 0, itemsb = 0, size;
        const char *t;

        for (cursor = a; *cursor != (char *)0; cursor++)
        {
            items++;
        }
        for (cursor = b; *cursor != (char *)0; cursor++)
        {
            itemsb++;
        }

        size = sizeof(const void *) * ((items < itemsb) ? items : itemsb);

        rvo = aalloc (size);

        rv = rvo;
        items = 0;

        for (cursor = a; *cursor != (char *)0; cursor++)
        {
            t = str_immutable (*cursor);

            for (tc = b; *tc != (char *)0; tc++)
            {
                if (t == str_immutable (*tc))
                {
                    goto add;
                }
            }

            continue;

          add:
            *rv = (char *)t;
            rv++;
            items++;
        }

        if (items == 0)
        {
            rv = (char **)0;
        }
        else
        {
            rv = (char **)immutable ((const void *)rvo,
                                     sizeof (const char *) * items);
        }

        afree (size, rvo);

        return rv;
    }
}

char **str_set_difference (char **a, char **b)
{
    if (a == b)
    {
        return (char **)0;
    }
    else if (a == (char **)0)
    {
        return b;
    }
    else if (b == (char **)0)
    {
        return a;
    }
    else
    {
        char **cursor, **tc, **tcc, **rv, **rvo;
        unsigned int items = 0, size;
        const char *t;

        for (cursor = a; *cursor != (char *)0; cursor++)
        {
            items++;
        }
        for (cursor = b; *cursor != (char *)0; cursor++)
        {
            items++;
        }

        size = sizeof(const void *) * items;

        rvo = aalloc (size);

        rv = rvo;

        cursor = a;
        tcc = b;
        items = 0;

      repeat:
        while (*cursor != (char *)0)
        {
            t = str_immutable (*cursor);

            for (tc = tcc; *tc != (char *)0; tc++)
            {
                if (t == str_immutable (*tc))
                {
                    goto skip;
                }
            }

            *rv = (char *)t;
            rv++;
            items++;

          skip:
            cursor++;
        }

        if (tcc == b)
        {
            tcc = a;
            cursor = b;
            goto repeat;
        }

        if (items == 0)
        {
            rv = (char **)0;
        }
        else
        {
            rv = (char **)immutable ((const void *)rvo,
                                     sizeof (const char *) * items);
        }

        afree (size, rvo);

        return rv;
    }
}

int str_set_memberp (char **set, const char *string)
{
    if (set != (char **)0)
    {
        string = str_immutable (string);

        while (*set != (char *)0)
        {
            if (str_immutable (*set) == string)
            {
                return ~0;
            }

            set++;
        }
    }

    return 0;
}

char **str_split (const char *string, int separator)
{
    if (string == (const char *)0)
    {
        return (char **)0;
    }
    else
    {
        char **rvo, **rv;
        const char *cursor = string;
        unsigned int items = 0, start = 0, size, length;

        while (*cursor != (char)0)
        {
            if ((int)(*cursor) == separator)
            {
                items++;
            }

            cursor++;
        }

        items++;

        size = sizeof (const char *) * items;

        rvo = aalloc (size);

        rv = rvo;
        cursor = string;

        while (*cursor != (char)0)
        {
            if ((int)(*cursor) == separator)
            {
                length = cursor - (string + start);

                *rv = (char *)immutable (string + start, length);
                rv++;

                start += length + 1;
            }

            cursor++;
        }

        if ((length = cursor - (string + start)) != 0)
        {
            *rv = (char *)immutable (string + start, length);
        }

        rv = (char **)immutable ((const void *)rvo, size);

        afree (size, rvo);

        return rv;
    }
}

const char *str_merge (char **set, int glue)
{
    if (set == (char **)0)
    {
        return "";
    }
    else
    {
    }
#warning str_merge() not implemented
}
