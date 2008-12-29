/*
 *  linux-ppc-gnu/io-system.S
 *  libcurie
 *
 *  Created by Magnus Deininger on 16/08/2008.
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

.text
        .align 8

.globl  a_open_directory
.globl  a_getdents64

.type a_open_directory,          @function
.type a_getdents64,                @function

/* C-functions: */
/* r3+ ? */
/* kernel: */
/* r3 r4 r5 r6 r7 ... ? */

a_open_directory:
    li      0, 5 /* sys_open */
    li      4, 0x4800 /* O_RDONLY | O_NONBLOCK | O_DIRECTORY */
    b       syscall_with_cleanup

a_getdents64:
    li      0, 202 /* sys_getdents64 */

syscall_with_cleanup:
    sc
    cmpwi   3, 0
    blt     negative_result
    blr
negative_result:
    cmpwi   3, -11 /* EAGAIN, as well as EWOULDBLOCK */
    beq     recoverable
    cmpwi   3, -4 /* EINTR */
    beq     recoverable
    li      4, 0
    lis     3, last_error_recoverable_p@ha
    stb     4, last_error_recoverable_p@l(3)
    li      3, -1
    blr
recoverable:
    li      4, 1
    lis     3, last_error_recoverable_p@ha
    stb     4, last_error_recoverable_p@l(3)
    li      3, -1
    blr

.section .note.GNU-stack,"",%progbits
