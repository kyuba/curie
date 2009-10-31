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

#include <curie/tree.h>

static void tree_map_worker
        (struct tree_node *node, void (*callback)(struct tree_node *, void *),
         void *sv)
{
    struct tree_node *onode;

    while (node != (struct tree_node *)0) {
        onode = node;

        if (node->right != (struct tree_node *)0)
        {
            if (node->left != (struct tree_node *)0)
            {
                tree_map_worker ((void *)node->left, callback, sv);
            }

            node = node->right;
        }
        else
        {
            node = node->left;
        }

        callback((void *)onode, sv);
    }
}

void tree_map
        (struct tree *tree, void (*callback)(struct tree_node *, void *),
         void *sv)
{
    if (tree->root != (struct tree_node *)0)
        tree_map_worker ((void *)tree->root, callback, sv);
}
