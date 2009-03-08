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
 *  \brief String Handling
 *
 *  Helper functions to deal with strings.
 */

#ifndef LIBCURIE_STRING_H
#define LIBCURIE_STRING_H

#include <curie/int.h>

/* kudos to markos for this one */

/*! \brief Hash a String
 *  \param[in]  data The string.
 *  \param[out] len  The calculated length of the string.
 *  \return 32-bit hash of the string.
 *
 *  This function calculates a 32-bit hash over any C-style string. Handy for
 *  some internal things, such as make_string() and make_symbol() to keep track
 *  of strings that have already been created.
 */
int_32 str_hash(/*@notnull@*/ const char *data,
                /*@notnull@*/ /*@out@*/ unsigned long *len);

#endif

/*! @} */
