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
 *  \brief Syscall Invocation Descriptions (Linux/ppc-32/gcc)
 */

#ifndef LIBSYSCALL_INVOCATION_H
#define LIBSYSCALL_INVOCATION_H

/* syscall registers: r3 and following, return value should be in r3 */

#define define_syscall0(a,b,c,r)\
static inline r c ()\
{ register r _a __asm__("3") = a;\
  __asm__ volatile ( "sc" : "=g"(_a) : "0"(_a) : "memory" );\
  return _a; }

#define define_syscall1(a,b,c,r,p1)\
static inline r c (p1 a1)\
{ register r _a __asm__("3") = a;\
  register p1 _a1 __asm__("4") = a1;\
  __asm__ volatile ( "sc" : "=g"(_a) : "0"(_a), "g"(_a1) : "memory" );\
  return _a; }

#define define_syscall2(a,b,c,r,p1,p2)\
static inline r c (p1 a1, p2 a2)\
{ register r _a __asm__("3") = a;\
  register p1 _a1 __asm__("4") = a1;\
  register p2 _a2 __asm__("5") = a1;\
  __asm__ volatile ( "sc" : "=g"(_a) : "0"(_a), "g"(_a1), "g"(_a2) :\
                     "memory" );\
  return _a; }

#define define_syscall3(a,b,c,r,p1,p2,p3)\
static inline r c (p1 a1, p2 a2, p3 a3)\
{ register r _a __asm__("3") = a;\
  register p1 _a1 __asm__("4") = a1;\
  register p2 _a2 __asm__("5") = a2;\
  register p3 _a3 __asm__("6") = a3;\
  __asm__ volatile ( "sc" : "=g"(_a) : "0"(_a), "g"(_a1), "g"(_a2), "g"(_a3) :\
                     "memory" );\
  return _a; }

#define define_syscall4(a,b,c,r,p1,p2,p3,p4)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4)\
{ register r _a __asm__("3") = a;\
  register p1 _a1 __asm__("4") = a1;\
  register p2 _a2 __asm__("5") = a2;\
  register p3 _a3 __asm__("6") = a3;\
  register p4 _a4 __asm__("7") = a4;\
  __asm__ volatile ( "sc" : "=g"(_a) : "0"(_a), "g"(_a1), "g"(_a2), "g"(_a3),\
                                       "g"(_a4) :\
                     "memory" );\
  return _a; }

#define define_syscall5(a,b,c,r,p1,p2,p3,p4,p5)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5)\
{ register r _a __asm__("3") = a;\
  register p1 _a1 __asm__("4") = a1;\
  register p2 _a2 __asm__("5") = a2;\
  register p3 _a3 __asm__("6") = a3;\
  register p4 _a4 __asm__("7") = a4;\
  register p5 _a5 __asm__("8") = a5;\
  __asm__ volatile ( "sc" : "=g"(_a) : "0"(_a), "g"(_a1), "g"(_a2), "g"(_a3),\
                                       "g"(_a4), "g"(_a5) :\
                     "memory" );\
  return _a; }

#define define_syscall6(a,b,c,r,p1,p2,p3,p4,p5,p6)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5, p6 a6)\
{ register r _a __asm__("3") = a;\
  register p1 _a1 __asm__("4") = a1;\
  register p2 _a2 __asm__("5") = a2;\
  register p3 _a3 __asm__("6") = a3;\
  register p4 _a4 __asm__("7") = a4;\
  register p5 _a5 __asm__("8") = a5;\
  register p6 _a6 __asm__("9") = a6;\
  __asm__ volatile ( "sc" : "=g"(_a) : "0"(_a), "g"(_a1), "g"(_a2), "g"(_a3),\
                                       "g"(_a4), "g"(_a5), "g"(_a6) :\
                     "memory" );\
  return _a; }

#define define_socketcall0(a,b,c,r)
#define define_socketcall1(a,b,c,r,a1)
#define define_socketcall2(a,b,c,r,a1,a2)
#define define_socketcall3(a,b,c,r,a1,a2,a3)
#define define_socketcall4(a,b,c,r,a1,a2,a3,a4)
#define define_socketcall5(a,b,c,r,a1,a2,a3,a4,a5)
#define define_socketcall6(a,b,c,r,a1,a2,a3,a4,a5,a6)

#endif
