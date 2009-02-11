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
 *  \brief Filesystem Information
 *
 *  The functions in this header file are used to query the OS about certain
 *  pieces of filesystem information.
 */

#ifndef LIBCURIE_FILESYSTEM_H
#define LIBCURIE_FILESYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/sexpr.h>

/*! \brief Check if a File exists
 *  \param[in] path The path to check.
 *  \return sx_false if the file does not exist, sx_true if it does exist.
 *
 *  This function basically uses stat() to figure out if the given file exists.
 */
sexpr filep(sexpr path);

/*! \brief Check if a Link exists
 *  \param[in] path The path to check.
 *  \return sx_false if the link does not exist, sx_true if it does exist.
 *
 *  Same as filep(), but for symlinks it only checks the presence of the
 *  symlink, as opposed to checking for the actual file referenced by the
 *  symlink.
 */
sexpr linkp(sexpr path);

#ifdef __cplusplus
}
#endif

#endif
