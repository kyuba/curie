/*
 *  linux-ppc-gnu/memory.S
 *  libcurie
 *
 *  Created by Magnus Deininger on 17/08/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

.data

#define _PAGESIZE         0x1000
#define _PAGESIZE_INVERSE ~0x0fff

.globl get_mem
.globl free_mem
.globl resize_mem
.globl mark_mem_ro
.globl mark_mem_rw
.globl mark_mem_rx
.globl get_mem_chunk
.globl set_get_mem_recovery_function
.globl set_resize_mem_recovery_function

.type get_mem,                 @function
.type free_mem,                @function
.type resize_mem,              @function
.type mark_mem_ro,             @function
.type mark_mem_rw,             @function
.type mark_mem_rx,             @function
.type get_mem_chunk,           @function
.type set_get_mem_recovery_function, @function
.type set_resize_mem_recovery_function, @function

.data
get_mem_recovery:
        .long 0x0

resize_mem_recovery:
        .long 0x0

.text
        .align 8

set_get_mem_recovery_function:
        lis     5, get_mem_recovery@ha
        addi    5, 5, get_mem_recovery@l
        stw     3, 0(5)
        blr

set_resize_mem_recovery_function:
        lis     5, resize_mem_recovery@ha
        addi    5, 5, resize_mem_recovery@l
        stw     3, 0(5)
        blr

cmopsass:
        mr      9, 4

        lis     10, _PAGESIZE_INVERSE@ha
        addi    10, 10, _PAGESIZE_INVERSE@l

        and     4, 4, 10
        cmpw    9, 4
        beq     do_syscall
        addi    4, 4, _PAGESIZE 
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
        li      4, _PAGESIZE
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

        lis     10, _PAGESIZE_INVERSE@ha
        addi    10, 10, _PAGESIZE_INVERSE@l

        and     5, 5, 10
        cmpw    9, 5
        beq     cmopsass
        addi    5, 5, _PAGESIZE

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

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif
