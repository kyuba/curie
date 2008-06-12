/*
 *  tree-random-node-removal.c
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

#define MAXKEYNUM 4096

/*@-branchstate@*/
/*@-sharedtrans@*/

unsigned int random_numbers[MAXKEYNUM];

static unsigned int have_number_before_slot (unsigned int n, unsigned int s) {
    unsigned int i;

    for (i = 0; i < s; i++) {
        if (random_numbers[i] == n) return 1;
    }

    return 0;
}

static void assign_number_to_slot (unsigned int n, unsigned int s) {
    while (have_number_before_slot(n, s) != 0) {
        n++;
    }

    random_numbers[s] = n;
}

static unsigned int populate_random_numbers_array() {
    unsigned int i;

    struct io *r = io_open_read ("/dev/urandom");
    while (r->length < (unsigned int)(MAXKEYNUM * sizeof(unsigned int))) {
        if (io_read(r) == io_unrecoverable_error) {
            io_close (r);
            return 1;
        }
    }

    for (i = 0; i < MAXKEYNUM; i++) {
        assign_number_to_slot (((unsigned int *)(r->buffer))[i], i);
    }

    io_close (r);

    return 0;
}

static unsigned int test_tree_random_node_removal(unsigned int keys) {
    struct tree *t = tree_create ();
    unsigned int i;
    struct tree_node *n;

    i = populate_random_numbers_array();
    if (i != 0) {
        tree_destroy(t);
        return i;
    }

    for (i = 0; i < keys; i++) {
        tree_add_node (t, random_numbers[i]);
    }

    /* remove half the nodes */
    for (i = 0; i < keys; i+=2) {
        tree_remove_node (t, random_numbers[i]);
    }

    for (i = 1; i < keys; i+=2) {
        n = tree_get_node (t, random_numbers[i]);

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 2;
        }
        if (n->key != random_numbers[i]) {
            tree_destroy(t);
            return 3;
        }
    }

    /* search for the nodes that should be missing */
    for (i = 0; i < keys; i+=2) {
        n = tree_get_node (t, random_numbers[i]);

        if (n != (struct tree_node *)0) return 4;
    }

    /* we do this twice to stress the optimising algo once it's in */
    for (i = 1; i < keys; i+=2) {
        n = tree_get_node (t, random_numbers[i]);

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 5;
        }
        if (n->key != random_numbers[i]) {
            tree_destroy(t);
            return 6;
        }
    }

    /* search for the nodes that should be missing */
    for (i = 0; i < keys; i+=2) {
        n = tree_get_node (t, random_numbers[i]);

        if (n != (struct tree_node *)0) return 7;
    }

    /* remove the remaining nodes */
    for (i = 1; i < keys; i+=2) {
        tree_remove_node (t, random_numbers[i]);
    }

    if (t->root != (struct tree_node *)0) {
        tree_destroy(t);
        return 8;
    }

    tree_destroy(t);

    return 0;
}

int atomic_main(void) {
    unsigned int i;

    for (i = 2; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree_random_node_removal(i);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
