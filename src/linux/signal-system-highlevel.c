/*
 *  signal-system-highlevel.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 10/08/2008.
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

#include <atomic/signal.h>
#include <atomic/signal-system.h>
#include <atomic/int.h>

typedef unsigned long size_t;

#define _LINUX_TYPES_H
#define _LINUX_TIME_H

#include <asm/signal.h>

typedef void (*signal_handler)(enum signal);
static signal_handler signal_handlers[(SIGNAL_MAX_NUM+1)];

int __a_set_signal_handler (int, void *);
int __a_send_signal (int, int);
int __a_sigreturn ();

static enum signal signum2signal (int signum) {
    switch (signum) {
        case SIGHUP:
            return sig_hup;
        case SIGINT:
            return sig_int;
        case SIGQUIT:
            return sig_quit;
        case SIGILL:
            return sig_ill;
        case SIGABRT:
            return sig_abrt;
        case SIGFPE:
            return sig_fpe;
        case SIGKILL:
            return sig_kill;
        case SIGSEGV:
            return sig_segv;
        case SIGPIPE:
            return sig_pipe;
        case SIGALRM:
            return sig_alrm;
        case SIGTERM:
            return sig_term;
        case SIGCHLD:
            return sig_chld;
        case SIGCONT:
            return sig_cont;
        case SIGSTOP:
            return sig_stop;
        case SIGTSTP:
            return sig_tstp;
        case SIGTTIN:
            return sig_ttin;
        case SIGTTOU:
            return sig_ttou;
        case SIGUSR1:
            return sig_usr1;
        case SIGUSR2:
            return sig_usr2;

        case SIGBUS:
            return sig_bus;
        case SIGPOLL:
            return sig_poll;
        case SIGPROF:
            return sig_prof;
        case SIGSYS:
            return sig_sys;
        case SIGTRAP:
            return sig_trap;
        case SIGURG:
            return sig_urg;
        case SIGVTALRM:
            return sig_vtalrm;
        case SIGXCPU:
            return sig_xcpu;
        case SIGXFSZ:
            return sig_xfsz;

#if SIGIOT != SIGABRT
        case SIGIOT:
            return sig_iot;
#endif
#if defined(SIGEMT)
        case SIGEMT:
            return sig_emt;
#endif
        case SIGSTKFLT:
            return sig_stkflt;
#if SIGIO != SIGPOLL
        case SIGIO:
            return sig_io;
#endif
        case SIGPWR:
            return sig_pwr;
#if defined(SIGINFO)
        case SIGINFO:
            return sig_info;
#endif
#if defined(SIGLOST)
        case SIGLOST:
            return sig_lost;
#endif
        case SIGWINCH:
            return sig_winch;

        default:
            return sig_unused;
    }
}

static int signal2signum (enum signal signal) {
    switch (signal) {
        case sig_hup:
            return SIGHUP;
        case sig_int:
            return SIGINT;
        case sig_quit:
            return SIGQUIT;
        case sig_ill:
            return SIGILL;
        case sig_abrt:
            return SIGABRT;
        case sig_fpe:
            return SIGFPE;
        case sig_kill:
            return SIGKILL;
        case sig_segv:
            return SIGSEGV;
        case sig_pipe:
            return SIGPIPE;
        case sig_alrm:
            return SIGALRM;
        case sig_term:
            return SIGTERM;
        case sig_chld:
            return SIGCHLD;
        case sig_cont:
            return SIGCONT;
        case sig_stop:
            return SIGSTOP;
        case sig_tstp:
            return SIGTSTP;
        case sig_ttin:
            return SIGTTIN;
        case sig_ttou:
            return SIGTTOU;
        case sig_usr1:
            return SIGUSR1;
        case sig_usr2:
            return SIGUSR2;

        case sig_bus:
            return SIGBUS;
        case sig_poll:
            return SIGPOLL;
        case sig_prof:
            return SIGPROF;
        case sig_sys:
            return SIGSYS;
        case sig_trap:
            return SIGTRAP;
        case sig_urg:
            return SIGURG;
        case sig_vtalrm:
            return SIGVTALRM;
        case sig_xcpu:
            return SIGXCPU;
        case sig_xfsz:
            return SIGXFSZ;

        case sig_iot:
            return SIGIOT;
#if defined(SIGEMT)
        case sig_emt:
            return SIGEMT;
#endif
        case sig_stkflt:
            return SIGSTKFLT;
        case sig_io:
            return SIGIO;
        case sig_pwr:
            return SIGPWR;
#if defined(SIGINFO)
        case sig_info:
            return SIGINFO;
#endif
#if defined(SIGLOST)
        case sig_lost:
            return SIGLOST;
#endif
        case sig_winch:
            return SIGWINCH;

        default:
            return sig_unused;
    }
}

static void sig_invoker (int signum) {
    enum signal signal = signum2signal(signum);

    if (signal != sig_unused) {
        signal_handler sighandler = signal_handlers[signal];

        sighandler(signal);
    }
}

void a_set_signal_handler (enum signal signal, void (*handler)(enum signal signal)) {
    struct sigaction action;
    int signum = signal2signum (signal);
    if (signum == sig_unused) return;

    action.sa_handler = sig_invoker;
    action.sa_mask = 0;
    action.sa_restorer = __a_sigreturn;
    action.sa_flags = SA_RESTORER;

    signal_handlers[signal] = handler;

    __a_set_signal_handler (signum, (void *)&action);
}

void a_kill (enum signal signal, int pid) {
    (void)__a_send_signal (pid, signal2signum(signal));
}
