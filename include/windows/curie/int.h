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

#ifndef LIBCURIE_INT_H
#define LIBCURIE_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Unsigned Integer with the Length of a Pointer
 *
 *  On windows we need to trick around a bit, since long is 32-bit, even on
 *  x86-64.
 */
#if defined(__ia64__) || defined(__ia64) || defined(_M_IA64) || defined(_M_X64)
typedef unsigned long long int_pointer;
#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(_M_IX86) || defined(__INTEL__)
typedef unsigned long int_pointer;
#else
typedef unsigned long int_pointer;
#endif

/*! \brief Signed Integer with the Length of a Pointer
 *
 *  Equivalent to int_pointer, but signed.
 */
typedef signed long int_pointer_s;

/*! \brief Unsigned Integer with a Length of 64 Bits
 *
 *  This could be most troublesome between archs. I see trouble for this on at
 *  least DOS/x86 in real mode.
 */
typedef unsigned long long int_64;

/*! \brief Signed Integer with a Length of 64 Bits
 *
 *  Equivalent to int_64, but signed.
 */
typedef signed long long int_64_s;

/*! \brief Unsigned Integer with a Length of 32 Bits
 *
 *  Most archs these days seem to have 32-bit ints.
 */
typedef unsigned int int_32;

/*! \brief Signed Integer with a Length of 32 Bits
 *
 *  Equivalent to int_32, but signed.
 */
typedef signed int int_32_s;

/*! \brief Unsigned Integer with a Length of 16 Bits
 *
 *  This should be true on just about anything, really.
 */
typedef unsigned short int_16;

/*! \brief Signed Integer with a Length of 16 Bits
 *
 *  Equivalent to int_16, but signed.
 */
typedef signed short int_16_s;

/*! \brief Unsigned Integer with a Length of 8 Bits
 *
 *  I know of at least one arch that uses 9-bit chars, but it's rare and the
 *  arch is way old.
 */
typedef unsigned char int_8;

/*! \brief Signed Integer with a Length of 8 Bits
 *
 *  Equivalent to int_8, but signed.
 */
typedef signed char int_8_s;

#ifdef __cplusplus
}
#endif

#endif
