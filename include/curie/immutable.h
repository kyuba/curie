/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBCURIE_IMMUTABLE_H
#define LIBCURIE_IMMUTABLE_H

/*! \file
 *  \brief Immutable Objects
 *
 *  Making immutable copies of strings may help reduce the likelihood of
 *  memory leaks by making sure all immutable copies only exist exactly once
 *  in the process space, as well as prevent improper access to these strings
 *  (such as inadvertedly modifying them.
 *
 *  For this purpose there are some functions to create immutable copies of
 *  strings and regular data.
 */

/*! \brief Create or find immutable Copy of a String
 *  \param[in] s The string to make an immutable copy of.
 *  \return An immutable copy of the string parameter.
 *
 *  This function will generate an immutable copy of the given string, or return
 *  a pointer to a previously generated immutable copy of the same string.
 *
 *  \note The input string MUST be aligned and padded to eight-byte boundaries!
 */
const char *str_immutable
        (const char *s);

/*! \brief Create immutable Copy of arbitrary Data
 *  \param[in] data   The data to make an immutable copy of.
 *  \param[in] length The length of the data, in bytes.
 *  \return An immutable copy of the data parameter.
 *
 *  Similarly to str_immutable(), non-string data should be storable in the
 *  same way, so here we go.
 *
 *  \note Data need not be aligned and padded to eight-byte boundaries.
 */
const void *immutable
        (const void *data, unsigned long length);

/*! \brief Lock current Memory Pages with immutable Data
 *
 * This function is used to force locking of all the current pages that are used
 * to store new immutable data. the idea is that if you know you wont be storing
 * (much) new stuff anytime soon, you can call this function and immediately get
 * the memory protection effect.
 */
void lock_immutable_pages ( void );

/*! \brief Create or find immutable Copy of a String, regardless of Alignment
 *  \param[in] s The string to make an immutable copy of.
 *  \return An immutable copy of the string parameter.
 *
 * The str_immutable() function expects its parameter to be aligned to an 8-byte
 * boundary, as well as zero-padded to the next 8-byte boundary, unless its a
 * previous return value of itself. this function is a bit of a helper to ensure
 * these alignment-constraints in case you dont know if your input meets these
 * criteria.
 */
const char *str_immutable_unaligned
        (const char *s);

#endif

#ifdef __cplusplus
}
#endif
