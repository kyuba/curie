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

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

#define define_syscall0(a,b,c,r)\
.globl c;\
.type c, @function;\
c:\
        movq    $a, %rax;\
        syscall;\
        ret;

#define define_syscall4(a,b,c,r,p1,p2,p3,p4)\
.globl c;\
.type c, @function;\
c:\
        movq    %rcx, %r10;\
        movq    $a, %rax;\
        syscall;\
        ret;

#define define_syscall1(a,b,c,r,p1) define_syscall0(a,b,c,r)
#define define_syscall2(a,b,c,r,p1,p2) define_syscall0(a,b,c,r)
#define define_syscall3(a,b,c,r,p1,p2,p3) define_syscall0(a,b,c,r)
#define define_syscall5(a,b,c,r,p1,p2,p3,p4,p5) define_syscall4(a,b,c,r,p1,p2,p3,p4)
#define define_syscall6(a,b,c,r,p1,p2,p3,p4,p5,p6) define_syscall4(a,b,c,r,p1,p2,p3,p4)

#define define_socketcall0(a,b,c,r)
#define define_socketcall1(a,b,c,r,a1)
#define define_socketcall2(a,b,c,r,a1,a2)
#define define_socketcall3(a,b,c,r,a1,a2,a3)
#define define_socketcall4(a,b,c,r,a1,a2,a3,a4)
#define define_socketcall5(a,b,c,r,a1,a2,a3,a4,a5)
#define define_socketcall6(a,b,c,r,a1,a2,a3,a4,a5,a6)

#include <syscall/syscall.h>

.section .note.GNU-stack,"",%progbits
