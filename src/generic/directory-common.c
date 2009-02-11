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

#include <curie/filesystem.h>
#include <curie/directory.h>

define_string (str_slash,    "/");
define_string (str_slashdot, "/.");
define_string (str_dot,      ".");

sexpr read_directory_sx (sexpr rx)
{
    sexpr r = sx_end_of_list;

    if (consp (rx)) {
        struct graph *g = sexpr_to_graph (rx);
        r = read_directory_rx (".", g);
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
    unsigned int l = 0, s = 0, c = 0;

    while (p[l]) {
        if (p[l] == '/') s++;

        l++;
    }

    s++;
    l++;

    char *map [s];
    char mapd [l];

    map[0] = mapd;

    for (l = s = 0; p[l]; l++)
    {
        if (p[l] == '/') {
            mapd[l] = 0;
            map[s] = (mapd + c);
            s++;
            c = l+1;
        } else {
            mapd[l] = p[l];
        }
    }

    mapd[l] = 0;
    map[s] = (mapd + c);
    s++;

    if (map[0][0] == 0)
    {
        r = cons (str_slashdot, r);
    }
    else
    {
        r = cons (str_dot, r);
    }

    for (c = 0; c < s; c++)
    {
        if (map[c][0] == 0) continue;

        char regex = 0;
        char *t = map[c];
        sexpr nr = sx_end_of_list;

        if (!((t[0] == '.') &&
               ((t[1] == 0) || ((t[1] == '.') && (t[2] == 0)))))
        {
            for (unsigned int cx = 0; (regex == 0) && t[cx]; cx++)
            {
                switch (t[cx])
                {
                    case '\\':
                    case '?':
                    case '*':
                    case '+':
                    case '(':
                    case ')':
                    case '|':
                    case '[':
                    case '.':
                        regex = 1;
                        break;
                    default:
                        break;
                }
            }
        }

        if (regex)
        {
            struct graph *g = rx_compile (map[c]);

            for (sexpr c = r; consp(c); c = cdr(c))
            {
                sexpr ca = car (c);
                sexpr n = read_directory_rx (sx_string (ca), g);

                for (sexpr e = n; consp (e); e = cdr (e))
                {
                    sexpr b = car (e);

                    nr = cons (sx_join (ca, str_slash, b), nr);
                }

                sx_destroy (n);
            }

            sx_destroy (r);
            graph_destroy (g);
        }
        else
        {
            sexpr b = make_string (t);

            for (sexpr c = r; consp(c); c = cdr(c))
            {
                sexpr ca = car (c);
                sexpr nf = sx_join (ca, str_slash, b);

                if (truep(filep (nf)))
                {
                    nr = cons (nf, nr);
                }
            }

            sx_destroy (b);
            sx_destroy (r);
        }

        r = nr;
    }

    return r;
}
