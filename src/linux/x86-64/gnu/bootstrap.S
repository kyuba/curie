/*
 *  linux-x86-64-gnu/bootstrap.S
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

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

