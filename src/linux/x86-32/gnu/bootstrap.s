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
    .size curie_argv, 4
curie_argv:
        .long 0x0

.globl curie_environment
    .type curie_environment, @object
    .size curie_environment, 4
curie_environment:
        .long 0x0

.text
    .align 8

.globl _start
    .type _start,             @function
.globl cexit
    .type cexit,              @function

_start:
        /* Fox! Clear the frame pointer! */
        xorl    %ebp, %ebp

        movl    %esp, %ecx
        addl    $0x04, %ecx
        movl    %ecx, $curie_argv

        movl    (%esp), %esi
        incl    %esi
        imull   $0x04, %esi, %esi
        addl    %esi, %ecx
        movl    %ecx, $curie_environment

        call    initialise_stack
        call    cmain

        movl    %eax, %ebx

cexit:
        movl    $1, %eax /* sys_exit */
        int     $0x80

.section .note.GNU-stack,"",%progbits
