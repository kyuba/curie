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

/*! \file
 *  \ingroup memory
 *  \brief Memory Management (Internal)
 *  \internal
 *
 */

#ifndef LIBCURIE_MEMORY_INTERNAL_H
#define LIBCURIE_MEMORY_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/memory.h>

/*! \brief Bitmap to keep track of used Entities in memory_pool
 *  \internal
 *
 *  This type is used to keep track of which entities in a memory pool are used.
 */
typedef BITMAPENTITYTYPE pool_bitmap[(BITMAPMAPSIZE+1)];

/*! \brief Memory Pool Frame Header
 *  \internal
 *
 *  Each memory pool page uses this struct as its header. It's used to keep
 *  track of which elements are still available, which are used, how big the
 *  entities in the pool are, etc.
 */
struct memory_pool_frame_header {
    enum memory_pool_frame_type type;

    /*! \brief Entity Size
     *
     *  All elements of this pool frame have exactly this size, in bytes.
     */
    unsigned int entitysize;

    /*! \brief Maximal Number of Elements
     *
     *  This is the number of entities that can be allocated from this pool
     *  frame. The number is calculated when the pool frame is allocated.
     */
    unsigned short maxentities;

    /*! \brief Next Frame
     *
     *  This is a pointer to the next pool frame, or (struct memory_pool *)0 if
     *  there is no next frame.
     */
    struct memory_pool_frame_header *next;

    /*! \brief Allocation Bitmap
     *
     *  This bitmap is used to keep track of which entities are still available.
     *  When an entity is given out by get_pool_mem(), the corresponding bit in
     *  this bitmap is set to 1, so that it won't be given out again. Once it's
     *  deallocated with free_pool_mem(), it's set to 0.
     */
    pool_bitmap map;
};

#ifdef __cplusplus
}
#endif

#endif
