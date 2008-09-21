/*
 *  immutable.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 10/06/2008.
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

    tree_add_node_value (&immutable_string_hashes, hash, (const void *)rv);

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
