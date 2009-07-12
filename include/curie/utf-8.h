/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2009, Kyuba Project Members
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

/*!\file
 * \brief Basic UTF-8 Support
 *
 * Just a couple macros and functions to aid in writing code that supports UTF-8
 * encoded strings. For most applications, none of these are needed, they should
 * only be needed in cases where regular C strings might be truncated.
 */

#ifndef LIBCURIE_UTF_8_H
#define LIBCURIE_UTF_8_H

#include <curie/int.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!\brief Read a character from a UTF-8 encoded string
 * \param[in]  b UTF-8 encoded string
 * \param[in]  p Current position in the string
 * \param[out] c The character that was read
 * \return The new position in the string
 *
 * It's pretty much undefined what happens on mangled/truncated utf-8 sequences,
 * but in general the result will be a bogus character or 0. For premature
 * end-of-string markers, this version of the function sets c to 0 and it will
 * will return 'p' unmodified.
 */
unsigned int utf8_get_character (const int_8 *b, unsigned int p, int_32 *c);

/*!\brief Encode Character in UTF-8
 * \param[out] b A buffer to put the character into.
 * \param[in]  c The character to encode.
 * \return The number of bytes used in the buffer.
 *
 * As per RFC 3629, the buffer won't exceed 4 bytes in length. This means that,
 * if c is larger than 0x10ffff, some data may be discarded if it would require
 * a 5- or 6-byte encoding.
 */
unsigned int utf8_encode (int_8 *b, unsigned int c);

/*!\brief Is the given byte an UTF-8 multibyte sequence start character?
 * \param[in] c Character to test
 * \return Something that evaluates to true if the condition holds, and false
 *         otherwise.
 */
#define utf8_mbsp(c) (((unsigned char)c & 0xc0) == 0xc0)

/*!\brief Is the given byte an UTF-8 multibyte sequence tail character?
 * \param[in] c Character to test
 * \return Something that evaluates to true if the condition holds, and false
 *         otherwise.
 */
#define utf8_mbmp(c) (((unsigned char)c & 0xc0) == 0x80)

/*!\brief Is the given byte an UTF-8 multibyte sequence character?
 * \param[in] c Character to test
 * \return Something that evaluates to true if the condition holds, and false
 *         otherwise.
 */
#define utf8_mbcp(c)  ((unsigned char)c & 0x80)

#ifdef __cplusplus
}
#endif

#endif
