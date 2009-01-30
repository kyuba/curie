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

.text
        .align 8

.globl  a_open_directory
.globl  a_getdents64

.type a_open_directory,          @function
.type a_getdents64,              @function

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

a_open_directory:
    /* %rdi is inherited from the callee */
    movq $2, %rax /* sys_open */
    movq $0x10800, %rsi /* O_RDONLY | O_NONBLOCK | O_DIRECTORY */
    jmp syscall_with_cleanup

a_getdents64:
    movq $217, %rax /* sys_getdents64 */

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
    movb $0, last_error_recoverable_p@GOTPCREL(%rip)
    movq $-1, %rax
    leave
    ret
recoverable:
    movb $1, last_error_recoverable_p@GOTPCREL(%rip)
    movq $-1, %rax
    leave
    ret

.section .note.GNU-stack,"",%progbits
