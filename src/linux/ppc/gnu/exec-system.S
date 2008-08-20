/*
 *  linux-ppc-gnu/exec-system.S
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

.text
        .align 8

.globl  a_fork
.globl  a_exec
.globl  a_wait

.type a_fork,                    @function
.type a_exec,                    @function
.type a_wait,                    @function

a_fork:
        li      0, 2 /* sys_fork */
        sc
        blr

a_exec:
        li      0, 11 /* sys_execve */
        sc
        blr

a_wait:
        li      0, 114 /* sys_wait4 */
        mr      15, 3
        mr      16, 4
        li      5, 1 /* WNOHNANG */
        li      6, 0 /* we don't care about the rusage field */

        sc

        cmpw    3, 15 /* see if the return value is the pid we passed */
        beq     wait_examine

        li      3, 0 /* wr_running */
        blr

wait_examine:
        l       3, 0(16)
        andi.   3, 3, 0x7f
        beq     exited_normally /* normal exit if that mask is 0 */

        li      3, 2 /* wr_killed */
        blr

exited_normally:
        l       3, 0(16)
        li      4, 0

        rotrwi  3, 3, 8 /* use the high-order bits of the lower 16 bits only */
        andi.   3, 3, 0x7f

        stw     3, 0(16)

        li      3, 1 /* wr_exited */
        blr


#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

