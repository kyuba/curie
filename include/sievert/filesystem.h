/**\file
 * \brief Additional Filesystem Functions
 *
 * Contains functions to clean up path names.
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

#ifndef LIBSIEVERT_FILESYSTEM_H
#define LIBSIEVERT_FILESYSTEM_H

#include <curie/filesystem.h>

#ifdef __cplusplus
extern "C" {
#endif

/**\brief Normalise path(s)
 *
 * Given a string, this function will remove any 'unnecessary' parts when the
 * string is interpreted as a file name, i.e. sequences of '/'-es are merged
 * into a single '/' and './' portions are removed.
 *
 * Given a list, the function is applied recursively to all elements of this
 * list.
 *
 * \param[in] path The path or paths to normalise.
 *
 * \returns The normalised path or paths.
 */
sexpr path_normalise        (sexpr path);

/**\brief Normalise, then remove longest common prefix
 *
 * Uses path_normalise() to normalise the given paths, then uses path_prefix()
 * to find the longest common prefix. Finally, it removes this longest prefix
 * from the normalised paths.
 *
 * \param[in] path A list of paths to clean up.
 *
 * \returns List of normalised paths with the longest common prefix removed.
 */
sexpr path_normalise_prefix (sexpr path);

/**\brief Obtain longest common prefix
 *
 * Search for the longest common prefix that occurs in all elements of a list of
 * path names. Only prefixes ending in '/' or '\\' are recognised.
 *
 * \param[in] list The list to process; must be a list of strings.
 *
 * \returns Longest common prefix, as a string.
 */
sexpr path_prefix           (sexpr list);

#ifdef __cplusplus
}
#endif

#endif

