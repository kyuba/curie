/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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
