/*
 *  linux/syscall.c
 *  libsyscall
 *
 *  Created by Magnus Deininger on 11/12/2008.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _BSD_SOURCE

#include <unistd.h>

#define define_syscall0(a,b,c,r)\
r c () { return (r)syscall(a); }

#define define_syscall1(a,b,c,r,p1)\
r c (p1 a1) { return (r)syscall(a, a1); }

#define define_syscall2(a,b,c,r,p1,p2)\
r c (p1 a1, p2 a2) { return (r)syscall(a, a1, a2); }

#define define_syscall3(a,b,c,r,p1,p2,p3)\
r c (p1 a1, p2 a2, p3 a3) { return (r)syscall(a, a1, a2, a3); }

#define define_syscall4(a,b,c,r,p1,p2,p3,p4)\
r c (p1 a1, p2 a2, p3 a3, p4 a4) { return (r)syscall(a, a1, a2, a3, a4); }

#define define_syscall5(a,b,c,r,p1,p2,p3,p4,p5)\
r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5) { return (r)syscall(a, a1, a2, a3, a4, a5); }

#define define_syscall6(a,b,c,r,p1,p2,p3,p4,p5,p6)\
r c (p1 a1, p2 a2, p3 a3, p4 a4, p5 a5, p6 a6) { return (r)syscall(a, a1, a2, a3, a4, a5, a6); }

#define define_socketcall0(a,b,c,r)
#define define_socketcall1(a,b,c,r,a1)
#define define_socketcall2(a,b,c,r,a1,a2)
#define define_socketcall3(a,b,c,r,a1,a2,a3)
#define define_socketcall4(a,b,c,r,a1,a2,a3,a4)
#define define_socketcall5(a,b,c,r,a1,a2,a3,a4,a5)
#define define_socketcall6(a,b,c,r,a1,a2,a3,a4,a5,a6)

#include <syscall/syscall.h>
