/*
 *  linux-ppc-gnu/signal-system.S
 *  curie-libc
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

.globl  __a_set_signal_handler
.globl  a_getpid
.globl  __a_send_signal
.globl  __a_sigreturn

.type __a_set_signal_handler,    @function
.type a_getpid,                  @function
.type __a_send_signal,           @function
.type __a_sigreturn,             @function

__a_set_signal_handler:
        li      0, 67 /* sys_sigaction */
        li      5, 0 /* don't care about the old handler */

        sc
        blr

__a_send_signal:
        li      0, 37 /* sys_kill */
        sc
        blr

a_getpid:
        li      0, 20 /* sys_getpid */
        sc
        blr

__a_sigreturn:
        li      0, 119 /* sys_sigreturn */
        sc
        blr

#if defined(__ELF__)
          .section .note.GNU-stack,"",%progbits
#endif

