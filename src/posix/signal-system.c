/*
 *  signal-system.c
 *  libcurie
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

#define _POSIX_SOURCE

#include <curie/signal.h>
#include <curie/signal-system.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

typedef void (*signal_handler)(enum signal);
static signal_handler signal_handlers[20]; /* 20 is sig_usr2 + 1 */

static void invoker (int signum) {
    enum signal signal = sig_unused;
    signal_handler sighandler;

    switch (signum) {
        case SIGHUP:
            signal = sig_hup;
            break;
        case SIGINT:
            signal = sig_int;
            break;
        case SIGQUIT:
            signal = sig_quit;
            break;
        case SIGILL:
            signal = sig_ill;
            break;
        case SIGABRT:
            signal = sig_abrt;
            break;
        case SIGFPE:
            signal = sig_fpe;
            break;
        case SIGKILL:
            signal = sig_kill;
            break;
        case SIGSEGV:
            signal = sig_segv;
            break;
        case SIGPIPE:
            signal = sig_pipe;
            break;
        case SIGALRM:
            signal = sig_alrm;
            break;
        case SIGTERM:
            signal = sig_term;
            break;
        case SIGCHLD:
            signal = sig_chld;
            break;
        case SIGCONT:
            signal = sig_cont;
            break;
        case SIGSTOP:
            signal = sig_stop;
            break;
        case SIGTSTP:
            signal = sig_tstp;
            break;
        case SIGTTIN:
            signal = sig_ttin;
            break;
        case SIGTTOU:
            signal = sig_ttou;
            break;
        case SIGUSR1:
            signal = sig_usr1;
            break;
        case SIGUSR2:
            signal = sig_usr2;
            break;
        default:
            /* since this is the posix variant, not all signals are supported;
               requests to handle these signals will be ignored. */
            return;
    }

    sighandler = signal_handlers[signal];
    sighandler(signal);
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
        default:
            /* since this is the posix variant, not all signals are supported;
            requests to handle these signals will be ignored. */
            return sig_unused;
    }
}

void a_set_signal_handler (enum signal signal, void (*handler)(enum signal signal)) {
    struct sigaction action;
    int signum = signal2signum (signal);
    if (signum == sig_unused) return;

    signal_handlers[signal] = handler;

    sigemptyset(&(action.sa_mask));

    action.sa_handler = invoker;
    action.sa_flags = 0;

    sigaction (signum, &action, (struct sigaction *)0);
}

void a_kill (enum signal signal, int pid) {
    int signum = signal2signum (signal);
    if (signum == sig_unused) return;
    kill (pid, signum);
}

int a_getpid () {
    return (int)getpid();
}
