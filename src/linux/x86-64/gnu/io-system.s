/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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
.globl __sccl

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
.type __sccl,                    @function

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

a_read:
    movq $0, %rax /* sys_read */
    jmp __sccl

a_write:
    movq $1, %rax /* sys_write */
    jmp __sccl

a_open_read:
    /* %rdi is inherited from the callee */
    movq $2, %rax /* sys_open */
    movq $0x800, %rsi /* O_RDONLY | O_NONBLOCK */
    jmp __sccl

a_open_write:
    /* %rdi is inherited from the callee */
    movq $2, %rax /* sys_open */
    movq $0x841, %rsi /* O_WRONLY | O_NONBLOCK | O_CREAT */
    movq $0x1b6, %rdx
    jmp __sccl

a_close:
    /* %rdi is inherited from the callee */
    movq $3, %rax /* sys_close */
    jmp __sccl

a_create:
    /* %rdi is inherited from the callee */
    movq $2, %rax /* sys_open */
    movq %rsi, %rdx
    movq $0x841, %rsi /* O_WRONLY | O_NONBLOCK | O_CREAT */
    jmp __sccl

a_dup:
    /* %rdi and %si are inherited from the callee */
    movq $33, %rax /* sys_dup2 */
    jmp __sccl

a_dup_n:
    movq $32, %rax /* sys_dup */
    jmp __sccl

a_stat:
    movq $4, %rax /* sys_newstat */
    jmp __sccl

a_lstat:
    movq $6, %rax /* sys_newlstat */
    jmp __sccl

a_make_nonblocking:
    movq $72, %rax /* sys_fcntl */
    movq $4, %rsi /* F_SETFL */
    movq $0x800, %rdx /* O_NONBLOCK */
    jmp __sccl

a_unlink:
    movq $87, %rax /* sys_unlink */

__sccl:
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
