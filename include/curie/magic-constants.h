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
 *  \brief Magic Constants (Internal)
 *
 *  I got these constants on forums.gentoo.org in a thread on how to isolate
 *  zeroes in an integer. They've been devised by Akkara.
 */

#ifndef LIBCURIE_MAGIC_CONSTANTS_H
#define LIBCURIE_MAGIC_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define HASH_MAGIC_MULTIPLIER 0x0458654Du
#define HASH_MAGIC_SHIFT (32 - 6)
#define HASH_MAGIC_TABLE ((int_8 const *)(\
            "\xFF\00\01\15\02\06\16\00\03\00\23\07\17\32\00\00"\
            "\36\04\00\30\26\24\10\00\12\20\33\00\00\00\00\00"\
            "\37\14\05\00\00\22\31\00\35\27\25\00\11\00\00\00"\
            "\13\00\21\00\34\00\00\00\00\00\00\00\00\00\00\00"))
#define HASH_MAGIC_TABLE_MASK 0x3F

#ifdef __cplusplus
}
#endif

#endif
