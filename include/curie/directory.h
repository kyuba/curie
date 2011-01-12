/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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
 *  \brief Directory Listing/Filtering
 *
 *  Functions to retrieve the contents of directories.
 */

#ifndef LIBCURIE_DIRECTORY_H
#define LIBCURIE_DIRECTORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/regex.h>

/*! \brief Read Directory, filtered with a Regular Expression
 *  \param[in] base The base directory to search.
 *  \param[in] rx   Pattern to match against the directory entries.
 *  \return A list of files in the directory.
 *
 *  Search the given directory and create a list of all files in the directory
 *  that match the given regular expression. The list of files doesn't contain
 *  the base path, just the filenames.
 *
 *  Example: read_directory_rx("/", rx_compile(".*"));
 *  Would return something like this list:
 *  ("bin" "lib" "dev" "sbin" "usr" "opt" "var" "etc" "root" "home")
 */
sexpr read_directory_rx (const char *base, sexpr rx);

/*! \brief Read Directory, filtered with an S-expression
 *  \param[in] rx   The Pattern to use.
 *  \return A list of files matching the pattern.
 *
 *  Simple wrapper around read_directory_rx() and read_directory(). If rx is a
 *  string, the string is passed to read_directory. If it's a cons, it's assumed
 *  to be an NFA graph in s-expression form, the graph is converted and passed
 *  to read_directory() with "." as the base path.
 */
sexpr read_directory_sx (sexpr rx);

/*! \brief Read Directory, filtered with a C-style string
 *  \param[in] p    The Pattern to use.
 *  \return A list of files matching the pattern.
 *
 *  The string is split by '/' and each of the tokens is used as a regular
 *  expression to ultimately specify a number of files to find. It's annoying to
 *  describe verbally, so an example is probably best:
 *
 *  read_directory("/.+/lib");
 *  should yield: ("/././lib" "/./usr/lib" "/./var/lib")
 *
 *  and
 *
 *  read_directory(".+/lib");
 *  could yield (if you're in the root directory):
 *  ("././lib" "./usr/lib" "./var/lib")
 *
 *  Note that a leading "/" always results in a "/." prefix of the resulting
 *  list and any other character results in a simple "." prefix. This is
 *  technically overly verbose, but it shouldn't really matter. Also, the list
 *  of files that is returned are full paths, relative to the current directory.
 */
sexpr read_directory    (const char *p);

#ifdef __cplusplus
}
#endif

#endif
