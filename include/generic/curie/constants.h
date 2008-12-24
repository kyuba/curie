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

#include <curie/int.h>

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

/*! \brief pool_bitmap: Entity Type
 *
 *  This is the type used for the pool_bitmap array.
 */
#define BITMAPENTITYTYPE int_32

/*! \brief pool_bitmap: Bits per Entity
 *
 *  This is the number of bits in BITMAPENTITYTYPE.
 */
#define BITSPERBITMAPENTITY (unsigned short)(32)

/*! \brief pool_bitmap: Number of Elements
 *
 *  This is the number of BITMAPENTITYTYPE elements in pool_bitmap.
 */
#define BITMAPMAPSIZE (unsigned short)16

/*! \brief pool_bitmap: Total Number of Bits
 *
 *  This is the number of bits in pool_mem that may be used.
 */
#define BITMAPMAXBLOCKENTRIES (unsigned short)(BITMAPMAPSIZE * BITSPERBITMAPENTITY)

/*! \brief The chunk size for I/O buffers
 *
 *  I/O buffers are always allocated in multiples of this. The default of 
 *  LIBCURIE_PAGE_SIZE should work quite fine for most purposes, especially
 *  since get_mem() is used to get memory for I/O buffers.
 */
#define IO_CHUNKSIZE LIBCURIE_PAGE_SIZE

/*! \brief Threshold for sx_read()
 *
 * the sx_read function will try to keep reading off a struct io * until either
 * nothing more is available just now, or MAX_READ_THRESHOLD is hit. */
#define SX_MAX_READ_THRESHOLD (128*1024)

/*! \brief Maximum Size for S-Expression Strings
 *
 *  Longer strings are silently truncated.
 */
#define SX_MAX_STRING_LENGTH 1025

/*! \brief Maximum Size for S-Expression Symbols
 *
 *  Longer symbols are silently truncated.
 */
#define SX_MAX_SYMBOL_LENGTH 385

/*! \brief Maximum Size for S-Expression Numerals
 *
 *  Longer numbers are silently truncated.
 */
#define SX_MAX_NUMBER_LENGTH 33

#ifdef __cplusplus
}
#endif

#endif
