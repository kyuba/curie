/*
 *  immutable.c
 *  atomic-libc
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

#include <atomic/immutable.h>
#include <atomic/memory.h>
#include <atomic/tree.h>

#define IMMUTABLE_CHUNKSIZE (4096*2)

/*@-onlytrans@*/

/*@notnull@*/ static char *immutable_data;
/*@notnull@*/ static char *immutable_cursor;
static unsigned long immutable_data_size = 0;
static unsigned long immutable_data_space_left = 0;

/*@null@*/ /*@owned@*/ static struct tree *immutable_strings = (struct tree *)0;

const char *str_immutable ( const char * string ) {
    unsigned long stringlength = 0;

    /* the compiler should put static strings into read-only storage... */
    if (string[0] == (char)0) return (const char *)"";

    if (immutable_strings == (struct tree *)0) {
        immutable_strings = tree_create();
    } else if (tree_get_node (immutable_strings, (int_pointer)string) != (struct tree_node *)0) {
        return string;
    }

    for (stringlength = 0; string[stringlength] != (char)0; stringlength++);

    stringlength++; /* add an extra character for the terminating 0 */

    return (char *)immutable (string, stringlength);
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

        immutable_data = get_mem(new_size);
        immutable_data_space_left = new_size;
        immutable_cursor = immutable_data;
        immutable_data_size = new_size;
    }

    rv = immutable_cursor;

    for (; length != 0;
         immutable_cursor++,
         data_char++,
         length--,
         immutable_data_space_left--) {

         *immutable_cursor = *data_char;
    }

    return rv;
}

void lock_immutable_pages ( void ) {
    if (immutable_data_size != 0) {
        mark_mem_ro (immutable_data_size, immutable_data);
    }

    immutable_data_size = 0;
    immutable_data_space_left = 0;
}
