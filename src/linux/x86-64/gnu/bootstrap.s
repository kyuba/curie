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

.globl curie_argv
        .data
        .type curie_argv, @object
        .size curie_argv, 8

curie_argv:
        .quad 0x0

.globl curie_environment
        .data
        .type curie_environment, @object
        .size curie_environment, 8

curie_environment:
        .quad 0x0

.text
        .align 8

.globl cmain
        .type cmain,              @function
.globl _start
        .type _start,             @function
.globl cexit
        .type cexit,              @function


/* C-functions: */
/* rdi rsi rdx rcx r8 r9 */
/* kernel: */
/* rdi rsi rdx r10 r8 r9 */

_start:
/* play dat funkeh music white boy */
        xor     %rbp, %rbp;
        and     $~15, %rsp;

/* parse argv */
        popq    %rbx
        inc     %rbx
        movq    $8, %rax
        mulq    %rbx

        movq    %rax, %rdi

        call    aalloc

        movq    %rax, curie_argv(%rip)

redo_argv:
        popq    %rcx
        movq    %rcx, (%rax)
        cmpq    $0x0, %rcx
        jz      argv_done
        addq    $8, %rax

        jmp     redo_argv

argv_done:

/* parse envp */
        movq    $0x1000, %rdi

        call    get_mem

        movq    %rax, curie_environment(%rip)
        movq    $0x1fe, %r11

redo_environment:
        popq    %rcx
        movq    %rcx, (%rax)
        cmpq    $0x0, %rcx
        jz      environment_done

        addq    $8, %rax

        dec     %r11
        jz      environment_done_prematurely

        jmp     redo_environment

environment_done_prematurely:
        movq    $0, (%rax)
environment_done:

        call cmain
        movq %rax, %rdi
cexit:
        movq $60, %rax /* sys_exit */
        syscall

.section .note.GNU-stack,"",%progbits
