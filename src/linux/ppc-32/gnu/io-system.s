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

.globl last_error_recoverable_p
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

/* C-functions: */
/* r3+ ? */
/* kernel: */
/* r3 r4 r5 r6 r7 ... ? */

a_read:
    li      0, 3 /* sys_read */
    b       syscall_with_cleanup

a_write:
    li      0, 4 /* sys_write */
    b       syscall_with_cleanup

a_open_read:
    li      0, 5 /* sys_open */
    li      4, 0x800 /* O_RDONLY | O_NONBLOCK */
    b       syscall_with_cleanup

a_open_write:
    li      0, 5 /* sys_open */
    li      4, 0x841 /* O_WRONLY | O_NONBLOCK | O_CREAT */
    li      5, 0x1b6
    b       syscall_with_cleanup

a_close:
    li      0, 6 /* sys_close */
    b       syscall_with_cleanup

a_create:
    li      0, 5 /* sys_open */
    mr      5, 4
    li      4, 0x841 /* O_WRONLY | O_NONBLOCK | O_CREAT */
    b       syscall_with_cleanup

a_dup:
    li      0, 63 /* sys_dup2 */
    b       syscall_with_cleanup

a_dup_n:
    li      0, 41 /* sys_dup */
    b       syscall_with_cleanup

a_stat:
    li      0, 106 /* sys_stat */
    b       syscall_with_cleanup

a_lstat:
    li      0, 107 /* sys_lstat */
    b       syscall_with_cleanup

a_make_nonblocking:
    li      0, 55 /* sys_fcntl */
    li      4, 4 /* F_SETFL */
    li      5, 0x800 /* O_NONBLOCK */
    b       syscall_with_cleanup

a_unlink:
    li      0, 10 /* sys_unlink */

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
