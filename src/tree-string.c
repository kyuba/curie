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

#include <curie/tree.h>
#include <curie/immutable.h>

void tree_add_node_string (struct tree *t, char *k)
{
    tree_add_node(t, (int_pointer)str_immutable_unaligned(k));
}

void tree_add_node_string_value (struct tree *t, char *k, void *v)
{
    tree_add_node_value(t, (int_pointer)str_immutable_unaligned(k), v);
}

struct tree_node * tree_get_node_string (struct tree *t, char *k)
{
    return tree_get_node (t, (int_pointer)str_immutable_unaligned(k));
}

void tree_remove_node_string_specific
        (struct tree *t, char *k, struct tree_node *v)
{
    tree_remove_node_specific(t, (int_pointer)str_immutable_unaligned(k), v);
}
