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

/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

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

        movq (%r11), %r11
        call *%r11

        leave
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
        pushq   get_mem_recovery@GOTPCREL(%rip)

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
        pushq   resize_mem_recovery@GOTPCREL(%rip)

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
