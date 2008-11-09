/*
 *  exec-system.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 06/08/2008.
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
