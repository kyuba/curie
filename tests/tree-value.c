/*
 *  tree-value.c
 *  atomic-libc
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

#include "atomic/tree.h"

#define MAXKEYNUM 2048

/*@-branchstate@*/
/*@-sharedtrans@*/

char *sentinelbase = (char*)0x8916316;
#define sentinelvalue(i) (void *)(sentinelbase + i)

static unsigned int test_tree_value(unsigned int keys) {
    struct tree *t = tree_create ();
    unsigned int i;
    struct tree_node *n;

    for (i = 0; i < keys; i++) {
        tree_add_node_value (t, i, sentinelvalue(i));
    }

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, i);

        if (n == (struct tree_node *)0) return 7;
        if (n->key != i) return 8;
        if (node_get_value (n) != sentinelvalue(i))
            return 9;
    }

    n = tree_get_node (t, keys + 1);
    if (n != (struct tree_node *)0) return 10;

    /* we do this twice to stress the optimising algo once it's in */

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, i);

        if (n == (struct tree_node *)0) return 11;
        if (n->key != i) return 12;
        if (node_get_value (n) != sentinelvalue(i))
            return 13;
    }

    n = tree_get_node (t, keys + 1);
    if (n != (struct tree_node *)0) return 14;

    tree_destroy(t);

    return 0;
}

int atomic_main(void) {
    unsigned int i;

    for (i = 1; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree_value(i);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
