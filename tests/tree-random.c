/*
 *  tree-random.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 08/06/2008.
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
#include "atomic/io.h"

#define MAXKEYNUM 2048

/*@-branchstate@*/
/*@-sharedtrans@*/

static unsigned int test_tree_random(unsigned int keys) {
    struct tree *t = tree_create ();
    unsigned int i;
    struct tree_node *n;
    struct io *r = io_open_read ("/dev/urandom");
    unsigned int *keyarray;

    while (r->length < (keys * sizeof(unsigned int))) {
        if (io_read(r) == io_unrecoverable_error) return 1;
    }

    keyarray = (unsigned int *)(r->buffer);

    for (i = 0; i < keys; i++) {
        tree_add_node (t, keyarray[i]);
    }

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, keyarray[i]);

        if (n == (struct tree_node *)0) return 2;
        if (n->key != keyarray[i]) return 3;
    }

    n = tree_get_node (t, keys + keyarray[i]);
    if (n != (struct tree_node *)0) return 4;

    /* we do this twice to stress the optimising algo once it's in */

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, keyarray[i]);

        if (n == (struct tree_node *)0) return 5;
        if (n->key != i) return 6;
    }

    tree_destroy(t);

    return 0;
}

int atomic_main(void) {
    unsigned int i;

    for (i = 1; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree_random(i);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
