/*
 *  linux-x86-64-gnu/atom.S
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 27/05/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
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

#include <sys/syscall.h>

.globl	_atomic_exit
.globl  _atomic_read
.globl  _atomic_write
.globl  _atomic_open
.globl  _atomic_close
.globl  _atomic_mmap
.globl  _atomic_socketpair
.globl  _atomic_sigaction
.globl  _atomic_kill
.globl  _atomic_select

.globl atomic_main
.globl _start

.type _atomic_exit,       @function
.type _atomic_read,       @function
.type _atomic_write,      @function
.type _atomic_open,       @function
.type _atomic_close,      @function
.type _atomic_mmap,       @function
.type _atomic_socketpair, @function
.type _atomic_sigaction,  @function
.type _atomic_kill,       @function
.type _atomic_select,     @function

.type atomic_main,        @function
.type _start,             @function

// one   two   three four  five  six
// rdi   rsi   rdx   rcx   r8    r9

_atomic_exit:
	movq $__NR_exit, %rax
    jmp syscall1

_atomic_read:
    movq $__NR_read, %rax
    jmp syscall3

_atomic_write:
    movq $__NR_write, %rax
    jmp syscall3

syscall3:
    movq %rdx, %r13
syscall2:
    movq %rsi, %r12
syscall1:
    movq %rdi, %rbx
    syscall
    retq

_start:
    CALL atomic_main
    movq %rax, %rbx
    movq $__NR_exit, %rax
    SYSCALL

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif
