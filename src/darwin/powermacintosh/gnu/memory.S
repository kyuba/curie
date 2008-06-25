/*
 *  darwin-powermacintosh-gnu/memory.S
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 22/06/2008.
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

.globl _mem_chunk_size

_mem_chunk_size:
        .long _PAGESIZE

.text
        .align 8

.globl _get_mem
.globl _free_mem
.globl _mark_mem_ro
.globl _mark_mem_rw
.globl _get_mem_chunk
.globl _mem_chunk_size

cmopsass:
        mr      r9, r4

        lis     r10, ha16(_PAGESIZE_INVERSE)
        addi    r10, r10, lo16(_PAGESIZE_INVERSE)

        and     r4, r4, r9
        cmpw    r9, r4
        beq     do_syscall
		addi    r4, r4, _PAGESIZE 
do_syscall:
        sc
        li      r3, 0
        blr

_get_mem_chunk:
		li      r4, _PAGESIZE
        b       get_mem_innards
_get_mem:
        mr      r4, r3
get_mem_innards:
        li      r0, 197 /* sys_mmap */
        li      r3, 0 /* pass 0-pointer as start address */
        li      r5, 3 /* PROT_READ | PROT_WRITE */
        li      r6, 0x1002 /* MAP_ANON | MAP_PRIVATE */
        li      r7, -1 /* fd is -1 */
        li      r8, 0 /* offset should be irrelevant */
        b       cmopsass

_free_mem:
        li      r0, 73 /* sys_munmap */

        b       swap_first_and_second_arg_sc

_mark_mem_rw:
        li      r5, 0x3
        b       mark_mem
_mark_mem_ro:
        li      r5, 0x1
mark_mem:
        li      r0, 74 /* sys_mprotect */

swap_first_and_second_arg_sc:
        mr      r9, r4
        mr      r4, r3
        mr      r3, r9

        b       cmopsass

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

