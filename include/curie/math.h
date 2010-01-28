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

/*! \file
 *  \brief Specific-length integers
 *
 *  This file is rather platform/architecture specific, so this header file
 *  may need porting, depending on the target architecture. for this very
 *  purpose, the build system is capable of feeding the compiler with
 *  appropriate path information to allow overrides in the include/ directory.
 *  this means, instead of mucking with wickedass macromagic, just add an
 *  appropriate directory under include/ and copy this file there.
 *
 *  This specific file is the generic version which so far seems to work across
 *  all the tested targets.
 */

#ifndef LIBCURIE_MATH_H
#define LIBCURIE_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/int.h>

/*! \brief Rational Number
 *
 *  Basic type to represent rational numbers.
 *
 *  Note that the types were chosen to maximise the range of the numbers, at a
 *  slight disadvantage in resolution for the sub-integer portions, thus the
 *  sign is only used in the denominator, even though it would appear to be more
 *  natural to stick it in the numerator.
 */
struct rational
{
    /*! \brief Nominator
     *
     *  Nominator of the rational number, i.e. the left/upper part of the
     *  fraction.
     */
    int_pointer   numerator;

    /*! \brief Denominator
     *
     *  Denominator of the rational number, i.e. the lower/right part of the
     *  fraction.
     */
    int_pointer_s denominator;
};

/*! \brief Calculate the Greatest Common Denominator
 *  \param[in] a Integer >= 1.
 *  \param[in] b Integer >= 1.
 *
 *  Calculate the greatest common denominator of a and b. We're using the
 *  euclidian algorithm for this, possibly also special variants depending on
 *  the architecture.
 */
int_64 gcd (int_64 a, int_64 b);

#ifdef __cplusplus
}
#endif

#endif
