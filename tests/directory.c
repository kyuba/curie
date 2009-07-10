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

#include <curie/main.h>
#include <curie/memory.h>
#include <curie/directory.h>

define_string (str_memory_pool,       "./tests/memory-pool.c");
define_string (str_memory_primitives, "./tests/memory-primitives.c");
define_string (str_memory_allocator,  "./tests/memory-allocator.c");

sexpr rd_fold (sexpr e, sexpr s)
{
    int i = sx_integer (s);

    if (truep(equalp(e, str_memory_pool))) i++;
    else if (truep(equalp(e, str_memory_primitives))) i++;
    else if (truep(equalp(e, str_memory_allocator))) i++;

    return make_integer (i);
}

int cmain()
{
    sexpr l = read_directory ("tests/memory-.*\\.c");

    int rv = truep(equalp(sx_list_fold(l, rd_fold, make_integer(0)),
                   make_integer (3))) ? 0 : 1;

    optimise_static_memory_pools();

    return rv;
}
