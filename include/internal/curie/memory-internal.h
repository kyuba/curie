/*
 *  memory-internal.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 26/11/2008.
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
typedef BITMAPENTITYTYPE pool_bitmap[BITMAPMAPSIZE];

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
    /*@null@*/ /*@only@*/ struct memory_pool_frame_header *next;

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
