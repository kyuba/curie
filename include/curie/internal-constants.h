/**\file
 * \brief Constants (Internal)
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

#ifndef LIBCURIE_INTERNAL_CONSTANTS_H
#define LIBCURIE_INTERNAL_CONSTANTS_H

#include <curie/constants.h>

#ifdef __cplusplus
extern "C" {
#endif

/**\brief sys_stat() buffer size
 *
 * Size of the buffer used to gather data via the sys_stat() call.
 */
#define LIBCURIE_STAT_BUFFER_SIZE 256

/**\brief Directory entity buffer size
 *
 * Size of a single directory entity. Used when reading directory entities in
 * a loop.
 */
#define LIBCURIE_DIRENT_BUFFER_SIZE 0x1000

/**\brief Number of static pools
 *
 * The number of static memory pools as kept by the memory allocator.
 */
#define POOLCOUNT (LIBCURIE_PAGE_SIZE / ENTITY_ALIGNMENT)

#ifdef __cplusplus
}
#endif

#endif
