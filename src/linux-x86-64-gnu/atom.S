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

#include <asm/unistd.h>

.globl  last_error_recoverable_p
        .data
        .type last_error_recoverable_p @object
        .size last_error_recoverable_p, 1

last_error_recoverable_p:
        .byte 1

.text
        .align 8

.globl	_atomic_exit
        .type _atomic_exit,            @function
.globl  _atomic_read
        .type _atomic_read,            @function
.globl  _atomic_write
        .type _atomic_write,           @function
.globl  _atomic_open_read
        .type _atomic_open_read,       @function
.globl  _atomic_open_write
        .type _atomic_open_write,      @function
.globl  _atomic_create
        .type _atomic_create,          @function
.globl  _atomic_close
        .type _atomic_close,           @function
.globl  _atomic_kill
        .type _atomic_kill,            @function

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

_atomic_exit:
    pushq   %rbp
    movq    %rsp, %rbp
    movq $__NR_exit, %rax
    jmp syscall_with_cleanup

_atomic_read:
    pushq   %rbp
    movq    %rsp, %rbp
    movq $__NR_read, %rax
    jmp syscall_with_cleanup

_atomic_write:
    pushq   %rbp
    movq    %rsp, %rbp
    movq $__NR_write, %rax
    jmp syscall_with_cleanup

_atomic_open_read:
    pushq   %rbp
    movq    %rsp, %rbp
    /* %rdi is inherited from the callee */
    movq $__NR_open, %rax
    movq $0x800, %rsi /* O_RDONLY | O_NONBLOCK */
    jmp syscall_with_cleanup

_atomic_open_write:
    pushq   %rbp
    movq    %rsp, %rbp
    /* %rdi is inherited from the callee */
    movq $__NR_open, %rax
    movq $0x841, %rsi /* O_WRONLY | O_NONBLOCK | O_CREAT */
    movq $0x1b6, %rdx
    jmp syscall_with_cleanup

_atomic_close:
    pushq   %rbp
    movq    %rsp, %rbp
    /* %rdi is inherited from the callee */
    movq $__NR_close, %rax
    jmp syscall_with_cleanup

_atomic_create:
    pushq   %rbp
    movq    %rsp, %rbp
    /* %rdi is inherited from the callee */
    movq $__NR_open, %rax
    movq %rsi, %rdx
    movq $0x841, %rsi /* O_WRONLY | O_NONBLOCK | O_CREAT */
    jmp syscall_with_cleanup

syscall_with_cleanup:
    syscall
    cmp $0, %rax
    js negative_result
    leave
    ret
negative_result:
    cmp $-11, %rax /* EAGAIN, as well as EWOULDBLOCK*/
    jz recoverable
    cmp $-4, %rax /* EINTR */
    jz recoverable
    movb $0, last_error_recoverable_p(%rip)
    movq $-1, %rax
    leave
    ret
recoverable:
    movb $1, last_error_recoverable_p(%rip)
    movq $-1, %rax
    leave
    ret

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

