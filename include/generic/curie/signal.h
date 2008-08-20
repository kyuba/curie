/*
 *  signal.h
 *  curie-libc
 *
 *  Created by Magnus Deininger on 08/08/2008.
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

#ifndef ATOMIC_SIGNAL_H
#define ATOMIC_SIGNAL_H

#include <curie/multiplex.h>

#define SIGNAL_MAX_NUM sig_winch

enum signal {
    sig_unused = 0,

    /*  posix.1-1990 */
    sig_hup = 1,     /* terminal hangup/controlling process died */
    sig_int = 2,     /* interrupt */
    sig_quit = 3,    /* quit */
    sig_ill = 4,     /* illegal instruction */
    sig_abrt = 5,    /* abort */
    sig_fpe = 6,     /* floating point exception */
    sig_kill = 7,    /* kill */
    sig_segv = 8,    /* segmentation violation */
    sig_pipe = 9,    /* broken pipe */
    sig_alrm = 10,   /* timer */
    sig_term = 11,   /* termination request */
    sig_chld = 12,   /* child stopped/terminated */
    sig_cont = 13,   /* continue */
    sig_stop = 14,   /* stop */
    sig_tstp = 15,   /* stop from tty */
    sig_ttin = 16,   /* tty input (background process) */
    sig_ttou = 17,   /* tty output (background process) */

    sig_usr1 = 18,   /* user-defined signal */
    sig_usr2 = 19,   /* user-defined signal */

    /*  posix.1-2001 */
    sig_bus = 20,    /* bus error */
    sig_poll = 21,   /* pollable event */
    sig_prof = 22,   /* profiling timer expired */
    sig_sys = 23,    /* bad argument to routine */
    sig_trap = 24,   /* trace/breakpoint trap */
    sig_urg = 25,    /* urgent condition on socket */
    sig_vtalrm = 26, /* virtual alarm clock */
    sig_xcpu = 27,   /* cpu time limit exceeded */
    sig_xfsz = 28,   /* file size limit exceeded */

    /* random (i've seen these on linux) */
    sig_iot = 29,    /* IOT trap */
    sig_emt = 30,    /* ? */
    sig_stkflt = 31, /* stack fault on coprocessor */
    sig_io = 32,     /* I/O possible */
    sig_pwr = 33,    /* power failure */
    sig_info = 34,   /* power status information ? */
    sig_lost = 35,   /* file lock lost */
    sig_winch = 36   /* window resize */
};

enum signal_callback_result {
    scr_keep = 0,
    scr_ditch = 1
};

void multiplex_signal ();
void multiplex_add_signal (enum signal signal, enum signal_callback_result (*handler)(enum signal, void *), void *data);

void send_signal (enum signal signal, int pid);
void send_signal_self (enum signal signal);

#endif
