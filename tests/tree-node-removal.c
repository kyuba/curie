/*
 *  tree-node-removal.c
 *  libcurie
 *
 *  Split from tests/tree.c by Magnus Deininger on 12/06/2008.
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

#include "curie/tree.h"

#define MAXKEYNUM 512

char *sentinelbase = (char*)0x8916316;
#define sentinelvalue(i) (void *)(sentinelbase + i)

static unsigned int test_tree_node_removal(unsigned int keys) {
    struct tree *t = tree_create ();
    unsigned int i;
    struct tree_node *n;

    for (i = 0; i < keys; i++) {
        tree_add_node_value (t, i, sentinelvalue(i));
    }

    /* remove half the nodes */
    for (i = 0; i < keys; i+=2) {
        tree_remove_node (t, i);
    }

    /* search for the nodes that should still be present */
    for (i = 1; i < keys; i+=2) {
        n = tree_get_node (t, i);

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 15;
        }
        if (n->key != i) {
            tree_destroy(t);
            return 16;
        }
        if (node_get_value (n) != sentinelvalue(i)) {
            tree_destroy(t);
            return 17;
        }
    }

    /* search for the nodes that should be missing */
    for (i = 0; i < keys; i+=2) {
        n = tree_get_node (t, i);

        if (n != (struct tree_node *)0) {
            tree_destroy(t);
            return 18;
        }
    }

    /* search for an arbitrary node that can't be present */
    n = tree_get_node (t, keys + 1);
    if (n != (struct tree_node *)0) {
        tree_destroy(t);
        return 19;
    }

    /* we do the searches twice to stress the optimising algo once it's in */

    /* search for the nodes that should still be present */
    for (i = 1; i < keys; i+=2) {
        n = tree_get_node (t, i);

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 20;
        }
        if (n->key != i) {
            tree_destroy(t);
            return 21;
        }
        if (node_get_value (n) != sentinelvalue(i)) {
            tree_destroy(t);
            return 22;
        }
    }

    /* search for the nodes that should be missing */
    for (i = 0; i < keys; i+=2) {
        n = tree_get_node (t, i);

        if (n != (struct tree_node *)0) {
            tree_destroy(t);
            return 23;
        }
    }

    /* remove the remaining nodes */
    for (i = 1; i < keys; i+=2) {
        tree_remove_node (t, i);
    }

    if (t->root != (struct tree_node *)0) {
        tree_destroy(t);
        return 24;
    }

    tree_destroy(t);

    return 0;
}

int cmain(void) {
    unsigned int i;

    for (i = 1; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree_node_removal(i * 2);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
