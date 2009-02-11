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

.data

.text
        .align 8

.globl  a_open_directory
.globl  a_getdents64

.type a_open_directory,          @function
.type a_getdents64,                @function

/* C-functions: */
/* r3+ ? */
/* kernel: */
/* r3 r4 r5 r6 r7 ... ? */

a_open_directory:
    li      0, 5 /* sys_open */
    li      4, 0x4800 /* O_RDONLY | O_NONBLOCK | O_DIRECTORY */
    b       syscall_with_cleanup

a_getdents64:
    li      0, 202 /* sys_getdents64 */

syscall_with_cleanup:
    sc
    cmpwi   3, 0
    blt     negative_result
    blr
negative_result:
    cmpwi   3, -11 /* EAGAIN, as well as EWOULDBLOCK */
    beq     recoverable
    cmpwi   3, -4 /* EINTR */
    beq     recoverable
    li      4, 0
    lis     3, last_error_recoverable_p@ha
    stb     4, last_error_recoverable_p@l(3)
    li      3, -1
    blr
recoverable:
    li      4, 1
    lis     3, last_error_recoverable_p@ha
    stb     4, last_error_recoverable_p@l(3)
    li      3, -1
    blr

.section .note.GNU-stack,"",%progbits
