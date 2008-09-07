/*
 *  tree.h
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

/*! \file
 *  \brief Basic BSTs
 *
 *  Binary search trees are used by parts of the library code itself, and they
 *  are generally needed by a good deal of applications. The functions from this
 *  header file implement a small subset of the useful functions on BSTs.
 */

#ifndef LIBCURIE_TREE_H
#define LIBCURIE_TREE_H

#include <curie/int.h>

struct tree {
    /*@null@*/ /*@owned@*/ struct tree_node * root;
};

struct tree_node {
    /*@null@*/ /*@owned@*/ struct tree_node * left;
    /*@null@*/ /*@owned@*/ struct tree_node * right;

    int_pointer key;
};

struct tree_node_pointer {
    /*@null@*/ /*@owned@*/ struct tree_node * left;
    /*@null@*/ /*@owned@*/ struct tree_node * right;

    int_pointer key;

    /*@dependent@*/ const void *value;
};

#define TREE_INITIALISER { .root = (struct tree_node *)0 }

/*@notnull@*/ /*@only@*/ struct tree * tree_create ();
void tree_destroy (/*@notnull@*/ /*@only@*/ struct tree *);

void tree_add_node (struct tree *, int_pointer);
void tree_add_node_value (struct tree *, int_pointer, /*@dependent@*/ const void *);

/*@null@*/ /*@dependent@*/ struct tree_node * tree_get_node (/*@notnull@*/ struct tree *, int_pointer);

void tree_remove_node_specific (/*@dependent@*/ struct tree *, int_pointer, /*@null@*/ struct tree_node *);

#define tree_remove_node(t,k) tree_remove_node_specific(t, k, (struct tree_node *)0)
#define node_get_value(node) ((struct tree_node_pointer *)node)->value

void tree_map (struct tree *, void (*)(struct tree_node *, void *), /*@null@*/ void *);

#endif
