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

/*! \brief BST Node
 *
 *  A single node in a binary search tree.
 */
struct tree_node_basic {
    /*! \brief Node Key
     *
     *  This is the node's key, or its value.
     */
    int_pointer key;

    /*! \brief Left Branch
     *
     *  This points to the next node to the left of the current node.
     */
    struct tree_node_basic * left;

    /*! \brief Right Branch
     *
     *  This points to the next node to the right of the current node.
     */
    struct tree_node_basic * right;
};

/*! \brief BST Node with Value
 *  \internal
 *
 *  A single node in a binary search tree, with an extra value attached to it.
 */
struct tree_node_basic_pointer {
    /*! \brief Node Key
     *
     *  This is the node's key, for lookups.
     */
    int_pointer key;

    /*! \brief Left Branch
     *
     *  This points to the next node to the left of the current node.
     */
    struct tree_node_basic * left;

    /*! \brief Right Branch
     *
     *  This points to the next node to the right of the current node.
     */
    struct tree_node_basic * right;

    /*! \brief Node Value
     *
     *  The node's value, or payload.
     */
    void *value;
};

static void node_destroy_fnd
        (struct tree_node_basic *node, void (*fnd)(struct tree_node *, void *),
         void *aux)
{
    struct tree_node_basic *onode;

    while (node != (struct tree_node_basic *)0) {
        onode = node;

        if (node->right != (struct tree_node_basic *)0)
        {
            if (node->left != (struct tree_node_basic *)0)
            {
                node_destroy_fnd (node->left, fnd, aux);
            }

            node = node->right;
        }
        else
        {
            node = node->left;
        }

        if (fnd != (void (*)(struct tree_node *, void *))0)
        {
            fnd ((struct tree_node *)onode, aux);
        }

        free_pool_mem((void *)onode);
    }
}

void tree_destroy (struct tree *tree)
{
    tree_destroy_fnd (tree, 0, 0);
}

void tree_destroy_fnd
        (struct tree *tree, void (*fnd)(struct tree_node *, void *), void *aux)
{
    node_destroy_fnd ((struct tree_node_basic *)tree->root, fnd, aux);

    free_pool_mem((void *)tree);
}

static void tree_map_worker
        (struct tree_node_basic *node,
         void (*callback)(struct tree_node *, void *),
         void *sv)
{
    struct tree_node_basic *onode;

    while (node != (struct tree_node_basic *)0) {
        onode = node;

        if (node->right != (struct tree_node_basic *)0)
        {
            if (node->left != (struct tree_node_basic *)0)
            {
                tree_map_worker (node->left, callback, sv);
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

static void tree_add_node_to_tree
        (struct tree *tree, struct tree_node_basic *node, int_pointer key)
{
    struct tree_node_basic *cur  = (struct tree_node_basic *)tree->root,
                           *last = (struct tree_node_basic *)0;

    node->key = key;
    node->left = (struct tree_node_basic *)0;
    node->right = (struct tree_node_basic *)0;

    if (tree->root == (struct tree_node *)0)
    {
        tree->root = (struct tree_node *)node;

        return;
    }

    while (cur != (struct tree_node_basic *)0)
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
            = MEMORY_POOL_INITIALISER(sizeof (struct tree_node_basic));
    struct tree_node_basic *node
            = (struct tree_node_basic *)get_pool_mem(&pool);

    if (node == (struct tree_node_basic *)0) return;

    tree_add_node_to_tree (tree, node, key);
}

void tree_add_node_value (struct tree *tree, int_pointer key, void *value)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct tree_node_basic_pointer));
    struct tree_node_basic_pointer * node
            = (struct tree_node_basic_pointer *)get_pool_mem(&pool);

    if (node == (struct tree_node_basic_pointer *)0) return;

    node->value = value;

    tree_add_node_to_tree (tree, (struct tree_node_basic *)node, key);
}

struct tree_node * tree_get_node (struct tree *tree, int_pointer key)
{
    struct tree_node_basic *cur = (struct tree_node_basic *)tree->root;

    while (cur != (struct tree_node_basic *)0) {
        if (cur->key == key)
        {
            if ((struct tree_node *)cur != tree->root)
            {
                struct tree_node_basic *r = (struct tree_node_basic*)tree->root;

                if (key > r->key)
                {
                    struct tree_node_basic *rl = r->right->left;

                    tree->root = (struct tree_node *)r->right;

                    r->right->left = r;
                    r->right = rl;
                }
                else
                {
                    struct tree_node_basic *lr = r->left->right;

                    tree->root = (struct tree_node *)r->left;

                    r->left->right = r;
                    r->left = lr;
                }
            }

            return (struct tree_node *)cur;
        }

        if (key > cur->key) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }

    return (struct tree_node *)0;
}

static void node_rotate
        (struct tree_node_basic **root,
         struct tree_node_basic *old, struct tree_node_basic *new)
{
    if (old->left == new) {
        old->left = new->right;
        new->right = old;
    } else {
        old->right = new->left;
        new->left = old;
    }

    if (root != (struct tree_node_basic **)0) {
        *root = new;
    }
}

void tree_remove_node_specific
        (struct tree *tree, int_pointer key, struct tree_node *node)
{
    struct tree_node_basic
            *cur  = (struct tree_node_basic *)tree->root,
            *last = (struct tree_node_basic *)0;

    while (cur != (struct tree_node_basic *)0) {
        if ((cur->key == key) &&
            ((node == (struct tree_node *)0) ||
             (cur  == (struct tree_node_basic *)node)))
        {
            /* perform tree rotations to make the node a leaf node */
            while ((cur->left  != (struct tree_node_basic *)0) ||
                   (cur->right != (struct tree_node_basic *)0))
            {
                struct tree_node_basic **update, *new_last;

                if (last == (struct tree_node_basic *)0)
                {
                    struct tree_node **update_temp = &(tree->root);
                    update = (struct tree_node_basic **)update_temp;
                }
                else
                {
                    if (last->left == cur)
                    {
                        update = &(last->left);
                    }
                    else
                    {
                        update = &(last->right);
                    }
                }

                if (cur->left != (struct tree_node_basic *)0)
                {
                    new_last = cur->left;
                }
                else
                {
                    new_last = cur->right;
                }

                node_rotate (update, cur, new_last);
                last = new_last;
            }

            /* isolate the node by disconnecting it */
            if (last == (struct tree_node_basic *)0) {
                tree->root = (struct tree_node *)0;
            } else {
                if (last->left == cur) {
                    last->left = (struct tree_node_basic *)0;
                } else {
                    last->right = (struct tree_node_basic *)0;
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

void *node_get_value
        (struct tree_node *node)
{
    return ((struct tree_node_basic_pointer *)node)->value;
}
