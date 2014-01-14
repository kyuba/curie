/**\file
 * \brief Hash Functions
 *
 * This file will contain a number of hash functions. Hash functions are rather
 * useful in general, and needed for a variety of problems, including some
 * of the basic functionality in curie.
 *
 * The hash functions in this file (should) be in the public domain, as far as
 * I know, anyway.
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
 * \see Projct Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
 */

#ifndef LIBCURIE_HASH_H
#define LIBCURIE_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/int.h>

/**\brief MurmurHash2, 32-Bit
 *
 * \param[in] key  The data to hash.
 * \param[in] len  Length (in bytes) of key.
 * \param[in] seed Start value for the hash.
 *
 * This is the MurmurHash2, written by Austin Appleby. For more details, see
 * http://murmurhash.googlepages.com/ -- this is the website where the hash
 * is available in its original form.
 *
 * This variant in particular is the regular 32-bit version of the hash.
 *
 * \returns The hash over the provided input, as described.
 */
int_32      hash_murmur2_32 ( const void * key, int len, unsigned int seed );

/**\brief MurmurHash2, 64-Bit
 *
 * \param[in] key  The data to hash.
 * \param[in] len  Length (in bytes) of key.
 * \param[in] seed Start value for the hash.
 *
 * This is the MurmurHash2, written by Austin Appleby. For more details, see
 * http://murmurhash.googlepages.com/ -- this is the website where the hash
 * is available in its original form.
 *
 * This variant in particular is either the native 64-bit version, or the
 * version for 32-bit architectures, depending on the current architecture.
 * Thus it's perfectly fine to use on 32-bit platforms.
 *
 * \returns The hash over the provided input, as described.
 */
int_64      hash_murmur2_64 ( const void * key, int len, unsigned int seed );

/**\brief MurmurHash2, Pointer-Sized
 *
 * \param[in] key  The data to hash.
 * \param[in] len  Length (in bytes) of key.
 * \param[in] seed Start value for the hash.
 *
 * This is the MurmurHash2, written by Austin Appleby. For more details, see
 * http://murmurhash.googlepages.com/ -- this is the website where the hash
 * is available in its original form.
 *
 * This variant is either the 32-bit or the 64-bit version of the hash,
 * depending on the pointer size of the target architecture.
 *
 * \returns The hash over the provided input, as described.
 */
int_pointer hash_murmur2_pt ( const void * key, int len, unsigned int seed );

#ifdef __cplusplus
}
#endif

#endif
