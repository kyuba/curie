/*
 *  linux-x86-64-gnu/memory.S
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 14/06/2008.
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

#include <asm/unistd.h>
#define _PAGESIZE         0x1000
#define _PAGESIZE_INVERSE 0x0fff

.globl get_mem
.globl free_mem
.globl resize_mem
.globl mark_mem_ro
.globl mark_mem_rw
.globl get_mem_chunk
.globl mem_chunk_size

.type get_mem,                 @function
.type free_mem,                @function
.type resize_mem,              @function
.type mark_mem_ro,             @function
.type mark_mem_rw,             @function
.type get_mem_chunk,           @function

.type mem_chunk_size,          @object
.size mem_chunk_size, 8

mem_chunk_size:
        .quad _PAGESIZE

.text
        .align 8

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

cmopsass:
/* the return value is stored in %rsi, since we always need it as the second
   argument. after that, do the actual syscall that was prepared in advance.
   input length is expected to be in %rsi as well. */
        movq %rsi, %r11
        and $_PAGESIZE_INVERSE, %rsi
        cmp %r11, %rsi
        je do_syscall
        add $_PAGESIZE, %rsi
do_syscall:
        syscall
        cmp $0, %rax
        js negative_result
        leave
        retq
negative_result:
        movq $0, %rax
        leave
        retq

get_mem_chunk:
        pushq   %rbp
        movq    %rsp, %rbp
        movq $_PAGESIZE, %rdi
        jmp get_mem_innards
get_mem:
        pushq   %rbp
        movq    %rsp, %rbp
get_mem_innards:
        movq $__NR_mmap, %rax
        movq %rdi, %rsi
        movq $0, %rdi /* pass 0-pointer as start address */
        movq $0x3, %rdx /* PROT_READ | PROT_WRITE */
        movq $0x22, %r10 /* MAP_ANON | MAP_PRIVATE */
        movq $-1, %r8 /* fd is -1 */
        movq $0, %r9 /* offset should be irrelevant */
        jmp cmopsass

free_mem:
        pushq   %rbp
        movq    %rsp, %rbp

        movq $__NR_munmap, %rax
        xor %rsi, %rdi
        xor %rdi, %rsi
        xor %rsi, %rdi

        jmp cmopsass

resize_mem:
        pushq   %rbp
        movq    %rsp, %rbp

        movq $__NR_mremap, %rax
        xor %rsi, %rdi
        xor %rdi, %rsi
        xor %rsi, %rdi

        movq $0x1, %r10

        movq %rdx, %r11
        and $_PAGESIZE_INVERSE, %rdx
        cmp %r11, %rdx
        je cmopsass
        add $_PAGESIZE, %rdx
        jmp cmopsass

mark_mem_ro:
        pushq   %rbp
        movq    %rsp, %rbp

        movq $__NR_mprotect, %rax
        xor %rsi, %rdi
        xor %rdi, %rsi
        xor %rsi, %rdi

        movq $0x1, %rdx
        jmp cmopsass

mark_mem_rw:
        pushq   %rbp
        movq    %rsp, %rbp

        movq $__NR_mprotect, %rax
        xor %rsi, %rdi
        xor %rdi, %rsi
        xor %rsi, %rdi

        movq $0x3, %rdx
        jmp cmopsass

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

