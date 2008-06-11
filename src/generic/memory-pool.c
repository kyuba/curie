/*
 *  memory-pool.c
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

#include <atomic/memory.h>
#include <atomic/int.h>

/*@-mustfreeonly@*/
/*@-onlytrans@*/
/*@-sharedtrans@*/
/*@-temptrans@*/
/*@-fixedformalarray@*/
/* turns out we need this here, because we actually implement memory management here,
   including functions that do do these thingamajigs */

static unsigned int bitmap_getslot(unsigned int b) {
    return (unsigned int)(b / BITSPERBITMAPENTITY);
}

static void bitmap_set(bitmap m, unsigned int b) {
    unsigned int s = bitmap_getslot(b);
    m[s] |= 1 << (b%BITSPERBITMAPENTITY);
}

static void bitmap_clear(bitmap m, unsigned int b) {
    unsigned int s = bitmap_getslot(b);
    m[s] &= ~(1 << (b%BITSPERBITMAPENTITY));
}

static unsigned char bitmap_isset(bitmap m, unsigned int b) {
    unsigned int s = bitmap_getslot(b);

    return (unsigned char)((m[s] & (1 << (b%BITSPERBITMAPENTITY))) != 0);
}

static unsigned char bitmap_isempty(bitmap m) {
    unsigned int i;
    for (i = 0; i < BITMAPMAPSIZE; i++) {
        if (m[i] != 0) return (unsigned char)0;
    }

    return (unsigned char)1;
}

struct memory_pool *create_memory_pool (unsigned long int entitysize) {
    struct memory_pool *pool = get_mem_chunk();

    pool->entitysize = (unsigned short)(((entitysize / ENTITY_ALIGNMENT)
                         + (((entitysize % ENTITY_ALIGNMENT) == 0) ? 0 : 1))
                        * ENTITY_ALIGNMENT);
    pool->maxentities = (unsigned short)((mem_chunk_size - sizeof(struct memory_pool)) / pool->entitysize);

    if (pool->maxentities > BITMAPMAXBLOCKENTRIES) pool->maxentities = BITMAPMAXBLOCKENTRIES;

    pool->next = (struct memory_pool *)0;

    return pool;
}

void free_memory_pool (struct memory_pool *pool) {
    if (pool->next) free_memory_pool (pool->next);
    free_mem_chunk((void *)pool);
}

void *get_pool_mem(struct memory_pool *pool) {
    unsigned int index = 0;

    for (; index < pool->maxentities; index++) {
        if (bitmap_isset(pool->map, index) == (unsigned char)0)
        {
            char *pool_mem_start;

            bitmap_set(pool->map, index);

            pool_mem_start = (char *)pool + sizeof(struct memory_pool);

            return (void *)(pool_mem_start + (index * (pool->entitysize)));
        }
    }

    if (pool->next == (struct memory_pool *)0) {
        pool->next = create_memory_pool (pool->entitysize);
    }

    return get_pool_mem(pool->next);
}

void free_pool_mem(void *mem) {
/* actually we /can/ derive the start address of a pool frame using an
   address that points into the pool...
   this is because get_mem_chunk() is supposed to use an address that is
   pagesize-aligned and mem_chunk_size should be the pagesize. */

    struct memory_pool *pool = (struct memory_pool *)((((int_pointer)((char *)mem)) / mem_chunk_size) * mem_chunk_size);
    char *pool_mem_start = (char *)pool + sizeof(struct memory_pool);

    unsigned int index = (unsigned int)(((char*)mem - pool_mem_start) / pool->entitysize);

    bitmap_clear (pool->map, index);
}

void optimise_memory_pool(struct memory_pool *pool) {
    struct memory_pool *cursor = pool, *last = (struct memory_pool *)0;

    while ((cursor != (struct memory_pool *)0) &&
            (cursor->next != (struct memory_pool *)0)) {
        last = cursor;
        cursor = cursor->next;

        while ((cursor != (struct memory_pool *)0) &&
               (bitmap_isempty(cursor->map) == (unsigned char)1)) {
            last->next = cursor->next;
            free_mem_chunk((void *)cursor);
            cursor = last->next;
        }
    }
}
