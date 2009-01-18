/*
 *  multiplex-all-processes.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 23/10/2008.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
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

#include "curie/multiplex.h"
#include "curie/exec.h"
#include "curie/main.h"
#include "curie/int.h"

#define NUM_SUBPROCESSES 20

static int n_sp = 0;
static int rtab[NUM_SUBPROCESSES];

static void mx_on_death(struct exec_context *cx, void *d) {
    int *i = (int *)d;

    if (cx->exitstatus == *i)
    {
        *i = -1;
        n_sp++;
    }
    else
    {
        cexit (2);
    }
}

int cmain(void) {
    multiplex_all_processes();

    for (int i = 0; i < NUM_SUBPROCESSES; i++)
    {
        struct exec_context *context
                = execute(EXEC_CALL_NO_IO, (char **)0, (char **)0);

        if (context->pid < 0) {
            return 3;
        } else if (context->pid == 0) {
            return i + 4;
        }

        rtab[i] = i + 4;

        multiplex_add_process(context, mx_on_death, (void *)(rtab + i));
    }

    while (1)
    {
        multiplex();

        if (n_sp == NUM_SUBPROCESSES)
            cexit (0);
    }

    return (n_sp == NUM_SUBPROCESSES) ? 0 : 1;
}
