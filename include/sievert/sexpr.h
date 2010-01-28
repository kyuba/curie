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

/*! \defgroup sexpr S-Expressions
 *  \brief Symbolic Expression Handling
 */

/*! \file
 *  \ingroup sexpr
 *  \brief S-expression Library Functions
 *
 *  The split between what's an essential function and what is part of a more
 *  high-level part of the typical library may seem fairly arbitrary; I'm
 *  trying to put the rarely used functions in libsievert to clean up the
 *  symbol space a bit.
 */

#ifndef LIBSIEVERT_SEXPR_H
#define LIBSIEVERT_SEXPR_H

#include <curie/sexpr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief List Map
 *  \param[in] list The list to map.
 *  \param[in] f    The function to apply.
 *
 *  This function applies the given function to each element of f.
 */
void sx_list_map (sexpr list, void (*f)(sexpr));

/*! \brief List Fold
 *  \param[in] list The list to fold.
 *  \param[in] f    The function to apply.
 *  \param[in] seed The seed value to use.
 *  \return The value constructed by f().
 *
 *  This function applies the given function to each element of f, passing the
 *  return value of f to the next function call and ultimately returning the
 *  return value of the last call to f().
 */
sexpr sx_list_fold (sexpr list, sexpr (*f)(sexpr, sexpr), sexpr seed);

#ifdef __cplusplus
}
#endif

#endif
