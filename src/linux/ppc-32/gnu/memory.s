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

.data

.globl get_mem
.globl free_mem
.globl resize_mem
.globl mark_mem_ro
.globl mark_mem_rw
.globl mark_mem_rx
.globl get_mem_chunk

.type get_mem,                 @function
.type free_mem,                @function
.type resize_mem,              @function
.type mark_mem_ro,             @function
.type mark_mem_rw,             @function
.type mark_mem_rx,             @function
.type get_mem_chunk,           @function

.text
        .align 8

cmopsass:
        mr      9, 4

        lis     10, ~0x0fff@ha
        addi    10, 10, ~0x0fff@l

        and     4, 4, 10
        cmpw    9, 4
        beq     do_syscall
        addi    4, 4, 0x1000 
do_syscall:
        sc

        cmpwi   3, 50
        ble     recover

epil:
        lwz     15,12(1)
        lwz     16,16(1)
        lwz     17,20(1)
        lwz     18,24(1)

        addi    1,1,32
        blr

recover:
        li      3, 0

        cmpwi   18,0
        beq     epil

        mr      3, 15
        mr      4, 16
        mr      5, 17
        mtctr   18

        lwz     15,12(1)
        lwz     16,16(1)
        lwz     17,20(1)
        lwz     18,24(1)
        addi    1,1,32

        bctr

get_mem_chunk:
        li      4, 0x1000
        b       get_mem_innards
get_mem:
        mr      4, 3
get_mem_innards:
        addi    1,1,-32

        stw     15,12(1)
        stw     16,16(1)
        stw     17,20(1)
        stw     18,24(1)

        mr      15, 3
        lis     18, get_mem_recovery@ha
        addi    18, 18, get_mem_recovery@l
        lwz     18, 0(18)

        li      0, 90 /* sys_mmap */
        li      3, 0 /* pass 0-pointer as start address */
        li      5, 3 /* PROT_READ | PROT_WRITE */
        li      6, 0x22 /* MAP_ANON | MAP_PRIVATE */
        li      7, -1 /* fd is -1 */
        li      8, 0 /* offset should be irrelevant */
        b       cmopsass

resize_mem:
        addi    1,1,-32

        stw     15,12(1)
        stw     16,16(1)
        stw     17,20(1)
        stw     18,24(1)

        mr      15, 3
        mr      16, 4
        mr      17, 5
        lis     18, resize_mem_recovery@ha
        addi    18, 18, resize_mem_recovery@l
        lwz     18, 0(18)

        li      0, 163 /* sys_mremap */

        mr      9, 4
        mr      4, 3
        mr      3, 9

        li      6, 1 /* MREMAP_MAYMOVE */

        mr      9, 5

        lis     10, ~0x0fff@ha
        addi    10, 10, ~0x0fff@l

        and     5, 5, 10
        cmpw    9, 5
        beq     cmopsass
        addi    5, 5, 0x1000

        b       cmopsass

free_mem:
        li      0, 91 /* sys_munmap */

        b       swap_first_and_second_arg_sc

mark_mem_rx:
        li      5, 0x5
        b       mark_mem
mark_mem_rw:
        li      5, 0x3
        b       mark_mem
mark_mem_ro:
        li      5, 0x1
mark_mem:
        li      0, 125 /* sys_mprotect */

swap_first_and_second_arg_sc:
        addi    1,1,-32

        stw     15,12(1)
        stw     16,16(1)
        stw     17,20(1)
        stw     18,24(1)

        mr      9, 4
        mr      4, 3
        mr      3, 9

        li      18, 0

        b       cmopsass

.section .note.GNU-stack,"",%progbits
