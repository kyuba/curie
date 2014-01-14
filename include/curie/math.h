/**\file
 * \brief Rational numbers
 *
 * This file contains some code to work with rational numbers, which come in
 * rather handy when you don't feel like relying on floating point arithmetic
 * and its various quirks.
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

#ifndef LIBCURIE_MATH_H
#define LIBCURIE_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/int.h>

/**\brief Rational Number
 *
 * Basic type to represent rational numbers.
 *
 * Note that the types were chosen to maximise the range of the numbers, at a
 * slight disadvantage in resolution for the sub-integer portions, thus the
 * sign is only used in the denominator, even though it would appear to be more
 * natural to stick it in the numerator.
 */
struct rational
{
    /**\brief Nominator
     *
     * Nominator of the rational number, i.e. the left/upper part of the
     * fraction.
     */
    int_pointer   numerator;

    /**\brief Denominator
     *
     * Denominator of the rational number, i.e. the lower/right part of the
     * fraction.
     */
    int_pointer_s denominator;
};

/**\brief Calculate the Greatest Common Denominator
 *
 * \param[in] a Integer >= 1.
 * \param[in] b Integer >= 1.
 *
 * Calculate the greatest common denominator of a and b. We're using the
 * euclidian algorithm for this, possibly also special variants depending on
 * the architecture.
 *
 * \returns The greatest common denominator of 'a' and 'b'.
 */
int_64 gcd (int_64 a, int_64 b);

#ifdef __cplusplus
}
#endif

#endif
