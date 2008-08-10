/*
 *  linux-x86-64-gnu/signal-system.S
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

.text
        .align 8

.globl  __a_set_signal_handler
        .type __a_set_signal_handler, @function
.globl  a_getpid
        .type a_getpid,          @function
.globl  __a_send_signal
        .type __a_send_signal,   @function

.globl  __a_sigreturn
        .type __a_sigreturn,     @function

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

__a_set_signal_handler:
    pushq   %rbp
    movq    %rsp, %rbp

    movq $13, %rax /* sys_sigaction */
    /* %rdi and %rsi are inherited */
    movq $0, %rdx /* don't care about the old handler */
    movq $8, %r10 /* sizeof(sigset_t) */

    syscall
    leave
    ret

__a_send_signal:
    pushq   %rbp
    movq    %rsp, %rbp

    movq $62, %rax /* sys_kill */
    syscall
    leave
    ret

a_getpid:
    pushq   %rbp
    movq    %rsp, %rbp

    movq $39, %rax /* sys_getpid */
    syscall
    leave
    ret

__a_sigreturn:
    movq $15, %rax /* sys_sigreturn */
    syscall
    ret

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

