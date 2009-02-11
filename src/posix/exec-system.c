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

#include <curie/exec-system.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <unistd.h>

int a_fork() {
    /*@-checkstrictglobs@*/
    return (int)fork();
    /*@=checkstrictglobs@*/
}

enum wait_return a_wait(int pid, int *status) {
    int st = 0;
    enum wait_return r;

    /*@-checkstrictglobs@*/
    (void)waitpid((pid_t)pid, &st, WNOHANG);
    /*@=checkstrictglobs@*/

    if (WIFEXITED(st)) {
        r = wr_exited;
        *status = (int)(char)(WEXITSTATUS(st));
    }
    else if (WIFSIGNALED(st))
    {
        r = wr_killed;
        *status = (int)(char)-(WTERMSIG(st));
    }
    else
    {
        r = wr_running;
        *status = (int)(char)0;
    }

    return r;
}

int a_wait_all(int *status) {
    int st, r;

    /*@-checkstrictglobs@*/
    r = waitpid((pid_t)-1, &st, WNOHANG);
    /*@=checkstrictglobs@*/

    if (WIFEXITED(st) != 0) {
        *status = WEXITSTATUS(st);
    }

    return r;
}

void a_exec(const char *image, char **argv, char **env) {
    /*@-checkstrictglobs@*/
    (void)execve(image, argv, env);
    /*@=checkstrictglobs@*/
}

int a_set_sid() {
    return setsid();
}
