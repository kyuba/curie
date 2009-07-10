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
 *  \brief Constants (Internal)
 *
 *  These constants are likely to require tuning on different OSs and
 *  architectures, so they get their own header.
 */

#ifndef LIBCURIE_INTERNAL_CONSTANTS_H
#define LIBCURIE_INTERNAL_CONSTANTS_H

#include <curie/constants.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIBCURIE_STAT_BUFFER_SIZE 256
#define LIBCURIE_DIRENT_BUFFER_SIZE 0x1000
#define AUTOOPT_N 300
#define POOLCOUNT (LIBCURIE_PAGE_SIZE / ENTITY_ALIGNMENT)

#ifdef __cplusplus
}
#endif

#endif
