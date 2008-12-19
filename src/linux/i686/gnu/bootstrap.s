/*
 *  linux-i686-gnu/bootstrap.S
 *  libcurie
 *
 *  Created by Joshua Keyes on 18/12/2008.
 *  Based on Magnus Deininger's x86-64 bootstrap code.
 *  Copyright 2008 Joshua Keyes. All rights reserved.
 *
 *  Doesn't support argv||envp due to x86 assembly failure.
 */

/*
 * Copyright (c) 2008, joshua Keyes All rights reserved.
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
        .size curie_argv, 4

curie_argv:
        .long 0x0

.globl curie_environment
        .data
        .type curie_environment, @object
        .size curie_environment, 4

curie_environment:
        .long 0x0

.text
        .align 8

.globl cmain
        .type cmain,              @function
.globl _start
        .type _start,             @function
.globl cexit
        .type cexit,              @function

_start:
        pushl	%ebp
        movl	%esp, %ebp

        call    cmain

        popl    %ebp
        movl    %eax, %ebx

cexit:
        movl    $1, %eax /* sys_exit */
        int     $0x80
        ret

.section .note.GNU-stack,"",%progbits
