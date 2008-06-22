/*
 *  darwin-powermacintosh-gnu/atom.S
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

.globl _last_error_recoverable_p
.globl _atomic_argv
.globl _atomic_environment

_last_error_recoverable_p:
        .byte 1

_atomic_argv:
        .long 0x0

_atomic_environment:
        .long 0x0

.text
        .align 8

.globl	__atomic_exit
.globl  __atomic_read
.globl  __atomic_write
.globl  __atomic_open_read
.globl  __atomic_open_write
.globl  __atomic_create
.globl  __atomic_close
.globl  __atomic_kill

/* C-functions: */
/* r3+ ? */
/* kernel: */
/* r3 r4 r5 r6 r7 ... */

__atomic_exit:
    li      r0, 1 /* sys_exit */
    b       syscall_with_cleanup

__atomic_read:
    li      r0, 3 /* sys_read */
    b       syscall_with_cleanup

__atomic_write:
    li      r0, 4 /* sys_write */
    b       syscall_with_cleanup

__atomic_open_read:
    li      r0, 5 /* sys_open */
    li      r4, 0x4 /* O_RDONLY | O_NONBLOCK */
    b       syscall_with_cleanup

__atomic_open_write:
    li      r0, 5 /* sys_open */
    li      r4, 0x205 /* O_WRONLY | O_NONBLOCK | O_CREAT */
    li      r5, 0x1b6
    b       syscall_with_cleanup

__atomic_close:
    li      r0, 6 /* sys_close */
    b       syscall_with_cleanup

__atomic_create:
    li      r0, 5 /* sys_open */
    mr      r5, r4
    li      r4, 0x205 /* O_WRONLY | O_NONBLOCK | O_CREAT */
    b       syscall_with_cleanup

__atomic_kill:
    li      r0, 37 /* sys_kill */
    b       syscall_with_cleanup

syscall_with_cleanup:
    sc
    b       negative_result /* system call failed */
    blr                     /* system call succeeded */
negative_result:
    cmpwi   r3, 35 /* EAGAIN, as well as EWOULDBLOCK */
	beq     recoverable
    cmpwi   r3, 4 /* EINTR */
	beq     recoverable
	li      r4, 0
    lis     r3, ha16(_last_error_recoverable_p)
    addi    r3, r3,lo16(_last_error_recoverable_p)
	stb     r4, 0(r3)
	li      r3, -1
    blr
recoverable:
	li      r4, 1
    lis     r3, ha16(_last_error_recoverable_p)
    addi    r3, r3,lo16(_last_error_recoverable_p)
	stb     r4, 0(r3)
	li      r3, -1
    blr

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

