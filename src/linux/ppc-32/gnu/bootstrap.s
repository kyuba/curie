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

.set STACKSIZE, 0x4000

.globl __libc_stack_end
.comm __libc_stack_end, STACKSIZE, 32

.globl curie_argv
.globl curie_environment

curie_argv:
        .long 0x0

curie_environment:
        .long 0x0

.globl _start
.globl cexit

.type _start,                    @function
.type cexit,                     @function

.text

_start:
        cmpwi   3, 0
        bne     _3

        mr      3, 1

        li      4, 0

_1:
        addi    1, 1, 4
        addi    4, 4, 1
        lwz     9, 0(1)
        cmpwi   9, 0
        bne     _1

        li      5, 0

_2:
        addi    1, 1, 4
        addi    5, 5, 1
        lwz     9, 0(1)
        cmpwi   9, 0
        bne     _2

        mr      1, 3
        addi    1, 1, -16

        bl      __do_startup
        mr      1, 3
        b       _4

_3:
        lis     16, curie_argv@ha
        stw     4, curie_argv@l(16)
        lis     16, curie_environment@ha
        stw     5, curie_environment@l(16)

_4:
        lis     1, __libc_stack_end@ha
        addi    1, 1, __libc_stack_end@l
        addi    1, 1, STACKSIZE

        bl      initialise_stack
        bl      cmain

cexit:
        li      0, 1
        sc

.section .note.GNU-stack,"",%progbits
