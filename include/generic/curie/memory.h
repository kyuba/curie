/*
 *  memory.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 01/06/2008.
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

/*! \defgroup memory Memory Management
 *
 *  Dynamic memory management for Curie applications consists of three more or
 *  less distinct types of memory allocation: The first type is used to get
 *  memory pages from the OS, the second type is special memory pools, and the
 *  third type is something that works almost like malloc().
 */

/*! \file
 *  \ingroup memory
 *  \brief Memory Management
 *
 *  Functions for dynamic memory management.
 */

#ifndef LIBCURIE_MEMORY_H
#define LIBCURIE_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/constants.h>

/*! \defgroup memoryExceptions Exception Handling
 *  \ingroup memory
 *  \brief Memory Allocation Exceptions
 *
 *  These functions help with handling memory allocation exceptions (usually
 *  that's when there's no more memory to get).
 *
 *  Only the block allocation functions may generate exceptions, because the
 *  two other allocation schemes rely on the block allocation.
 *
 *  @{
 */

/*! \brief Define get_mem() Recovery Function
 *  \param[in] handler Pointer to the recovery function.
 *
 *  This function is called when get_mem() can't allocate any memory, and its
 *  result is returned instead of the (void *)0 that get_mem() would return.
 */
void set_get_mem_recovery_function 
        (/*@null@*/ void *(*handler)(unsigned long int));

/*! \brief Define resize_mem() Recovery Function
 *  \param[in] handler Pointer to the recovery function.
 *
 *  This function is called when resize_mem() can't allocate any memory, and
 *  its result is returned instead of the (void *)0 that resize_mem() would
 *  return.
 */
void set_resize_mem_recovery_function
        (/*@null@*/ void *(*handler)(unsigned long int, void *, unsigned long int));

/*! @} */

/*! \defgroup memoryBlocks Block Allocation
 *  \ingroup memory
 *  \brief Allocation of Blocks of Memory that are multiples of the Page Size
 *
 *  Memory blocks are always fetched from the operating system in large chunks,
 *  for example with mmap() to get anonymous memory on POSIX systems. If you
 *  only have small amounts of memory, i.e. anything smaller than 2 or so
 *  kilobytes, then using this allocation scheme will waste a good deal of
 *  memory. On the bright side, this type of allocation is also quite possibly
 *  the fastests, and most of the complexity is left for the operating system to
 *  optimise.
 *
 *  \note The two other allocation mechanisms rely on this one.
 *
 *  @{
 */

/*! \brief Allocate Memory
 *  \param[in] size The number of bytes to allocate.
 *  \return Pointer to the newly allocated chunk of memory, or (void*)0 if the
 *          memory could not be allocated.
 *
 *  This function allocates memory and returns a pointer to the newly allocated
 *  chunk. The memory is allocated in multiples of the pagesize, so if you only
 *  need small amounts of memory, using this function wastes memory. On the
 *  other hand, this function is usually very fast to allocate the memory.
 */
/*@null@*/ /*@only@*/ void *get_mem
        (unsigned long int size);

/*! \brief Resize Memory Block
 *  \param[in] size    Current size of the block, in chunks.
 *  \param[in] block   The block to resize.
 *  \param[in] newsize The new size of the block.
 *  \return Pointer to the resized chunk of memory, or (void*)0 if new memory to
 *          hold the data could not be allocated.
 *
 *  This function takes a previously allocated block of memory and resizes it to
 *  a new size. The contents of the block are kept in tact, unless the new size
 *  is smaller than the current size, in which case the block's contents will be
 *  truncated.
 */
/*@null@*/ /*@only@*/ void *resize_mem
        (unsigned long int size,
         /*@notnull@*/ /*@only@*/ /*@returned@*/ void * block,
         unsigned long int newsize);

/*! \brief Free Memory Block
 *  \param[in] size  The size of the block.
 *  \param[in] block The block to deallocate.
 *
 *  The passed block is deallocated, and its memory is returned to the OS. Any
 *  further dereferences of the block are likely to trigger a SIGSEGV.
 */
void free_mem
        (unsigned long int size, /*@notnull@*/ /*@only@*/ void * block);

/*! \brief Make Block of Memory read-only
 *  \param[in] size  The current size of the block.
 *  \param[in] block The block to make read-only.
 *
 *  After calling this function, the given block of memory is set to read-only
 *  mode, that is, writing to it should produce a segmentation violation.
 *  Probably not all operating systems support this, but it may help in
 *  tightening down memory access for those operating systems that do.
 */
void mark_mem_ro (unsigned long int size, /*@notnull@*/ void *block);

/*! \brief Make Block of Memory writable
 *  \param[in] size  The current size of the block.
 *  \param[in] block The block to make writable.
 *
 *  Make the given block readable as well as writable. This is the default.
 */
void mark_mem_rw (unsigned long int size, /*@notnull@*/ void *block);

/*! \brief Allocate a Chunk of Memory
 *  \return Pointer to the newly allocated chunk of memory, or (void*)0 if the
 *          memory could not be allocated.
 *
 *  Same as get_mem(), but it always allocates LIBCURIE_PAGE_SIZE bytes of
 *  memory.
 */
/*@null@*/ /*@only@*/ void *get_mem_chunk();

/*! \brief Free Memory allocated by get_mem_chunk()
 *
 *  Same as free_mem(), with the size parameter set to LIBCURIE_PAGE_SIZE.
 */
#define free_mem_chunk(p) free_mem(LIBCURIE_PAGE_SIZE, p)

