/**\file
 * \brief Syscall Header (Linux)
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

#ifndef LIBSYSCALL_INVOCATION_H
#define LIBSYSCALL_INVOCATION_H

#define _BSD_SOURCE
#define __USE_MISC

#include <unistd.h>
#include <errno.h>

#define define_syscall0(a,b,c,r)\
static inline r c ()\
{ r rv = (r)syscall(a); if (rv == (r)-1) return -errno; return rv; }

#define define_syscall1(a,b,c,r,p1)\
static inline r c (p1 a1)\
{ r rv = (r)syscall(a, a1); if (rv == (r)-1) return -errno; return rv; }

#define define_syscall2(a,b,c,r,p1,p2)\
static inline r c (p1 a1, p2 a2)\
{ r rv = (r)syscall(a, a1, a2); if (rv == (r)-1) return -errno; return rv; }

#define define_syscall3(a,b,c,r,p1,p2,p3)\
static inline r c (p1 a1, p2 a2, p3 a3)\
{ r rv = (r)syscall(a, a1, a2, a3); if (rv == (r)-1) return -errno; return rv; }

#define define_syscall4(a,b,c,r,p1,p2,p3,p4)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4)\
{ r rv = (r)syscall(a, a1, a2, a3, a4);\
  if (rv == (r)-1) return -errno; return rv; }

#define define_syscall5(a,b,c,r,p1,p2,p3,p4,p5)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5)\
{ r rv = (r)syscall(a, a1, a2, a3, a4, a5);\
  if (rv == (r)-1) return -errno; return rv; }

#define define_syscall6(a,b,c,r,p1,p2,p3,p4,p5,p6)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5, p6 a6)\
{ r rv = (r)syscall(a, a1, a2, a3, a4, a5, a6);\
  if (rv == (r)-1) return -errno; return rv; }

#endif
