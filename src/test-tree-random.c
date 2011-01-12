/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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

#include "curie/tree.h"
#include "curie/io.h"

#define MAXKEYNUM 512

static unsigned int test_tree_random(unsigned int keys) {
    struct tree *t = tree_create ();
    unsigned int i;
    struct tree_node *n;
    struct io *r = io_open_read ("/dev/urandom");
    unsigned int *keyarray;

    while (r->length < (unsigned int)(keys * sizeof(unsigned int))) {
        if (io_read(r) == io_unrecoverable_error) {
            io_close (r);
            tree_destroy(t);
            return 1;
        }
    }

    keyarray = (unsigned int *)(r->buffer);

    for (i = 0; i < keys; i++) {
        tree_add_node (t, keyarray[i]);
    }

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, keyarray[i]);

        if (n == (struct tree_node *)0) {
            io_close (r);
            tree_destroy(t);
            return 2;
        }
        if (n->key != keyarray[i]) {
            io_close (r);
            tree_destroy(t);
            return 3;
        }
    }

    /* we do this twice to stress the optimising algo once it's in */

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, keyarray[i]);

        if (n == (struct tree_node *)0) {
            io_close (r);
            tree_destroy(t);
            return 5;
        }
        if (n->key != keyarray[i]) {
            io_close (r);
            tree_destroy(t);
            return 6;
        }
    }

    io_close (r);
    tree_destroy(t);

    return 0;
}

int cmain(void) {
    unsigned int i;

    for (i = 1; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree_random(i);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
