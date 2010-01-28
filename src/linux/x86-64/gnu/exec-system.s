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

.text
        .align 8

.globl  a_fork
.globl  a_exec
.globl  a_wait
.globl  a_wait_all
.globl  a_set_sid

.type a_fork,                    @function
.type a_exec,                    @function
.type a_wait,                    @function
.type a_wait_all,                @function
.type a_set_sid,                 @function

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


a_wait_all:
    pushq   %rbp
    movq    %rsp, %rbp

    movq %rdi, %rsi /* arg1 -> arg2 */
    movq $-1, %rdi /* wait for anything */
    movq $61, %rax /* sys_wait4 */
    movq $1, %rdx /* WNOHNANG */
    movq $0, %r10 /* we don't care about the rusage field */

    syscall

    pushq %rbx
    pushq %rcx

    movl (%rsi), %ecx
    xor %rbx, %rbx

    movb  %ch, %bl /* use the high-order bits of the lower 16 bits only */
    movl %ebx, (%rsi)

    popq %rcx
    popq %rbx

    leave
    ret

a_set_sid:
    pushq   %rbp
    movq    %rsp, %rbp

    movq $112, %rax /* sys_setsid */

    syscall
    leave
    ret

.section .note.GNU-stack,"",%progbits
