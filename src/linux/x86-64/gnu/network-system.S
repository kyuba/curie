/*
 *  linux-x86-64-gnu/network-system.S
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 10/08/2008.
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

.globl __a_unix_socketpair
    .type __a_unix_socketpair,     @function

.globl __a_accept
    .type __a_accpt,               @function

.globl __a_unix_socket
    .type __a_unix_socket,         @function

.globl __a_listen
    .type __a_listen,              @function

.globl __a_connect
    .type __a_connect,             @function

.globl __a_bind
    .type __a_bind,                @function

.text
        .align 8

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

__a_unix_socketpair:
        pushq   %rbp
        movq    %rsp, %rbp

        movq    $53, %rax /* sys_socketpair */
        movq    %rdi, %r10 /* the result pointer */
        movq    $1, %rdi /* AF_UNIX */
        movq    $1, %rsi /* SOCK_STREAM */
        movq    $0, %rdx
        syscall

        leave
        ret

__a_unix_socket:
        pushq   %rbp
        movq    %rsp, %rbp

        movq    $41, %rax /* sys_socket */
        movq    $1, %rdi /* AF_UNIX */
        movq    $1, %rsi /* SOCK_STREAM */
        movq    $0, %rdx

        jmp syscall_check

__a_listen:
        pushq   %rbp
        movq    %rsp, %rbp

        movq    $50, %rax /* sys_listen */
        movq    $32, %rsi /* backlog */

        jmp syscall_check

__a_connect:
        pushq   %rbp
        movq    %rsp, %rbp

        movq    $42, %rax /* sys_connect */

        jmp syscall_check

__a_bind:
        pushq   %rbp
        movq    %rsp, %rbp

        movq    $49, %rax /* sys_connect */

        jmp syscall_check

__a_accept:
        pushq   %rbp
        movq    %rsp, %rbp

        movq    $43, %rax /* sys_accept */
        movq    $0, %rsi
        movq    $0, %rdx

syscall_check:
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
        movq $-1, %rax
        leave
        ret
recoverable:
        movq $-2, %rax
        leave
        ret

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

