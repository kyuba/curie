/*
 *  tree.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 01/06/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <curie/memory.h>
#include <curie/tree.h>
#include <curie/immutable.h>

static struct memory_pool tree_root_pool = MEMORY_POOL_INITIALISER(sizeof (struct tree));
static struct memory_pool tree_node_pool = MEMORY_POOL_INITIALISER(sizeof (struct tree_node));
static struct memory_pool tree_node_pointer_pool = MEMORY_POOL_INITIALISER(sizeof (struct tree_node_pointer));

struct tree * tree_create () {
    struct tree *tree;

    tree = (struct tree *)get_pool_mem(&tree_root_pool);

    tree->root = (struct tree_node *)0;

    return tree;
}

static void node_destroy(struct tree_node *node)
{
    if (node != (struct tree_node *)0) {
        node_destroy ((void *)node->left);
        node_destroy ((void *)node->right);

        free_pool_mem((void *)node);
    }
}

void tree_destroy (struct tree *tree) {
    /* TODO: clean all nodes in 'ere */

    node_destroy (tree->root);

    free_pool_mem((void *)tree);
}

static void tree_add_node_to_tree (struct tree *tree, struct tree_node *node, int_pointer key) {
    struct tree_node *cur = tree->root, *last = (struct tree_node *)0;

    node->key = key;
    node->left = (struct tree_node *)0;
    node->right = (struct tree_node *)0;

    if (tree->root == (struct tree_node *)0) {
        tree->root = node;

        return;
    }

    for (; cur != (struct tree_node *)0; ) {
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

void tree_add_node (struct tree *tree, int_pointer key) {
    struct tree_node *node = (struct tree_node *)get_pool_mem(&tree_node_pool);

    tree_add_node_to_tree (tree, node, key);
}

void tree_add_node_value (struct tree *tree, int_pointer key, const void *value) {
    struct tree_node_pointer *node = (struct tree_node_pointer *)get_pool_mem(&tree_node_pointer_pool);

    node->value = value;

    tree_add_node_to_tree (tree, (struct tree_node *)node, key);
}

struct tree_node * tree_get_node (struct tree *tree, int_pointer key) {
    struct tree_node *cur = tree->root;

    while (cur != (struct tree_node *)0) {
        if (cur->key == key) break;

        if (key > cur->key) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }

    return cur;
}

static void node_rotate (struct tree_node **root, struct tree_node *old, struct tree_node *new) {
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

void tree_remove_node_specific (struct tree *tree, int_pointer key, struct tree_node *node) {
    struct tree_node *cur = tree->root, *last = last = (struct tree_node *)0;

    while (cur != (struct tree_node *)0) {

        if ((cur->key == key) &&
            ((node == (struct tree_node *)0) ||
             (cur == node))) {

            /* perform tree rotations to make the node a leaf node */
            while ((cur->left != (struct tree_node *)0) ||
                   (cur->right != (struct tree_node *)0)) {

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
    }
}

static void tree_map_worker(struct tree_node *node, void (*callback)(struct tree_node *, void *), void *sv) {
    if (node != (struct tree_node *)0) {
        tree_map_worker ((void *)node->left, callback, sv);
        tree_map_worker ((void *)node->right, callback, sv);

        callback((void *)node, sv);
    }
}

void tree_map (struct tree *tree, void (*callback)(struct tree_node *, void *), void *sv) {
    tree_map_worker ((void *)tree->root, callback, sv);
}

void tree_add_node_string (struct tree *t, char *k)
{
    tree_add_node(t, (int_pointer)str_immutable_unaligned(k));
}

void tree_add_node_string_value (struct tree *t, char *k, const void *v)
{
    tree_add_node_value(t, (int_pointer)str_immutable_unaligned(k), v);
}

struct tree_node * tree_get_node_string (struct tree *t, char *k)
{
    return tree_get_node (t, (int_pointer)str_immutable_unaligned(k));
}

void tree_remove_node_string_specific (struct tree *t, char *k, struct tree_node *v)
{
    tree_remove_node_specific(t, (int_pointer)str_immutable_unaligned(k), v);
}
