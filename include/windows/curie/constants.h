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
#define LIBCURIE_PAGE_SIZE 0x10000

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
