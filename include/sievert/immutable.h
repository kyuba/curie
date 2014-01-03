/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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

#ifndef LIBSIEVERT_IMMUTABLE_H
#define LIBSIEVERT_IMMUTABLE_H

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

#endif

#ifdef __cplusplus
}
#endif
