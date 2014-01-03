/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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
 *  \brief Syscall Invocation Descriptions (Linux/x86-32/gcc)
 */

#ifndef LIBSYSCALL_INVOCATION_H
#define LIBSYSCALL_INVOCATION_H

/* syscall registers: ebx, ecx, edx, esi, edi, ebp */

#define define_syscall0(a,b,c,r)\
static inline r c ()\
{ register unsigned long out __asm__("eax") = (unsigned long)a;\
  __asm__ volatile ( "int $0x80" : "+a"(out) :\
                     :\
                     "cc", "memory" );\
  return (r)out; }

#define define_syscall1(a,b,c,r,p1)\
static inline r c (p1 a1)\
{ register unsigned long out __asm__("eax") = (unsigned long)a;\
  register unsigned long _a1 __asm__("ebx") = (unsigned long)a1;\
  __asm__ volatile ( "int $0x80" : "+a"(out) :\
                     "b"(_a1) :\
                     "cc", "memory" );\
  return (r)out; }

#define define_syscall2(a,b,c,r,p1,p2)\
static inline r c (p1 a1, p2 a2)\
{ register unsigned long out __asm__("eax") = (unsigned long)a;\
  register unsigned long _a1 __asm__("ebx") = (unsigned long)a1;\
  register unsigned long _a2 __asm__("ecx") = (unsigned long)a2;\
  __asm__ volatile ( "int $0x80" : "+a"(out) :\
                     "b"(_a1), "c"(_a2) :\
                     "cc", "memory" );\
  return (r)out; }

#define define_syscall3(a,b,c,r,p1,p2,p3)\
static inline r c (p1 a1, p2 a2, p3 a3)\
{ register unsigned long out __asm__("eax") = (unsigned long)a;\
  register unsigned long _a1 __asm__("ebx") = (unsigned long)a1;\
  register unsigned long _a2 __asm__("ecx") = (unsigned long)a2;\
  register unsigned long _a3 __asm__("edx") = (unsigned long)a3;\
  __asm__ volatile ( "int $0x80" : "+a"(out) :\
                     "b"(_a1), "c"(_a2), "d"(_a3) :\
                     "cc", "memory" );\
  return (r)out; }

#define define_syscall4(a,b,c,r,p1,p2,p3,p4)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4)\
{ register unsigned long out __asm__("eax") = (unsigned long)a;\
  register unsigned long _a1 __asm__("ebx") = (unsigned long)a1;\
  register unsigned long _a2 __asm__("ecx") = (unsigned long)a2;\
  register unsigned long _a3 __asm__("edx") = (unsigned long)a3;\
  register unsigned long _a4 __asm__("esi") = (unsigned long)a4;\
  __asm__ volatile ( "int $0x80" : "+a"(out) :\
                     "b"(_a1), "c"(_a2), "d"(_a3), "S"(_a4) :\
                     "cc", "memory" );\
  return (r)out; }

#define define_syscall5(a,b,c,r,p1,p2,p3,p4,p5)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5)\
{ register unsigned long out __asm__("eax") = (unsigned long)a;\
  register unsigned long _a1 __asm__("ebx") = (unsigned long)a1;\
  register unsigned long _a2 __asm__("ecx") = (unsigned long)a2;\
  register unsigned long _a3 __asm__("edx") = (unsigned long)a3;\
  register unsigned long _a4 __asm__("esi") = (unsigned long)a4;\
  register unsigned long _a5 __asm__("edi")  = (unsigned long)a5;\
  __asm__ volatile ( "int $0x80" : "+a"(out) :\
                     "b"(_a1), "c"(_a2), "d"(_a3), "S"(_a4),\
                     "D"(_a5) :\
                     "cc", "memory" );\
  return (r)out; }

#define define_syscall6(a,b,c,r,p1,p2,p3,p4,p5,p6)\
static inline r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5, p6 a6)\
{ register unsigned long out __asm__("eax") = (unsigned long)a;\
  register unsigned long _a1 __asm__("ebx") = (unsigned long)a1;\
  register unsigned long _a2 __asm__("ecx") = (unsigned long)a2;\
  register unsigned long _a3 __asm__("edx") = (unsigned long)a3;\
  register unsigned long _a4 __asm__("esi") = (unsigned long)a4;\
  register unsigned long _a5 __asm__("edi")  = (unsigned long)a5;\
  register unsigned long _a6 __asm__("ebp")  = (unsigned long)a6;\
  __asm__ volatile ( "push %%ebp\nint $0x80\npop %%ebp" : "+a"(out) :\
                     "b"(_a1), "c"(_a2), "d"(_a3), "S"(_a4),\
                     "D"(_a5), "R"(_a6) :\
                     "cc", "memory" );\
  return (r)out; }

#endif
