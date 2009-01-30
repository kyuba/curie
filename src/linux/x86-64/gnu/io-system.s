/*
 *  linux-x86-64-gnu/io-system.S
 *  libcurie
 *
 *  Created by Magnus Deininger on 27/05/2008.
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

.globl  last_error_recoverable_p
        .data
        .type last_error_recoverable_p, @object
        .size last_error_recoverable_p, 1

last_error_recoverable_p:
        .zero 1

.text
        .align 8

.globl  a_read
.globl  a_write
.globl  a_open_read
.globl  a_open_write
.globl  a_create
.globl  a_close
.globl  a_dup
.globl  a_dup_n
.globl  a_stat
.globl  a_lstat
.globl  a_make_nonblocking
.globl  a_unlink

.type a_read,                    @function
.type a_write,                   @function
.type a_open_read,               @function
.type a_open_write,              @function
.type a_create,                  @function
.type a_close,                   @function
.type a_dup,                     @function
.type a_dup_n,                   @function
.type a_stat,                    @function
.type a_lstat,                   @function
.type a_make_nonblocking,        @function
.type a_unlink,                  @function

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

a_read:
    movq $0, %rax /* sys_read */
    jmp syscall_with_cleanup

a_write:
    movq $1, %rax /* sys_write */
    jmp syscall_with_cleanup

a_open_read:
    /* %rdi is inherited from the callee */
    movq $2, %rax /* sys_open */
    movq $0x800, %rsi /* O_RDONLY | O_NONBLOCK */
    jmp syscall_with_cleanup

a_open_write:
    /* %rdi is inherited from the callee */
    movq $2, %rax /* sys_open */
    movq $0x841, %rsi /* O_WRONLY | O_NONBLOCK | O_CREAT */
    movq $0x1b6, %rdx
    jmp syscall_with_cleanup

a_close:
    /* %rdi is inherited from the callee */
    movq $3, %rax /* sys_close */
    jmp syscall_with_cleanup

a_create:
    /* %rdi is inherited from the callee */
    movq $2, %rax /* sys_open */
    movq %rsi, %rdx
    movq $0x841, %rsi /* O_WRONLY | O_NONBLOCK | O_CREAT */
    jmp syscall_with_cleanup

a_dup:
    /* %rdi and %si are inherited from the callee */
    movq $33, %rax /* sys_dup2 */
    jmp syscall_with_cleanup

a_dup_n:
    movq $32, %rax /* sys_dup */
    jmp syscall_with_cleanup

a_stat:
    movq $4, %rax /* sys_newstat */
    jmp syscall_with_cleanup

a_lstat:
    movq $6, %rax /* sys_newlstat */
    jmp syscall_with_cleanup

a_make_nonblocking:
    movq $72, %rax /* sys_fcntl */
    movq $4, %rsi /* F_SETFL */
    movq $0x800, %rdx /* O_NONBLOCK */
    jmp syscall_with_cleanup

a_unlink:
    movq $87, %rax /* sys_unlink */

syscall_with_cleanup:
    pushq   %rbp
    movq    %rsp, %rbp

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

.section .note.GNU-stack,"",%progbits
