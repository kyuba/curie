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

a_fork:
        li      0, 2 /* sys_fork */
        sc
        blr

a_exec:
        li      0, 11 /* sys_execve */
        sc
        blr

a_wait:
        li      0, 114 /* sys_wait4 */
        mr      15, 3
        mr      16, 4
        li      5, 1 /* WNOHNANG */
        li      6, 0 /* we don't care about the rusage field */

        sc

        cmpw    3, 15 /* see if the return value is the pid we passed */
        beq     wait_examine

        li      3, 0 /* wr_running */
        blr

wait_examine:
        lwz     3, 0(16)
        andi.   3, 3, 0x7f
        beq     exited_normally /* normal exit if that mask is 0 */

        li      3, 2 /* wr_killed */
        blr

exited_normally:
        lwz     3, 0(16)
        li      4, 0

        rotrwi  3, 3, 8 /* use the high-order bits of the lower 16 bits only */
        andi.   3, 3, 0x7f

        stw     3, 0(16)

        li      3, 1 /* wr_exited */
        blr


a_wait_all:
        li      0, 114 /* sys_wait4 */
        mr      16, 3
        mr      4, 3 /* arg1 is actually arg2 for the call */
        li      3, -1 /* wait for anything */
        li      5, 1 /* WNOHNANG */
        li      6, 0 /* we don't care about the rusage field */

        sc

        cmpwi   3, 10 /* check for ECHILD */
        beq     a_wait_all_end_no_child_processes

        lwz     4, 0(16)

        rotrwi  4, 4, 8 /* use the high-order bits of the lower 16 bits only */
        andi.   4, 4, 0x7f

        stw     4, 0(16)

        blr

a_wait_all_end_no_child_processes:
        li      3, 0
        blr

a_set_sid:
        li      0, 66 /* sys_execve */
        sc
        blr

.section .note.GNU-stack,"",%progbits
