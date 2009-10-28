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
#include <curie/immutable.h>

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

/*! \todo clean all nodes in 'ere */
void tree_destroy (struct tree *tree)
{
    if (tree->root != (struct tree_node *)0)
    {
        node_destroy (tree->root);
    }

    free_pool_mem((void *)tree);
}

static void tree_add_node_to_tree
        (struct tree *tree, struct tree_node *node, int_pointer key)
{
    struct tree_node *cur = tree->root, *last = (struct tree_node *)0;

    node->key = key;
    node->left = (struct tree_node *)0;
    node->right = (struct tree_node *)0;

    if (tree->root == (struct tree_node *)0)
    {
        tree->root = node;

        return;
    }

    while (cur != (struct tree_node *)0)
    {
        last = cur;
        if (key > cur->key) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }

    if (key > last->key) {
        last->right = node;
    } else {
        last->left = node;
    }
}

void tree_add_node (struct tree *tree, int_pointer key)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct tree_node));
    struct tree_node *node = (struct tree_node *)get_pool_mem(&pool);

    if (node == (struct tree_node *)0) return;

    tree_add_node_to_tree (tree, node, key);
}

void tree_add_node_value (struct tree *tree, int_pointer key, void *value)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct tree_node_pointer));
    struct tree_node_pointer * node
            = (struct tree_node_pointer *)get_pool_mem(&pool);

    if (node == (struct tree_node_pointer *)0) return;

    node->value = value;

    tree_add_node_to_tree (tree, (struct tree_node *)node, key);
}

struct tree_node * tree_get_node (struct tree *tree, int_pointer key)
{
    struct tree_node *cur = tree->root;

    while (cur != (struct tree_node *)0) {
        if (cur->key == key) return cur;

        if (key > cur->key) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }

    return (struct tree_node *)0;
}

static void node_rotate
        (struct tree_node **root, struct tree_node *old, struct tree_node *new)
{
    if (old->left == new) {
        old->left = new->right;
        new->right = old;
    } else {
        old->right = new->left;
        new->left = old;
    }

    if (root != (struct tree_node **)0) {
        *root = new;
    }
}

void tree_remove_node_specific
        (struct tree *tree, int_pointer key, struct tree_node *node)
{
    struct tree_node
            *cur = tree->root,
            *last = (struct tree_node *)0;

    while (cur != (struct tree_node *)0) {
        if ((cur->key == key) &&
            ((node == (struct tree_node *)0) || (cur == node)))
        {
            /* perform tree rotations to make the node a leaf node */
            while ((cur->left != (struct tree_node *)0) ||
                   (cur->right != (struct tree_node *)0))
            {
                struct tree_node **update, *new_last;

                if (last == (struct tree_node *)0) {
                    update = &(tree->root);
                } else {
                    if (last->left == cur) {
                        update = &(last->left);
                    } else {
                        update = &(last->right);
                    }
                }

                if (cur->left != (struct tree_node *)0) {
                    new_last = cur->left;
                } else {
                    new_last = cur->right;
                }

                node_rotate (update, cur, new_last);
                last = new_last;
            }

            /* isolate the node by disconnecting it */
            if (last == (struct tree_node *)0) {
                tree->root = (struct tree_node *)0;
            } else {
                if (last->left == cur) {
                    last->left = (struct tree_node *)0;
                } else {
                    last->right = (struct tree_node *)0;
                }
            }

            /* release the node's memory back into the pool */
            free_pool_mem((void *)cur);
            return;
        }

        last = cur;

        if (key > cur->key) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    };
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