/*! @} */

/*! \brief Bitmap to keep track of used Entities in memory_pool
 *  \internal
 *
 *  This type is used to keep track of which entities in a memory pool are used.
 */
typedef BITMAPENTITYTYPE pool_bitmap[BITMAPMAPSIZE];

/*! \brief Memory Pool Page Header
 *  \internal
 *
 *  Each memory pool page uses this struct as its header. It's used to keep
 *  track of which elements are still available, which are used, how big the
 *  entities in the pool are, etc.
 */
struct memory_pool {
    /*! \brief Entity Size
     *
     *  All elements of this pool frame have exactly this size, in bytes.
     */
    unsigned long int entitysize;

    /*! \brief Maximal Number of Elements
     *
     *  This is the number of entities that can be allocated from this pool
     *  frame. The number is calculated when the pool frame is allocated.
     */
    unsigned short maxentities;

    /*! \brief Optimisation Counter
     *
     *  Once this counter reaches zero, the pool is optimised, i.e. empty frames
     *  are removed.
     */
    unsigned short optimise_counter;

    /*! \brief Next Frame
     *
     *  This is a pointer to the next pool frame, or (struct memory_pool *)0 if
     *  there is no next frame.
     */
    /*@null@*/ /*@only@*/ struct memory_pool *next;

    /*! \brief Allocation Bitmap
     *
     *  This bitmap is used to keep track of which entities are still available.
     *  When an entity is given out by get_pool_mem(), the corresponding bit in
     *  this bitmap is set to 1, so that it won't be given out again. Once it's
     *  deallocated with free_pool_mem(), it's set to 0.
     */
    pool_bitmap map;
};

/*! \brief Static Memory Pool Initialiser
 *  \param[in] size The size of the entities to get from the pool.
 *  \return Sentinel pool frame.
 *
 *  Just like create_memory_pool(), but you get to use it as a static
 *  initialiser, that is it's legit to use it when defining file-scope variables
 *  or static variables.
 */
#define MEMORY_POOL_INITIALISER(size) { size, 0, 300, (struct memory_pool *)0 }

/*! \brief Create a new Memory Pool
 *  \param[in] entitysize The size of the entities to get from the pool.
 *  \return New pool frame, or (struct memory_pool *)0 if get_mem() failed.
 *
 *  Initialises a new memory pool that returns elements of the given entitysize.
 */
/*@null@*/ /*@only@*/ struct memory_pool *create_memory_pool
        (unsigned long int entitysize);

/*! \brief Free a Memory Pool
 *  \param[in] pool The pool to deallocate.
 *
 *  Return all the memory assoicated with the pool argument back to the
 *  operating system. Using either the passed pool, or any entities retrieved
 *  from it after this function has been called results in a segmentation
 *  violation.
 */
void free_memory_pool (/*@notnull@*/ /*@only@*/ struct memory_pool *pool);

/*! \brief Get Memory from Pool
 *  \param[in] pool The pool to allocate from.
 *  \return Pointer to the new entity, or (void *)0 if no entities were
 *          available and get_mem() failed to allocate memory for a new frame.
 *
 *  This allocates an entity of pool's entitiy size from the given pool. It is
 *  up to the caller to remember how big entities in the pool are.
 */
/*@null@*/ /*@only@*/ void *get_pool_mem
        (/*@notnull@*/ struct memory_pool *pool);

/*! \brief Free Memory allocated from a Memory Pool
 *  \param[in] entity The entity to deallocate.
 *
 *  The given entity's address is marked as being available for subsequent
 *  allocations. The memory is not returned to the operating system immediately,
 *  it's only made available for subsequent allocations.
 */
void free_pool_mem(/*@notnull@*/ /*@only@*/void *entity);

/*! \brief Optimise Memory Pool
 *  \param[in] pool The pool to clean up.
 *
 *  This function prunes the pool of empty frames, that is frames where all
 *  entities are still available. The memory associated with these frames is
 *  returned to the operating system.
 */
void optimise_memory_pool (/*@notnull@*/ struct memory_pool *pool);


/*! \brief Allocate Memory
 *  \param[in] size The number of bytes to allocate.
 *  \return The allocated block, or (void *)0 if no memory is available.
 *
 *  Allocate the requested number of bytes. This function uses either get_mem()
 *  or a memory pool, depending on the size to allocate. If you're thinking
 *  malloc(), and you don't know how big your memory is going to be, use this.
 */
/*@null@*/ /*@only@*/ void *aalloc
        (unsigned long size);

/*! \brief Resize Memory
 *  \param[in] size    The current size of the block.
 *  \param[in] block   The block to resize.
 *  \param[in] newsize The new size.
 *  \return The resized block, or (void *)0 if no memory was available.
 *
 *  This resizes the given block of memory from size to newsize. Only use this
 *  function for memory you've gotten from aalloc().
 */
/*@null@*/ /*@only@*/ void *arealloc
        (unsigned long size,
         /*@notnull@*/ /*@only@*/ void *block,
         unsigned long newsize);

/*! \brief Free Memory
 *  \param[in] size  The current size of the block.
 *  \param[in] block The block to deallocate.
 *
 *  This deallocates a block that was allocated with aalloc(). Depending on the
 *  size of the block, the memory may or may not be available to the operating
 *  system right after this call.
 */
void afree (unsigned long size, /*@notnull@*/ /*@only@*/ void *block);

#ifdef __cplusplus
}
#endif

#endif
