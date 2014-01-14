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

sexpr sx_integer_to_string_hex (int_pointer_s i)
{
    char num [SX_MAX_NUMBER_LENGTH];

    int neg = 0;
    unsigned int j = 1;

    num[(SX_MAX_NUMBER_LENGTH-2)] = 0;

    if(i < 0) {
        neg = 1;
        i *= -1;
    }
   
    do {
        char s;
        s = '0' + (char)(i % 16);

        if (s > '9')
        {
            s = 'a' + (s - '9' - 1);
        }

        num[(SX_MAX_NUMBER_LENGTH-2)-j] = s;

        i /= 16;
        j++;
    } while ((i != 0) && (j < (SX_MAX_NUMBER_LENGTH-2)));

    if(neg == 1) {
        num[SX_MAX_NUMBER_LENGTH-2-j] = '-';
        j++;
    }
    num[(SX_MAX_NUMBER_LENGTH-1)] = (char)0;

    return make_string (num + ((SX_MAX_NUMBER_LENGTH - 1) - j));
}

