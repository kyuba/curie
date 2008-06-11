/*
 *  tree.c
 *  atomic-libc
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

#include <atomic/memory.h>
#include <atomic/tree.h>

/*@-branchstate@*/
/*@-nullpass@*/
/*@-mustfreeonly@*/
/*@-onlytrans@*/
/*@-nullderef@*/
/* the usual memory management woes again... */

/*@null@*/ struct memory_pool *tree_root_pool = 0;
/*@null@*/ struct memory_pool *tree_node_pool = 0;
/*@null@*/ struct memory_pool *tree_node_pointer_pool = 0;

struct tree * tree_create () {
    struct tree *tree;

    if (tree_root_pool == 0) {
        tree_root_pool = create_memory_pool (sizeof (struct tree));
        tree_node_pool = create_memory_pool (sizeof (struct tree_node));
        tree_node_pointer_pool = create_memory_pool (sizeof (struct tree_node_pointer));
    }

    tree = (struct tree *)get_pool_mem(tree_root_pool);

    tree->root = (struct tree_node *)0;

    return tree;
}

static void node_destroy(/*@only@*/ struct tree_node *node)
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

    optimise_memory_pool (tree_root_pool);
    optimise_memory_pool (tree_node_pool);
    optimise_memory_pool (tree_node_pointer_pool);
}

static void tree_add_node_to_tree (/*@shared@*/ struct tree *tree, /*@only@*/ struct tree_node *node, unsigned long key) {
    node->key = key;

    /* we'll be implementing a self-optimising tree, so we can use dirty cheap
       inserts */

    if ((tree->root != (struct tree_node *)0) &&
        (tree->root->key > node->key)) {
        node->left = (struct tree_node *)0;
        node->right = tree->root;
    } else {
        node->left = tree->root;
        node->right = (struct tree_node *)0;
    }

    tree->root = node;
}

void tree_add_node (struct tree *tree, unsigned long key) {
    struct tree_node *node = (struct tree_node *)get_pool_mem(tree_node_pool);

    tree_add_node_to_tree (tree, node, key);
}

void tree_add_node_value (struct tree *tree, unsigned long key, void *value) {
    struct tree_node_pointer *node = (struct tree_node_pointer *)get_pool_mem(tree_node_pointer_pool);

    node->value = value;

    tree_add_node_to_tree (tree, (struct tree_node *)node, key);
}

struct tree_node * tree_get_node (struct tree *tree, unsigned long key) {
    struct tree_node *cur;

    for (cur = tree->root; cur != (struct tree_node *)0; ) {
        if (cur->key == key) break;

        if (key > cur->key) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }

    return cur;
}
