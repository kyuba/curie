/*
 *  tree.h
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

#ifndef ATOMIC_TREE_H
#define ATOMIC_TREE_H

struct tree {
    struct tree_node * root;
};

struct tree_node {
    /*@null@*/ struct tree_node * left;
    /*@null@*/ struct tree_node * right;

    unsigned long key;
};

struct tree_node_pointer {
    /*@null@*/ struct tree_node * left;
    /*@null@*/ struct tree_node * right;

    unsigned long key;

    void *value;
};

struct tree * tree_create ();
void tree_destroy (struct tree *);

void tree_add_node (struct tree *, unsigned long);
void tree_add_node_value (struct tree *, unsigned long, void *);

struct tree_node * tree_get_node (struct tree *, unsigned long);

void tree_remove_node (struct tree *, unsigned long);
void tree_remove_node_specific (struct tree *, unsigned long, struct tree_node *);

#define node_get_value(node) ((struct tree_node_pointer *)node)->value

void tree_map (struct tree *, void (*)(struct tree_node *, void *), void *);

#endif
