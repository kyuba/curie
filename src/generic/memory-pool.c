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

#define POOLCOUNT (LIBCURIE_PAGE_SIZE / ENTITY_ALIGNMENT)

static unsigned int optimise_counter = AUTOOPT_N;
static struct memory_pool *static_pools[POOLCOUNT];

#define bitmap_getslot(b) ((unsigned int)((b) / BITSPERBITMAPENTITY))

#define bitmap_set(m,b,c)\
    m[c] |= 1 << (b%BITSPERBITMAPENTITY)

#define bitmap_isset(m,b,c)\
    ((unsigned char)((m[c] & (1 << (b%BITSPERBITMAPENTITY))) != 0))

#define bitmap_clear(m,b)\
    m[bitmap_getslot(b)] &= ~(1 << (b%BITSPERBITMAPENTITY))

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
    do {
        unsigned int index = 0, cell = 0;

        do {
            if (frame->map[cell] == ((BITMAPENTITYTYPE)(~0)))
            {
                index += BITSPERBITMAPENTITY;
            }
            else
            {
                unsigned int high = index + BITSPERBITMAPENTITY;
                if (high > frame->maxentities) high = frame->maxentities;

                do {
                    if (bitmap_isset(frame->map, index, cell)
                        == (unsigned char)0)
                    {
                        char *frame_mem_start = (char *)frame + sizeof(struct memory_pool_frame_header);

                        bitmap_set(frame->map, index, cell);

                        if ((pool != frame) &&
                            (pool->next != (struct memory_pool_frame_header *)0)
                            && (pool->next != frame))
                        {
                            struct memory_pool_frame_header *cursor = pool->next;

                            while (cursor->next != frame)
                            {
                                cursor = cursor->next;
                            }

                            /*@-mustfree@*/
                            cursor->next = frame->next;
                            frame->next = pool->next;
                            pool->next = frame;
                            /*@=mustfree@*/
                        }

                        /*@-usedef@*/
                        return (void *)(frame_mem_start
                                + (index * (frame->entitysize)));
                        /*@=usedef@*/
                    }
                    index++;
                } while (index < high);
            }

            cell++;
        } while (index < frame->maxentities);

        if (frame->next == (struct memory_pool_frame_header *)0) {
            frame->next = (struct memory_pool_frame_header *)
                          create_memory_pool (frame->entitysize);
        }
    } while ((frame = frame->next) != (struct memory_pool_frame_header *)0);

    return (void *)0;
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

    optimise_counter--;
    if (optimise_counter == 0)
    {
        optimise_counter = AUTOOPT_N;
        for (unsigned int i = 0; i < POOLCOUNT; i++)
        {
            if (static_pools[i] != (struct memory_pool *)0)
            {
                optimise_memory_pool (static_pools[i]);
            }
        }
    }
}
/*@=mustfree@*/

/*@-branchstate -memtrans@*/
void optimise_memory_pool(struct memory_pool *pool)
{
    struct memory_pool_frame_header
        *cursor = (struct memory_pool_frame_header *)pool,
        *last = (struct memory_pool_frame_header *)0;
    unsigned int i;

    if (cursor->type != mpft_frame) return;

    while ((cursor != (struct memory_pool_frame_header *)0) &&
           (cursor->next != (struct memory_pool_frame_header *)0))
    {
        last = cursor;
        cursor = cursor->next;

        for (i = 0; i < BITMAPMAPSIZE; i++)
        {
            if (cursor->map[i] != 0) break;
        }

        if (i == BITMAPMAPSIZE)
        {
            /*@-mustfree@*/
            last->next = cursor->next;
            /*@=mustfree@*/
            free_mem_chunk((void *)cursor);
            cursor = last;
        }
    }
}
/*@=branchstate =memtrans@*/
