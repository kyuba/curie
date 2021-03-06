/**\file
 * \brief Constants
 *
 * These constants are likely to require tuning on different OSs and
 * architectures, so they get their own header.
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
 */

#ifndef LIBCURIE_CONSTANTS_H
#define LIBCURIE_CONSTANTS_H

#include <curie/int.h>

#ifdef __cplusplus
extern "C" {
#endif

/**\brief Page Size
 *
 * This is the page size that is assumed for memory operations. This is usually
 * constant for a given OS/architecture combination.
 */
#define LIBCURIE_PAGE_SIZE 0x1000

/**\brief Stack Buffer Size
 *
 * Temporary buffers up to this size are allocated on the stack instead of via
 * aalloc().
 */
#define STACK_BUFFER_SIZE 0x1000

/**\brief Allocator Alignment (in Bytes)
 *
 * Memory allocated by the curie functions will always be aligned to this value
 * -- the alignment will be 'both ways', so to speak, i.e. the start address
 * will be aligned to a multiple of this value and the size of the allocated
 * chunk of memory will be a multiple of this as well.
 */
#define ENTITY_ALIGNMENT ((unsigned short)8)

/**\brief aalloc() Allocator Cutoff
 *
 * Requested sizes bigger than this cutoff will be allocated directly as memory
 * pages, sizes smaller than that will be allocated using a mempool.
 */
#define CURIE_POOL_CUTOFF 0x400

/**\brief pool_bitmap: Entity Type
 *
 * This is the type used for the pool_bitmap array.
 */
#define BITMAPENTITYTYPE int_32

/**\brief pool_bitmap: Bits per Entity
 *
 * This is the number of bits in BITMAPENTITYTYPE.
 */
#define BITSPERBITMAPENTITY (unsigned short)(32)

/**\brief pool_bitmap: Number of Elements
 *
 * This is the number of BITMAPENTITYTYPE elements in pool_bitmap.
 */
#define BITMAPMAPSIZE (unsigned short)16

/**\brief pool_bitmap: Total Number of Bits
 *
 * This is the number of bits in pool_mem that may be used.
 */
#define BITMAPMAXBLOCKENTRIES (unsigned short)(BITMAPMAPSIZE * BITSPERBITMAPENTITY)

/**\brief The chunk size for I/O buffers
 *
 * I/O buffers are always allocated in multiples of this. The default of 
 * LIBCURIE_PAGE_SIZE should work quite fine for most purposes, especially
 * since get_mem() is used to get memory for I/O buffers.
 */
#define IO_CHUNKSIZE LIBCURIE_PAGE_SIZE

/**\brief Number of IO Structs to retain
 *
 * This is to prevent some rare and obscure bugs that easily creep in due to
 * asynchronous programming.
 */
#define IO_STRUCT_POOL_ENTRIES 0xf

/**\brief Threshold for sx_read()
 *
 * The sx_read function will try to keep reading off a struct io * until either
 * nothing more is available just now, or MAX_READ_THRESHOLD is hit. */
#define SX_MAX_READ_THRESHOLD (128*1024)

/**\brief Maximum Size for S-Expression Symbols
 *
 * Longer symbols are silently truncated.
 */
#define SX_MAX_SYMBOL_LENGTH 385

/**\brief Maximum Size for S-Expression Numerals
 *
 * Longer numbers are silently truncated.
 */
#define SX_MAX_NUMBER_LENGTH 33

/**\brief Chunk Size for Graph Nodes
 *
 * Space for graph nodes is allocated in chunks of this; needs to be a power of
 * 2. Smaller values provide better memory efficiency for smaller graphs and
 * regexen, larger values usually provide better performance since
 * modifications require fewer reallocations, which are expensive.
 *
 * This value also directly influences the minimum size requirements for graphs
 * as this number of nodes is always allocated along with the graph.
 */
#define GRAPH_NODE_CHUNK_SIZE 0x10

/**\brief Chunk Size for Graph Edges
 *
 * Space for graph edges is allocated in chunks of this; needs to be a power of
 * 2. Smaller values provide better memory efficiency for smaller graphs and
 * regexen, larger values usually provide better performance since
 * modifications require fewer reallocations, which are expensive.
 *
 * This value also directly influences the minimum size requirements for graph
 * nodes as this number of edges is always allocated along with each graph
 * node.
 */
#define GRAPH_EDGE_CHUNK_SIZE 0x8

/**\brief Date of the Unix Epoch
 *
 * For code dealing with dates, this is the offset between the long count and
 * the unix epoch.
 *
 * Remember: the first of January 1970 is 12.17.16.7.5.
 */
#define UNIX_EPOCH (12*144000 + 17*7200 + 16*360 + 7*20 + 5)

/**\brief Seconds per Day
 *
 * Should work fine everywhere on earth, but you never know...
 */
#define SECONDS_PER_DAY (24 * 60 * 60)

#ifdef __cplusplus
}
#endif

#endif
