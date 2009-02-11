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

#include <curie/internal-constants.h>
#include <curie/magic-constants.h>
#include <curie/memory.h>
#include <curie/memory-internal.h>
#include <curie/int.h>

static unsigned int optimise_counter = AUTOOPT_N;
static struct memory_pool *static_pools[POOLCOUNT];

#define bitmap_set(m,b,c)\
    m[c] &= ~(1 << (b%BITSPERBITMAPENTITY))

#define bitmap_clear(m,b,c)\
    m[c] |= (1 << (b%BITSPERBITMAPENTITY))

struct memory_pool *create_memory_pool (unsigned long int entitysize)
{
    struct memory_pool_frame_header *pool = get_mem_chunk();
    unsigned int i;

    if (pool == (struct memory_pool_frame_header *)0) {
        return (struct memory_pool *)0;
    }

    pool->entitysize = calculate_aligned_memory_size(entitysize);

    pool->maxentities = (unsigned short)((LIBCURIE_PAGE_SIZE - sizeof(struct memory_pool_frame_header)) / pool->entitysize);

    for (i = 0; i <= BITMAPMAPSIZE; i++) {
        pool->map[i] = (BITMAPENTITYTYPE)~0;
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
        BITMAPENTITYTYPE x = (((1 << BITMAPMAPSIZE)-1) & (frame->map[BITMAPMAPSIZE]));
        if (x)
        {
            BITMAPENTITYTYPE cell = (HASH_MAGIC_TABLE [(((x & -x) * HASH_MAGIC_MULTIPLIER) >> HASH_MAGIC_SHIFT) & HASH_MAGIC_TABLE_MASK]);

            x = frame->map[cell];

            BITMAPENTITYTYPE index = cell * BITSPERBITMAPENTITY +
                    (HASH_MAGIC_TABLE [(((x & -x) * HASH_MAGIC_MULTIPLIER) >> HASH_MAGIC_SHIFT) & HASH_MAGIC_TABLE_MASK]);

            if (index < frame->maxentities) {
                char *frame_mem_start = (char *)frame + sizeof(struct memory_pool_frame_header);

                bitmap_set(frame->map, index, cell);

                if (frame->map[cell] == ((BITMAPENTITYTYPE)0))
                {
                    frame->map[BITMAPMAPSIZE] &= ~((BITMAPENTITYTYPE)(1 << cell));
                }

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
        }

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
            return get_pool_mem_inner((struct memory_pool_frame_header *)pool,
                                      (struct memory_pool_frame_header *)pool);

        case mpft_static_header:
        {
            unsigned short r = (pool->entitysize / ENTITY_ALIGNMENT) - 1;

            if (static_pools[r] == (struct memory_pool *)0)
            {
                static_pools[r] = create_memory_pool(pool->entitysize);
            }

            return get_pool_mem_inner
                    ((struct memory_pool_frame_header *)static_pools[r],
                     (struct memory_pool_frame_header *)static_pools[r]);
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
    unsigned int cell = ((unsigned int)((index) / BITSPERBITMAPENTITY));

    bitmap_clear (pool->map, index, cell);
    pool->map[BITMAPMAPSIZE] |= ((BITMAPENTITYTYPE)(1 << cell));

    optimise_counter--;
    if (optimise_counter == 0)
    {
        optimise_static_memory_pools();
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
            if (cursor->map[i] != ((BITMAPENTITYTYPE)~0)) break;
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

void optimise_static_memory_pools()
{
    optimise_counter = AUTOOPT_N;

    for (unsigned int i = 0; i < POOLCOUNT; i++)
    {
        if (static_pools[i] != (struct memory_pool *)0)
        {
            optimise_memory_pool (static_pools[i]);

            for (struct memory_pool_frame_header *h =
                         (struct memory_pool_frame_header *)(static_pools[i]);
                 h != (struct memory_pool_frame_header *)0;)
            {
                unsigned int j;

                for (j = 0; j < BITMAPMAPSIZE; j++)
                {
                    if (h->map[j] != ((BITMAPENTITYTYPE)~0)) break;
                }

                if (j == BITMAPMAPSIZE)
                {
                    static_pools[i] = (struct memory_pool *)h->next;
                    free_mem_chunk((void *)h);

                    h = (struct memory_pool_frame_header *)(static_pools[i]);
                }
                else
                {
                    h = (struct memory_pool_frame_header *)0;
                }
            }
        }
    }
}
