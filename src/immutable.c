/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#include <sievert/immutable.h>
#include <curie/memory.h>
#include <curie/tree.h>
#include <curie/hash.h>
#include <curie/int.h>

#define IMMUTABLE_CHUNKSIZE (4096*2)

static char *immutable_data = (char *)0;
static char *immutable_cursor = (char *)0;
static unsigned long immutable_data_size = 0;
static unsigned long immutable_data_space_left = 0;

static struct tree immutable_data_tree = TREE_INITIALISER;
static struct tree immutable_hashes = TREE_INITIALISER;

const char *str_immutable (const char * string)
{
    unsigned long stringlength = 0;

    /* the compiler should put static strings into read-only storage... */
    if (string[0] == (char)0) return (const char *)"";

    while (string[stringlength] != 0)
    {
        stringlength++;
    }

    return (const char *)immutable (string, stringlength);
}

const void *immutable (const void * data, unsigned long length)
{
    const char *rv;
    const char *data_char = (const char *)data;
    struct tree_node *n;
    int_pointer hash = hash_murmur2_pt (&data, sizeof (const void *), 0);

    if (tree_get_node (&immutable_data_tree, hash)
        != (struct tree_node *)0)
    {
        return data;
    }

    hash = hash_murmur2_pt (data, length, 0);

    if ((n = tree_get_node (&immutable_hashes, hash))
        != (struct tree_node *)0)
    {
        return (const void *)node_get_value (n);
    }

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

        immutable_data = get_mem(new_size);

        immutable_data_space_left = new_size;
        immutable_cursor = immutable_data;
        immutable_data_size = new_size;
    }

    for (rv = immutable_cursor; length != 0;
         immutable_cursor++,
         data_char++,
         length--,
         immutable_data_space_left--)
    {
         *immutable_cursor = *data_char;
    }

    *immutable_cursor = 0; /* write an extra 0 after whatever we just wrote */
    immutable_data_space_left--;

    while ((((unsigned long)immutable_cursor) % sizeof(void *)) != 0)
    {
        *immutable_cursor = 0;
        immutable_cursor++;
        immutable_data_space_left--;
    }

    tree_add_node_value (&immutable_hashes, hash, (void *)rv);

    hash = hash_murmur2_pt (&rv, sizeof (const void *), 0);

    tree_add_node (&immutable_data_tree, hash);

    return rv;
}

void lock_immutable_pages ( void )
{
    if (immutable_data_size != 0) {
        /* not null here... */
        mark_mem_ro (immutable_data_size, immutable_data);
    }

    immutable_data_size = 0;
    immutable_data_space_left = 0;
}
