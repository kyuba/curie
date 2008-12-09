/*
 *  linux-x86-64-gnu/memory.S
 *  libcurie
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
        .long 0x0

resize_mem_recovery:
        .long 0x0
        .long 0x0

.text
        .align 8

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

set_get_mem_recovery_function:
        movq %rdi, get_mem_recovery(%rip)
        ret

set_resize_mem_recovery_function:
        movq %rdi, resize_mem_recovery(%rip)
        ret

cmopsass:
/* the return value is stored in %rsi, since we always need it as the second
   argument. after that, do the actual syscall that was prepared in advance.
   input length is expected to be in %rsi as well. */
        movq %rsi, %r11
        andq $~0x0fff, %rsi
        cmpq %rsi, %r11
        jz do_syscall
        addq $0x1000, %rsi
do_syscall:
        syscall
        cmp $0, %rax
        js negative_result

        popq %r11
        popq %rdx
        popq %rsi
        popq %rdi

        leave
        ret
negative_result:
        movq $0, %rax

        popq %r11
        popq %rdx
        popq %rsi
        popq %rdi

        cmp $0, %r11
        jz rnr
        call *%r11

rnr:    leave
        ret

get_mem_chunk:
        movq $0x1000, %rsi
        jmp get_mem_innards
get_mem:
        movq %rdi, %rsi
get_mem_innards:
        pushq   %rbp
        movq    %rsp, %rbp

        pushq   %rdi
        pushq   $0
        pushq   $0
        pushq   get_mem_recovery(%rip)

        movq $9, %rax /* sys_mmap */
        movq $0, %rdi /* pass 0-pointer as start address */
        movq $0x3, %rdx /* PROT_READ | PROT_WRITE */
        movq $0x22, %r10 /* MAP_ANON | MAP_PRIVATE */
        movq $-1, %r8 /* fd is -1 */
        movq $0, %r9 /* offset should be irrelevant */
        jmp cmopsass

free_mem:
        pushq   %rbp
        movq    %rsp, %rbp

        movq $11, %rax /* sys_munmap */

        jmp     xor_first_and_second_arg_sc

resize_mem:
        pushq   %rbp
        movq    %rsp, %rbp

        pushq   %rdi
        pushq   %rsi
        pushq   %rdx
        pushq   resize_mem_recovery(%rip)

        movq $25, %rax /* sys_mremap */
        xor %rsi, %rdi
        xor %rdi, %rsi
        xor %rsi, %rdi

        movq $0x1, %r10
        movq $0x0, %r8

        movq %rdx, %r11
        andq $~0x0fff, %rdx
        cmpq %rdx, %r11
        jz cmopsass
        addq $0x1000, %rdx
        jmp cmopsass

mark_mem_rx:
        movq $0x5, %rdx
        jmp mark_mem
mark_mem_rw:
        movq $0x3, %rdx
        jmp mark_mem
mark_mem_ro:
        movq $0x1, %rdx
mark_mem:
        pushq   %rbp
        movq    %rsp, %rbp

        movq $10, %rax /* sys_mprotect */

xor_first_and_second_arg_sc:
        xor %rsi, %rdi
        xor %rdi, %rsi
        xor %rsi, %rdi

        pushq   $0
        pushq   $0
        pushq   $0
        pushq   $0

        jmp cmopsass

.section .note.GNU-stack,"",%progbits
