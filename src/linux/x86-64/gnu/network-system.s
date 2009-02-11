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

.globl __a_unix_socketpair
.globl __a_accept
.globl __a_unix_socket
.globl __a_listen
.globl __a_connect
.globl __a_bind

.type __a_unix_socketpair,         @function
.type __a_accept,                  @function
.type __a_unix_socket,             @function
.type __a_listen,                  @function
.type __a_connect,                 @function
.type __a_bind,                    @function

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

        movq    $49, %rax /* sys_bind */

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

.section .note.GNU-stack,"",%progbits
