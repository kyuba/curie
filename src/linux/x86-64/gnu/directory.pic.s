/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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
    movq last_error_recoverable_p@GOTPCREL(%rip), %rax
    movb $0, (%rax)
    movq $-1, %rax
    leave
    ret
recoverable:
    movq last_error_recoverable_p@GOTPCREL(%rip), %rax
    movb $1, (%rax)
    movq $-1, %rax
    leave
    ret

.section .note.GNU-stack,"",%progbits
