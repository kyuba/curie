/*
 *  memory-pool.c
 *  libcurie
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

#include <curie/internal-constants.h>
#include <curie/memory.h>
#include <curie/memory-internal.h>
#include <curie/int.h>

#define AUTOOPT_N 300

static struct memory_pool *static_pools[LIBCURIE_PAGE_SIZE / ENTITY_ALIGNMENT];

/*@-fixedformalarray@*/
static unsigned int bitmap_getslot(unsigned int b)
{
    return (unsigned int)(b / BITSPERBITMAPENTITY);
}

static void bitmap_set(pool_bitmap m, unsigned int b)
{
    unsigned int s = bitmap_getslot(b);
    m[s] |= 1 << (b%BITSPERBITMAPENTITY);
}

static void bitmap_clear(pool_bitmap m, unsigned int b)
{
    unsigned int s = bitmap_getslot(b);
    m[s] &= ~(1 << (b%BITSPERBITMAPENTITY));
}

static unsigned char bitmap_isset(pool_bitmap m, unsigned int b)
{
    unsigned int s = bitmap_getslot(b);

    return (unsigned char)((m[s] & (1 << (b%BITSPERBITMAPENTITY))) != 0);
}

static unsigned char bitmap_isempty(pool_bitmap m)
{
    unsigned int i;
    for (i = 0; i < BITMAPMAPSIZE; i++) {
        if (m[i] != 0) return (unsigned char)0;
    }

    return (unsigned char)1;
}
/*@=fixedformalarray@*/

struct memory_pool *create_memory_pool (unsigned long int entitysize)
{
    struct memory_pool_frame_header *pool = get_mem_chunk();
    unsigned int i;

    if (pool == (struct memory_pool_frame_header *)0) {
        return (struct memory_pool *)0;
    }

    pool->entitysize = calculate_aligned_memory_size(entitysize);

    pool->maxentities = (unsigned short)((LIBCURIE_PAGE_SIZE - sizeof(struct memory_pool_frame_header)) / pool->entitysize);

    for (i = 0; i < BITMAPMAPSIZE; i++) {
        pool->map[i] = 0;
    }

    if (pool->maxentities > BITMAPMAXBLOCKENTRIES) pool->maxentities = BITMAPMAXBLOCKENTRIES;

    /*@-mustfree@*/
    pool->next = (struct memory_pool_frame_header *)0;
    /*@=mustfree@*/

    pool->type = mpft_frame;

    pool->optimise_counter = AUTOOPT_N;

    return (struct memory_pool *)pool;
}

void free_memory_pool (struct memory_pool *pool)
{
    if (pool->type == mpft_frame)
    {
        struct memory_pool_frame_header *h
            = (struct memory_pool_frame_header *)pool;

        if (h->next != ((void *)0))
            free_memory_pool ((struct memory_pool *)h->next);

        free_mem_chunk((struct memory_pool *)pool);
    }
}

/*@-memtrans -branchstate@*/
/*@null@*/ /*@only@*/ static void *get_pool_mem_inner
    (struct memory_pool_frame_header *pool,
     struct memory_pool_frame_header *frame)
{
    unsigned int index = 0;

    for (; index < frame->maxentities; index++) {
        if (bitmap_isset(frame->map, index) == (unsigned char)0)
        {
            char *frame_mem_start;

            bitmap_set(frame->map, index);

            frame_mem_start = (char *)frame + sizeof(struct memory_pool_frame_header);

            if ((pool != frame) &&
                (pool->next != (struct memory_pool_frame_header *)0) &&
                (pool->next != frame))
            {
                struct memory_pool_frame_header *cursor = pool->next;

                while (cursor->next != frame) {
                    cursor = cursor->next;
                }

                /*@-mustfree@*/
                cursor->next = frame->next;
                frame->next = pool->next;
                pool->next = frame;
                /*@=mustfree@*/
            }

            /*@-usedef@*/
            return (void *)(frame_mem_start + (index * (frame->entitysize)));
            /*@=usedef@*/
        }
    }

    if (frame->next == (struct memory_pool_frame_header *)0) {
        struct memory_pool *n = create_memory_pool (frame->entitysize);
        if (n == (struct memory_pool *)0) return (void *)0;

        frame->next = (struct memory_pool_frame_header *)n;
    }

    return get_pool_mem_inner(pool, frame->next);
}
/*@=memtrans =branchstate@*/

void *get_pool_mem(struct memory_pool *pool)
{
    switch (pool->type)
    {
        case mpft_frame:
        {
            struct memory_pool_frame_header *h
                = (struct memory_pool_frame_header *)pool;

            h->optimise_counter--;
            if (h->optimise_counter == 0) {
                h->optimise_counter = AUTOOPT_N;
                optimise_memory_pool (pool);
            }
        }
        return get_pool_mem_inner((struct memory_pool_frame_header *)pool,
                                  (struct memory_pool_frame_header *)pool);

        case mpft_static_header:
        {
            unsigned short r = (pool->entitysize / ENTITY_ALIGNMENT) - 1;

            if (static_pools[r] == (struct memory_pool *)0)
            {
                static_pools[r] = create_memory_pool(pool->entitysize);
            }

            return get_pool_mem (static_pools[r]);
        }
    }

    return (void *)0;
}

/*@-mustfree@*/
void free_pool_mem(void *mem)
{
/* actually we /can/ derive the start address of a pool frame using an
   address that points into the pool...
   this is because get_mem_chunk() is supposed to use an address that is
   pagesize-aligned. */

    struct memory_pool_frame_header *pool = (struct memory_pool_frame_header *)((((int_pointer)(((int_pointer)mem)) / LIBCURIE_PAGE_SIZE)) * LIBCURIE_PAGE_SIZE);
    char *pool_mem_start = (char *)pool + sizeof(struct memory_pool_frame_header);

    unsigned int index = (unsigned int)(((char*)mem - pool_mem_start) / pool->entitysize);

    bitmap_clear (pool->map, index);
}
/*@=mustfree@*/

/*@-branchstate -memtrans@*/
void optimise_memory_pool(struct memory_pool *pool)
{
    struct memory_pool_frame_header
        *cursor = (struct memory_pool_frame_header *)pool,
        *last = (struct memory_pool_frame_header *)0;

    if (cursor->type != mpft_frame) return;

    while ((cursor != (struct memory_pool_frame_header *)0) &&
           (cursor->next != (struct memory_pool_frame_header *)0))
    {
        last = cursor;
        cursor = cursor->next;

        if (bitmap_isempty(cursor->map) == (unsigned char)1) {
            /*@-mustfree@*/
            last->next = cursor->next;
            /*@=mustfree@*/
            free_mem_chunk((void *)cursor);
            cursor = last;
        }
    }
}
/*@=branchstate =memtrans@*/
