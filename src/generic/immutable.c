/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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

#include <curie/immutable.h>
#include <curie/memory.h>
#include <curie/tree.h>
#include <curie/string.h>
#include <curie/int.h>

#define IMMUTABLE_CHUNKSIZE (4096*2)

/*@null@*/ /*@owned@*/ static char *immutable_data = (char *)0;
/*@null@*/ /*@dependent@*/ static char *immutable_cursor = (char *)0;
static unsigned long immutable_data_size = 0;
static unsigned long immutable_data_space_left = 0;

static struct tree immutable_strings = TREE_INITIALISER;
static struct tree immutable_string_hashes = TREE_INITIALISER;

const char *str_immutable_unaligned (const char * string) {
    unsigned int length;

    for (length = 0; string[length] != (char)0; length++);
    length++;

    if (((length % 8) == 0) && ((((int_pointer)string) % 8) == 0)) {
        /* must be suitably aligned */

        return str_immutable (string);
    } else {
        const char *rv;
        char *r;
        unsigned int i;

        if ((r = get_mem (length)) == (char *)0)
        /* the return value of this is always suitable for our purposes. */
        {
            return (void *)0;
        }

        for (i = 0; string[i] != (char)0; i++) r[i] = string[i];
        do {
            r[i] = (char)0;
            i++;
        } while (i < (length + (8-(length % 8))));

        rv = str_immutable (r);

        /*@-usereleased@*/
        free_mem (length, r);

        return rv;
        /*@=usereleased@*/
    }
}

const char *str_immutable (const char * string) {
    unsigned long stringlength = 0;
    int_pointer hash;
    /*@observer@*/ const char *rv;
    struct tree_node *n;

    /* the compiler should put static strings into read-only storage... */
    if (string[0] == (char)0) return (const char *)"";

    if (tree_get_node (&immutable_strings, (int_pointer)string) != (struct tree_node *)0) {
        return string;
    }

    hash = (int_pointer)str_hash (string, &stringlength);

    stringlength++; /* add an extra character for the terminating 0 */

    if ((n = tree_get_node (&immutable_string_hashes, hash))
         != (struct tree_node *)0)
    {
        return (const char *)node_get_value (n);
    }

    if ((rv = immutable (string, stringlength)) == (char *)0)
    {
        return (char *)0;
    }

    tree_add_node (&immutable_strings, (int_pointer)rv);

    tree_add_node_value (&immutable_string_hashes, hash, (void *)rv);

    return rv;
}

const void *immutable ( const void * data, unsigned long length ) {
    const char *rv;
    const char *data_char = (const char *)data;

    if (length > immutable_data_space_left) {
        unsigned long new_size = IMMUTABLE_CHUNKSIZE;
        lock_immutable_pages();

        if (length > IMMUTABLE_CHUNKSIZE) {
            new_size = ((length / IMMUTABLE_CHUNKSIZE) +
                         (((length % IMMUTABLE_CHUNKSIZE) != 0) ? 1 : 0))
                       * IMMUTABLE_CHUNKSIZE;
        }

        if (immutable_data != (void *)0)
        {
            lock_immutable_pages();
        }

        /*@-mustfree*/
        if ((immutable_data = get_mem(new_size)) == (void *)0)
        {
            /*@=mustfree*/
            return (char *)0;
        }

        immutable_data_space_left = new_size;
        immutable_cursor = immutable_data;
        immutable_data_size = new_size;
    }

    rv = immutable_cursor;

    for (; length != 0;
         immutable_cursor++,
         data_char++,
         length--,
         immutable_data_space_left--)
    {
         /*@-nullderef@*/ /* can't be null here */
         *immutable_cursor = *data_char;
         /*@=nullderef@*/
    }

    return rv;
}

void lock_immutable_pages ( void ) {
    if (immutable_data_size != 0) {
        /* not null here... */
        /*@-nullpass@*/
        mark_mem_ro (immutable_data_size, immutable_data);
        /*@=nullpass@*/
    }

    immutable_data_size = 0;
    immutable_data_space_left = 0;
}
