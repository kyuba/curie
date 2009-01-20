/*
 *  icemake-run-tests.c
 *  libcurie/icemake
 *
 *  Split from icemake.c by Magnus Deininger on 20/01/2009.
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

#include <curie/sexpr.h>
#include <curie/tree.h>
#include <curie/multiplex.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <icemake/icemake.h>

static void run_tests_library_gcc (sexpr name, struct target *t)
{
    sexpr s = t->test_cases;

    while (consp (s))
    {
        sexpr r = cdr(cdr(cdr(car(s))));

        workstack = cons (r, workstack);

        s = cdr (s);
    }
}

static void run_tests_library (sexpr name, struct target *t)
{
    sexpr cur = t->test_reference;

    while (consp (cur))
    {
        sexpr r = car (car (cur));

        unlink (sx_string (r));

        cur = cdr (cur);
    }

    switch (uname_toolchain)
    {
        case tc_gcc:
            run_tests_library_gcc (name, t); break;
    }
}

static void diff_test_reference_library_gcc (sexpr name, struct target *t)
{
    sexpr cur = t->test_reference;

    while (consp (cur))
    {
        sexpr r = car (cur);
        sexpr o = car (r), g = cdr (r);

        workstack = cons (cons (p_diff, cons (o, cons (g, sx_end_of_list))), workstack);

        cur = cdr (cur);
    }
}

static void diff_test_reference_library (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            diff_test_reference_library_gcc (name, t); break;
    }
}

static void do_run_tests_target(struct target *t)
{
    if (truep(t->library))
    {
        diff_test_reference_library (t->name, t);
        run_tests_library (t->name, t);
    }
}

static void target_map_run_tests (struct tree_node *node, void *u)
{
    do_run_tests_target(node_get_value(node));
}

static void run_tests_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_run_tests_target (node_get_value(node));
    }
}

void run_tests (sexpr buildtargets)
{
    sexpr cursor = buildtargets;
    if (eolp(cursor))
    {
        tree_map (&targets, target_map_run_tests, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        run_tests_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}
