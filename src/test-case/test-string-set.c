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

#include <curie/main.h>
#include <sievert/immutable.h>
#include <sievert/string.h>

int cmain()
{
    char **a = (char **)0, **b = (char **)0, **c = (char **)0;

    a = str_set_add (a, "hello");
    a = str_set_add (a, "world");

    if (!str_set_memberp (a, "hello"))           { return 0x01; }
    if (!str_set_memberp (a, "world"))           { return 0x02; }
    if (str_set_memberp  (b, "hello"))           { return 0x03; }

    b = str_set_add (b, "!");

    if (!str_set_memberp (b, "!"))               { return 0x04; }

    c = str_set_merge (a, b);

    if (!str_set_memberp (c, "hello"))           { return 0x05; }
    if (!str_set_memberp (c, "world"))           { return 0x06; }
    if (!str_set_memberp (c, "!"))               { return 0x07; }

    a = str_set_remove (a, "hello");

    if (str_set_memberp  (a, "hello"))           { return 0x08; }
    if (!str_set_memberp (a, "world"))           { return 0x09; }
    if (str_set_memberp  (a, "!"))               { return 0x0a; }

    a = str_set_intersect (a, c);

    if (str_set_memberp  (a, "hello"))           { return 0x0b; }
    if (!str_set_memberp (a, "world"))           { return 0x0c; }
    if (str_set_memberp  (a, "!"))               { return 0x0d; }

    a = str_set_difference (a, c);

    if (!str_set_memberp (a, "hello"))           { return 0x0e; }
    if (str_set_memberp  (a, "world"))           { return 0x0f; }
    if (!str_set_memberp (a, "!"))               { return 0x10; }

    a = (char **)0;
    a = str_set_add (a, "!");
    a = str_set_add (a, "hello");
    a = str_set_add (a, "world");

    if (str_set_difference (c, c) != (char **)0) { return 0x11; }
    if (str_set_difference (a, c) != (char **)0) { return 0x12; }

    b = str_set_add (a, "!");

    if (a != b)                                  { return 0x13; }

    a = str_split ("meow: hello world! <3", ' ');

    if (!str_set_memberp (a, "meow:"))           { return 0x14; }
    if (!str_set_memberp (a, "hello"))           { return 0x15; }
    if (str_set_memberp  (a, "world"))           { return 0x16; }
    if (!str_set_memberp (a, "world!"))          { return 0x17; }
    if (str_set_memberp  (a, "!"))               { return 0x18; }
    if (!str_set_memberp (a, "<3"))              { return 0x19; }

    if (str_merge (a, ' ') != str_immutable ("meow: hello world! <3"))
                                                 { return 0x1a; }

    return 0;
}
