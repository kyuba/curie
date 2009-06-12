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

#include <curie/main.h>
#include <curie/exec.h>
#include <curie/sexpr.h>
#include <curie/multiplex.h>
#include <curie/shell.h>
#include <curie/memory.h>

#include <syscall/syscall.h>

#include <icemake/icemake.h>

#define WIDTH 78
static int items_total  = 1;
static int items_have   = 0;
static int exitstatus   = -1;
static int items_failed = 0;
static sexpr phase      = sx_false;

static void update_screen ()
{
    char buffer[0x1000] = "\r * ";
    const char *s = sx_symbol (phase);
    int i = 4, p = 4, j, x;

    for (j = 0; s[j] != (char)0; j++, i++, p++)
    {
        buffer[i] = s[j];
    }

    buffer[i]     = ' ';
    buffer[i+1]   = '[';
    buffer[i+2]   = ' ';
    i += 3;

    if (items_have > items_total) items_have = items_total;

    for (j = 0, x = ((double)items_have / (double)items_total)
                        * (WIDTH - p - 3);
         j < x; j++, i++, p++)
    {
        buffer[i] = '#';
    }

    for (j = 0, x = WIDTH - p - 3; j < x; j++, i++, p++)
    {
        buffer[i] = ' ';
    }

    buffer[i]     = ' ';
    buffer[i+1]   = ']';
    buffer[i+2]   = ' ';
    i += 3;

#if defined(have_sys_write)
    sys_write (1, buffer, i);
#endif
}

static void complete ()
{
#if defined(have_sys_write)
    sys_write (1, (char*)"\n", 1);
#endif
    cexit (exitstatus);
}

static void icemake_death(struct exec_context *context, void *aux)
{
    if (exitstatus == -1)
    {
        exitstatus = context->exitstatus;
    }
    else
    {
        exitstatus = context->exitstatus;

        complete();
    }
}

static void icemake_read (sexpr sx, struct sexpr_io *io, void *aux)
{
    if (consp (sx))
    {
        sexpr c = car (sx);

        if (truep(equalp (c, sym_items_total)))
        {
            sexpr r = car (cdr(sx));
            items_total = (int)sx_integer (r);
            if (items_total < 1) items_total = 1;
            items_have = 0;
        }
        else if (truep(equalp (c, sym_items_remaining)))
        {
            sexpr r = car (cdr(sx));
            int items_remaining = (int)sx_integer (r);
            if (items_remaining > items_total) items_total = items_remaining;
            items_have = (items_total - items_remaining);
            if (items_have < 0) items_have = 0;
        }
        else if (truep(equalp (c, sym_phase)))
        {
            sexpr r = car (cdr(sx));
            sx_xref (r);
            sx_destroy (phase);
            phase = r;
        }
        else if (truep(equalp (c, sym_completed)))
        {
            items_have++;
            if (items_have > items_total) items_have = items_total;
        }
        else if (truep(equalp (c, sym_failed)))
        {
            items_have++;
            items_failed++;
            if (items_have > items_total) items_have = items_total;
        }

        update_screen();
        sx_destroy (sx);
    } else if (eofp(sx)) {
        if (exitstatus != -1)
        {
            complete ();
        }
        else
        {
            exitstatus = -2;
        }
    }
}

int cmain()
{
    struct exec_context *context;
    int i = 0, argvsize;
    define_string (str_icemake, "icemake");
    sexpr icemake;
    char **argv;

    while (curie_argv[i] != (char *)0) { i++; }

    argvsize = sizeof (char *) * (i + 1);
    argv = aalloc (argvsize);

    for (i = 0; curie_argv[i] != (char *)0; i++)
    {
        argv[i] = curie_argv[i];
    }
    curie_argv[i] = (char *)0;

    terminate_on_allocation_errors();
    multiplex_sexpr();
    multiplex_process();

    icemake = which (str_icemake);

    if (!stringp (icemake)) return -2;

    argv[0] = (char *)sx_string (icemake);

    context = execute (EXEC_CALL_PURGE, argv, curie_environment);

    if (context == (struct exec_context *)0) return -1;
    if (context->pid <= 0) return -3;

    multiplex_add_process (context, icemake_death, (void *)0);
    multiplex_add_sexpr (sx_open_io (context->in, context->out),
                         icemake_read, (void *)0);

    while (1) multiplex();

    afree (argvsize, argv);

    return 0;
}
