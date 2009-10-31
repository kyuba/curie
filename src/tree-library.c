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

#include <curie/memory.h>
#include <curie/tree.h>

struct tree * tree_create ()
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct tree));
    struct tree *tree = (struct tree *)get_pool_mem(&pool);

    if (tree == (struct tree *)0)
    {
        return (struct tree *)0;
    }

    tree->root = (struct tree_node *)0;

    return tree;
}

static void node_destroy (struct tree_node *node)
{
    if (node != (struct tree_node *)0) {
        if (node->left != (struct tree_node *)0)
        {
            node_destroy (node->left);
        }
        if (node->right != (struct tree_node *)0)
        {
            node_destroy (node->right);
        }

        free_pool_mem((void *)node);
    }
}

void tree_destroy (struct tree *tree)
{
    if (tree->root != (struct tree_node *)0)
    {
        node_destroy (tree->root);
    }

    free_pool_mem((void *)tree);
}

static void node_destroy_fnd
        (struct tree_node *node, void (*fnd)(struct tree_node *, void *),
         void *aux)
{
    fnd (node, aux);

    if (node != (struct tree_node *)0) {
        if (node->left != (struct tree_node *)0)
        {
            node_destroy_fnd (node->left, fnd, aux);
        }
        if (node->right != (struct tree_node *)0)
        {
            node_destroy_fnd (node->right, fnd, aux);
        }

        free_pool_mem((void *)node);
    }
}

void tree_destroy_fnd
        (struct tree *tree, void (*fnd)(struct tree_node *, void *), void *aux)
{
    if (tree->root != (struct tree_node *)0)
    {
        node_destroy_fnd (tree->root, fnd, aux);
    }

    free_pool_mem((void *)tree);
}

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
