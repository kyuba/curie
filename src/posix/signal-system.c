/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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

typedef void (*signal_handler)(enum signal);
static signal_handler signal_handlers[20]; /* 20 is sig_usr2 + 1 */

static void invoker (int signum) {
    enum signal signal = sig_unused;
    signal_handler sighandler;

    switch (signum) {
#if defined(SIGHUP)
        case SIGHUP:
            signal = sig_hup;
            break;
#endif
        case SIGINT:
            signal = sig_int;
            break;
#if defined(SIGQUIT)
        case SIGQUIT:
            signal = sig_quit;
            break;
#endif
        case SIGILL:
            signal = sig_ill;
            break;
        case SIGABRT:
            signal = sig_abrt;
            break;
        case SIGFPE:
            signal = sig_fpe;
            break;
#if defined(SIGKILL)
        case SIGKILL:
            signal = sig_kill;
            break;
#endif
        case SIGSEGV:
            signal = sig_segv;
            break;
#if defined(SIGPIPE)
        case SIGPIPE:
            signal = sig_pipe;
            break;
#endif
#if defined(SIGALRM)
        case SIGALRM:
            signal = sig_alrm;
            break;
#endif
        case SIGTERM:
            signal = sig_term;
            break;
#if defined(SIGCHLD)
        case SIGCHLD:
            signal = sig_chld;
            break;
#endif
#if defined(SIGCONT)
        case SIGCONT:
            signal = sig_cont;
            break;
#endif
#if defined(SIGSTOP)
        case SIGSTOP:
            signal = sig_stop;
            break;
#endif
#if defined(SIGTSTP)
        case SIGTSTP:
            signal = sig_tstp;
            break;
#endif
#if defined(SIGTTIN)
        case SIGTTIN:
            signal = sig_ttin;
            break;
#endif
#if defined(SIGTTOU)
        case SIGTTOU:
            signal = sig_ttou;
            break;
#endif
#if defined(SIGUSR1)
        case SIGUSR1:
            signal = sig_usr1;
            break;
#endif
#if defined(SIGUSR2)
        case SIGUSR2:
            signal = sig_usr2;
            break;
#endif
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
#if defined(SIGHUP)
        case sig_hup:
            return SIGHUP;
#endif
        case sig_int:
            return SIGINT;
#if defined(SIGQUIT)
        case sig_quit:
            return SIGQUIT;
#endif
        case sig_ill:
            return SIGILL;
        case sig_abrt:
            return SIGABRT;
        case sig_fpe:
            return SIGFPE;
#if defined(SIGKILL)
        case sig_kill:
            return SIGKILL;
#endif
        case sig_segv:
            return SIGSEGV;
#if defined(SIGPIPE)
        case sig_pipe:
            return SIGPIPE;
#endif
#if defined(SIGALRM)
        case sig_alrm:
            return SIGALRM;
#endif
        case sig_term:
            return SIGTERM;
#if defined(SIGCHLD)
        case sig_chld:
            return SIGCHLD;
#endif
#if defined(SIGCONT)
        case sig_cont:
            return SIGCONT;
#endif
#if defined(SIGSTOP)
        case sig_stop:
            return SIGSTOP;
#endif
#if defined(SIGTSTP)
        case sig_tstp:
            return SIGTSTP;
#endif
#if defined(SIGTTIN)
        case sig_ttin:
            return SIGTTIN;
#endif
#if defined(SIGTTOU)
        case sig_ttou:
            return SIGTTOU;
#endif
#if defined(SIGUSR1)
        case sig_usr1:
            return SIGUSR1;
#endif
#if defined(SIGUSR2)
        case sig_usr2:
            return SIGUSR2;
#endif
        default:
            /* since this is the posix variant, not all signals are supported;
               requests to handle these signals will be ignored. */
            return sig_unused;
    }
}

void a_set_signal_handler (enum signal sig, void (*handler)(enum signal)) {
#ifdef HAVE_SIGACTION
    struct sigaction action;
    int signum = signal2signum (sig);
    if (signum == sig_unused) return;

    signal_handlers[sig] = handler;

    (void)sigemptyset(&(action.sa_mask));

    action.sa_handler = invoker;
    action.sa_flags = 0;

    (void)sigaction (signum, &action, (struct sigaction *)0);
#else
    int signum = signal2signum (sig);
    if (signum == sig_unused) return;

    signal_handlers[sig] = handler;

    (void)signal (signum, invoker);
#endif
}
