/*
 *  linux-x86-64-gnu/exec-system.S
 *  libcurie
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

.globl  a_fork
.globl  a_exec
.globl  a_wait

.type a_fork,                    @function
.type a_exec,                    @function
.type a_wait,                    @function

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

a_fork:
    pushq   %rbp
    movq    %rsp, %rbp

    movq $57, %rax /* sys_fork */

    syscall
    leave
    ret

a_exec:
    pushq   %rbp
    movq    %rsp, %rbp

    movq $59, %rax /* sys_execve */

    syscall
    leave
    ret

a_wait:
    pushq   %rbp
    movq    %rsp, %rbp

    movq $61, %rax /* sys_wait4 */
    movq $1, %rdx /* WNOHNANG */
    movq $0, %r10 /* we don't care about the rusage field */

    syscall

    cmp %rax, %rdi /* see if the return value is the pid we passed */
    jz wait_examine

    movq $0, %rax /* wr_running */
    leave
    ret

wait_examine:
    movl (%rsi), %eax
    and $0x7f, %eax
    jz exited_normally /* normal exit if that mask is 0 */

    movq $2, %rax /* wr_killed */
    leave
    ret

exited_normally:
    pushq %rbx

    movl (%rsi), %eax
    xor %rbx, %rbx

    movb  %ah, %bl /* use the high-order bits of the lower 16 bits only */
    movl %ebx, (%rsi)

    popq %rbx

    movq $1, %rax /* wr_exited */
    leave
    ret


#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

