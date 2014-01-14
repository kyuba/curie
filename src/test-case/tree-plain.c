/**\file
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
*/

#include "curie/tree.h"

#define MAXKEYNUM 512

static unsigned int test_tree(unsigned int keys) {
    struct tree *t = tree_create ();
    unsigned int i;
    struct tree_node *n;

    for (i = 0; i < keys; i++) {
        tree_add_node (t, i);
    }

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, i);

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 1;
        }
        if (n->key != i) {
            tree_destroy(t);
            return 2;
        }
    }

    n = tree_get_node (t, keys + 1);
    if (n != (struct tree_node *)0) {
        tree_destroy(t);
        return 3;
    }

    /* we do this twice to stress the optimising algo once it's in */

    for (i = 0; i < keys; i++) {
        n = tree_get_node (t, i);

        if (n == (struct tree_node *)0) {
            tree_destroy(t);
            return 4;
        }
        if (n->key != i) {
            tree_destroy(t);
            return 5;
        }
    }

    n = tree_get_node (t, keys + 1);
    if (n != (struct tree_node *)0) {
        tree_destroy(t);
        return 6;
    }

    tree_destroy(t);

    return 0;
}

int cmain(void) {
    unsigned int i;

    for (i = 1; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree(i);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
