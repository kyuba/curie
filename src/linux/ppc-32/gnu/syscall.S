/*
 *  linux-powerpc/syscall.S
 *  libsyscall
 *
 *  Created by Magnus Deininger on 29/12/2008.
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

#define define_syscall0(a,b,c,r)\
.globl c;\
.type c, @function;\
c:\
        li 0, a;\
        sc;\
        blr;

#define define_syscall1(a,b,c,r,p1) define_syscall0(a,b,c,r)
#define define_syscall2(a,b,c,r,p1,p2) define_syscall0(a,b,c,r)
#define define_syscall3(a,b,c,r,p1,p2,p3) define_syscall0(a,b,c,r)
#define define_syscall4(a,b,c,r,p1,p2,p3,p4) define_syscall0(a,b,c,r)
#define define_syscall5(a,b,c,r,p1,p2,p3,p4,p5) define_syscall0(a,b,c,r)
#define define_syscall6(a,b,c,r,p1,p2,p3,p4,p5,p6) define_syscall0(a,b,c,r)

#include <syscall/syscall.h>

.section .note.GNU-stack,"",%progbits
