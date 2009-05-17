/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

    (void)sigemptyset(&(action.sa_mask));

    action.sa_handler = invoker;
    action.sa_flags = 0;

    (void)sigaction (signum, &action, (struct sigaction *)0);
}

void a_kill (enum signal signal, int pid) {
    int signum = signal2signum (signal);
    if (signum == sig_unused) return;
    (void)kill ((pid_t)pid, signum);
}

int a_getpid () {
    return (int)getpid();
}
