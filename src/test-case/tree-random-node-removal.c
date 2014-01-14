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
#include "curie/io.h"

#define MAXKEYNUM 512

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

        if (n != (struct tree_node *)0) {
            tree_destroy(t);
            return 4;
        }
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

        if (n != (struct tree_node *)0) {
            tree_destroy(t);
            return 7;
        }
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

int cmain(void) {
    unsigned int i;

    for (i = 2; i < MAXKEYNUM; i++) {
        unsigned int rv;

        rv = test_tree_random_node_removal(i);
        if (rv != 0) return (int)rv;
    }

    return 0;
}
