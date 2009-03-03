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
 *  only have small amounts of memory, i.e. anything smaller than one or so
 *  kilobyte, then using this allocation scheme will waste a good deal of
 *  memory. On the bright side, this type of allocation is also quite possibly
 *  the fastest, and most of the complexity is left for the operating system to
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

/*! \brief Make Block of Memory executable
 *  \param[in] size  The current size of the block.
 *  \param[in] block The block to make executable.
 *
 *  Make the given block readable as well as executable.
 */
void mark_mem_rx (unsigned long int size, /*@notnull@*/ void *block);

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

/*! \defgroup memoryPools Memory Pools
 *  \ingroup memory
 *  \brief Pools of small Memory Entities with the same Size
 *
 *  These functions are more efficient for smaller amounts of memory: the memory
 *  is allocated in chunks of the page size, and then divided up in smaller
 *  chunks defined at pool creation time. Since all chunks are of the same size,
 *  the total memory overhead for managing a multitude of chunks is reduced a
 *  lot.
 *
 *  \note For best results, the individual entities should not exceed one fourth
 *        of the target machine's pagesize.
 *
 *  @{
 */

/*! \brief Pool Frame Type
 *
 *  Description tag to specify the type of pool frame.
 */
enum memory_pool_frame_type {
    mpft_static_header = 0x0, /*!< Static, info-only frame */
    mpft_frame         = 0x1  /*!< "Real" frame that contains memory to serve */
};

/*! \brief Memory Pool Header
 *  \internal
 *
 *  Each memory pool page uses this struct as its header. It's used to keep
 *  track of which elements are still available, which are used, how big the
 *  entities in the pool are, etc.
 */
struct memory_pool {
    /*! \brief Pool Frame Type
     *  \internal
     *
     *  This is used to figure out just what the header is describing.
     */
    enum memory_pool_frame_type type;

    /*! \brief Entity Size
     *
     *  All elements of this pool frame have exactly this size, in bytes.
     */
    unsigned int entitysize;
};

/*! \brief Calculate aligned Memory Size
 *  \param[in] a The (potentially) unaligned size, in bytes.
 *  \return The aligned size, in bytes.
 *
 *  This macro is used to determine the number of bytes that you would get
 *  from any memory allocation functions.
 */
#define calculate_aligned_memory_size(a)\
    ((((unsigned long)a) & ~(ENTITY_ALIGNMENT - 1)) +\
      (((((unsigned long)a) & ~(ENTITY_ALIGNMENT - 1)) != ((unsigned long)a)) ?\
         ENTITY_ALIGNMENT : 0))

/*! \brief Static Memory Pool Initialiser
 *  \param[in] size The size of the entities to get from the pool.
 *  \return Sentinel pool frame.
 *
 *  Just like create_memory_pool(), but you get to use it as a static
 *  initialiser, that is it's legit to use it when defining file-scope variables
 *  or static variables.
 */
#define MEMORY_POOL_INITIALISER(size)\
    { mpft_static_header, calculate_aligned_memory_size(size) }

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

/*! \brief Optimise Static Memory Pools
 *
 *  This function calls optimise_memory_pool() on all the memory pools created
 *  using the MEMORY_POOL_INITIALISER() macro.
 */
void optimise_static_memory_pools();

/*! @} */

/*! \defgroup memoryMixed Mixed Memory Allocator
 *  \ingroup memory
 *  \brief Allocating Memory with less straightforward Characteristica
 *
 *  This allocation scheme should be used when the exact size of the memory
 *  blocks aren't known in advance, or they vary a lot. Basically this allocator
 *  will use the memory pools or blocks, depending on the size of the requested
 *  memory at runtime.
 *
 *  If you're thinking in malloc() and free(), this is for you.
 *
 *  @{
 */

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

/*! @} */

#ifdef __cplusplus
}
#endif

#endif
