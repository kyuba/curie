/*
 *  linux-ppc-gnu/bootstrap.S
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

.globl curie_argv
.globl curie_environment

curie_argv:
        .long 0x0

curie_environment:
        .long 0x0

.globl _start
.globl cexit

.type _start,                    @function
.type cexit,                     @function

.text

_start:
        cmpwi   3, 0
        bne     _3

        mr      3, 1

        li      4, 0

_1:
        addi    1, 1, 4
        addi    4, 4, 1
        l       9, 0(1)
        cmpwi   9, 0
        bne     _1

        li      5, 0

_2:
        addi    1, 1, 4
        addi    5, 5, 1
        l       9, 0(1)
        cmpwi   9, 0
        bne     _2

        mr      1, 3
        addi    1, 1, -16

        bl      __do_startup
        mr      1, 3
        b       _4

_3:
        lis     16, curie_argv@ha
        stw     4, curie_argv@l(16)
        lis     16, curie_environment@ha
        stw     5, curie_environment@l(16)

_4:
        addi    1, 1, -16
        bl      cmain

cexit:
        li      0, 1
        sc

.section .note.GNU-stack,"",%progbits
