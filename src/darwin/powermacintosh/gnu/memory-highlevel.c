/*
 *  darwin-powermacintosh-gnu/memory-highlevel.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 22/06/2008.
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

#include <atomic/memory.h>

#define _PAGESIZE 0x1000

static unsigned long int get_multiple_of_pagesize(unsigned long int s)
{
    if ((s % _PAGESIZE) == 0)
        return (unsigned long int)s;
    else
        return (unsigned long int)(((s / _PAGESIZE) + 1) * _PAGESIZE);
}


/* this is the same as the generic posix variant */

void *resize_mem(unsigned long int size, void *location, unsigned long int new_size) {
    unsigned long int msize = get_multiple_of_pagesize(size);
    unsigned long int mnew_size = get_multiple_of_pagesize(new_size);

    if (msize != mnew_size) {
        int *new_location = (int *)get_mem(new_size),
            *old_location = (int *)location;
        int i = 0,
            copysize = (int)((size < new_size) ? size : new_size);

        copysize = (int)((copysize / sizeof(int))
                         + (((copysize % sizeof(int)) == 0) ? 0 : 1));

        for(; i < copysize; i++) {
            /* copy in chunks of ints */
            new_location[i] = old_location[i];
        }

        free_mem (size, location);

        return (void*)new_location;
    } else return location;
}
