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

#include <curie/main.h>
#include <curie/exec.h>
#include <curie/sexpr.h>
#include <curie/multiplex.h>
#include <curie/memory.h>
#include <sievert/shell.h>
#include <icemake/icemake.h>

#define WIDTH 78
static int items_total  = 1;
static int items_have   = 0;
static int items_failed = 0;
static sexpr phase      = sx_false;
static struct io *out;

static void update_screen ()
{
    char buffer[0x1000] = " * ";
    const char *s = sx_symbol (phase);
    int i = 3, p = 4, j, x;

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
    buffer[i+2]   = '\r';
    i += 3;

    io_write (out, buffer, i);
}

static void complete ()
{
    io_write (out, (char*)"\r                                              "
                          "                                 \r", 81);
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

    multiplex_io();
    multiplex_sexpr();

    icemake = which (str_icemake);

    if (!stringp (icemake)) return -2;

    out = io_open_stdout ();

    argv[0] = (char *)sx_string (icemake);

    context = execute (EXEC_CALL_PURGE, argv, curie_environment);

    if (context->pid <= 0) return -3;

    multiplex_add_io_no_callback (out);
    multiplex_add_sexpr (sx_open_io (context->in, context->out),
                         icemake_read, (void *)0);

    while (multiplex() == mx_ok);

    afree (argvsize, argv);

    complete();

    return 0;
}

