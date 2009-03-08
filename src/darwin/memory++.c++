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

#include <curie++/int.h>

#include <curie/memory.h>
#include <curie/tree.h>

using namespace curiepp;

static struct tree size_map = TREE_INITIALISER;

void *operator new(op_new_size length)
{
    void *r = aalloc(length);

    tree_add_node_value (&size_map, (int_pointer)r, (void *)(int_pointer)length);

    return r;
}

void operator delete(void *object)
{
    struct tree_node *node = tree_get_node (&size_map, (int_pointer)object);

    afree((unsigned long)node_get_value(node), object);
    tree_remove_node (&size_map, (int_pointer)object);
}

void *operator new[](op_new_size length)
{
    void *r = aalloc(length);

    tree_add_node_value (&size_map, (int_pointer)r, (void *)(int_pointer)length);

    return r;
}

void operator delete[](void *object)
{
    delete object;
}
