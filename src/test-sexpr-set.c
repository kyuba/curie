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
#include <sievert/sexpr.h>

define_string (str_meow_colon, "meow:");
define_string (str_hello,      "hello");
define_string (str_world,      "world");
define_string (str_space,      " ");
define_string (str_bang,       "!");
define_string (str_heart,      "<3");
define_string (str_world_bang, "world!");
define_string (str_teststring, "meow: hello world! <3");

int cmain()
{
    sexpr a = sx_end_of_list, b = sx_end_of_list, c = sx_end_of_list;

    a = sx_set_add (a, str_hello);
    a = sx_set_add (a, str_world);

    if (falsep(sx_set_memberp (a, str_hello)))      { return 0x01; }
    if (falsep(sx_set_memberp (a, str_world)))      { return 0x02; }
    if (truep(sx_set_memberp  (b, str_hello)))      { return 0x03; }

    b = sx_set_add (b, str_bang);

    if (falsep(sx_set_memberp (b, str_bang)))       { return 0x04; }

    c = sx_set_merge (a, b);

    if (falsep(sx_set_memberp (c, str_hello)))      { return 0x05; }
    if (falsep(sx_set_memberp (c, str_world)))      { return 0x06; }
    if (falsep(sx_set_memberp (c, str_bang)))       { return 0x07; }

    a = sx_set_remove (a, str_hello);

    if (truep(sx_set_memberp  (a, str_hello)))      { return 0x08; }
    if (falsep(sx_set_memberp (a, str_world)))      { return 0x09; }
    if (truep(sx_set_memberp  (a, str_bang)))       { return 0x0a; }

    a = sx_set_intersect (a, c);

    if (truep(sx_set_memberp  (a, str_hello)))      { return 0x0b; }
    if (falsep(sx_set_memberp (a, str_world)))      { return 0x0c; }
    if (truep(sx_set_memberp  (a, str_bang)))       { return 0x0d; }

    a = sx_set_difference (a, c);

    if (falsep(sx_set_memberp (a, str_hello)))      { return 0x0e; }
    if (truep(sx_set_memberp  (a, str_world)))      { return 0x0f; }
    if (falsep(sx_set_memberp (a, str_bang)))       { return 0x10; }

    a = sx_end_of_list;
    a = sx_set_add (a, str_bang);
    a = sx_set_add (a, str_hello);
    a = sx_set_add (a, str_world);

    if (!eolp(sx_set_difference (c, c)))            { return 0x11; }
    if (!eolp(sx_set_difference (a, c)))            { return 0x12; }

    b = sx_set_add (a, str_bang);

    if (falsep (equalp (a, b)))                     { return 0x13; }

    a = sx_split (str_teststring, str_space);

    if (falsep(sx_set_memberp (a, str_meow_colon))) { return 0x14; }
    if (falsep(sx_set_memberp (a, str_hello)))      { return 0x15; }
    if (truep(sx_set_memberp  (a, str_world)))      { return 0x16; }
    if (falsep(sx_set_memberp (a, str_world_bang))) { return 0x17; }
    if (truep(sx_set_memberp  (a, str_bang)))       { return 0x18; }
    if (falsep(sx_set_memberp (a, str_heart)))      { return 0x19; }

    a = sx_reverse (a);

    if (falsep(equalp(sx_merge (a, str_space), str_teststring)))
                                                    { return 0x1a; }

    return 0;
}
