/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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

#define MAXKEYNUM 512

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

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 7;
        }
        if (n->key != i) {
            tree_destroy(t);
            return 8;
        }
        if (node_get_value (n) != sentinelvalue(i)) {
            tree_destroy(t);
            return 9;
        }
    }

    n = tree_get_node (t, keys + 1);
    if (n != (struct tree_node *)0) {
        tree_destroy(t);
        return 10;
    }

    /* we do this twice to stress the optimising algo once it's in */

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, i);

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 11;
        }
        if (n->key != i) {
            tree_destroy(t);
            return 12;
        }
        if (node_get_value (n) != sentinelvalue(i)) {
            tree_destroy(t);
            return 13;
        }
    }

    n = tree_get_node (t, keys + 1);
    if (n != (struct tree_node *)0) {
        tree_destroy(t);
        return 14;
    }

    tree_destroy(t);

    return 0;
}

int cmain(void) {
    unsigned int i;

    for (i = 1; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree_value(i);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
