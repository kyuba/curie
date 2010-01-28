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

.data

.globl curie_argv
    .type curie_argv, @object
    .size curie_argv, 8
curie_argv:
        .quad 0x0

.globl curie_environment
    .type curie_environment, @object
    .size curie_environment, 8
curie_environment:
        .quad 0x0

.text
    .align 8

.globl _start
    .type _start,             @function
.globl cexit
    .type cexit,              @function


_start:
        /* play dat funkeh music white boy */
        xorq    %rbp, %rbp

        movq    %rsp, %r11
        addq    $0x8, %r11
        movq    %r11, curie_argv(%rip)

        movq    (%rsp), %rbx
        incq    %rbx
        imulq   $0x8, %rbx, %rbx
        addq    %rbx, %r11
        movq    %r11, curie_environment(%rip)

        call    initialise_stack

        call    cmain
        movq    %rax, %rdi
cexit:
        movq    $0x3c, %rax /* sys_exit */
        syscall

.section .note.GNU-stack,"",%progbits
