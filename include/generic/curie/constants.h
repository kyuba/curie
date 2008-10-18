/*
 *  constants.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 20/09/2008.
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
 *  \brief Constants
 *
 *  These constants are likely to require tuning on different OSs and
 *  architectures, so they get their own header.
 */

#ifndef LIBCURIE_CONSTANTS_H
#define LIBCURIE_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Page Size
 *
 *  This is the page size that is assumed for memory operations. This is usually
 *  constant for a given OS/architecture combination.
 */
#define LIBCURIE_PAGE_SIZE 0x1000

/*! \brief Allocator Alignment (in Bytes)
 *
 *  Memory allocated by the curie functions will always be aligned to this value
 *  -- the alignment will be 'both ways', so to speak, i.e. the start address
 *  will be aligned to a multiple of this value and the size of the allocated
 *  chunk of memory will be a multiple of this as well.
 */
#define ENTITY_ALIGNMENT ((unsigned short)8)

/*! \brief aalloc() Allocator Cutoff
 *
 *  Requested sizes bigger than this cutoff will be allocated directly as memory
 *  pages, sizes smaller than that will be allocated using a mempool.
 */
#define CURIE_POOL_CUTOFF 1024

/*! \brief The chunk size for I/O buffers
 *
 *  I/O buffers are always allocated in multiples of this. The default of 
 *  LIBCURIE_PAGE_SIZE should work quite fine for most purposes, especially
 *  since get_mem() is used to get memory for I/O buffers.
 */
#define IO_CHUNKSIZE LIBCURIE_PAGE_SIZE

#ifdef __cplusplus
}
#endif

#endif
