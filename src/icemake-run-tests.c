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

#include <icemake/icemake.h>

#include <curie/multiplex.h>

static void run_tests_library_common (sexpr name, struct target *t)
{
    sexpr s = t->test_cases;

    while (consp (s))
    {
        sexpr r = cdr(cdr(cdr(car(s))));

        t->icemake->workstack = sx_set_add (t->icemake->workstack, r);

        s = cdr (s);
    }
}

static void run_tests_library_gcc     (sexpr name, struct target *t)
{
    run_tests_library_common (name, t);
}

static void run_tests_library_borland (sexpr name, struct target *t)
{
    run_tests_library_common (name, t);
}

static void run_tests_library_msvc    (sexpr name, struct target *t)
{
    run_tests_library_common (name, t);
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
            run_tests_library_gcc     (name, t); break;
        case tc_borland:
            run_tests_library_borland (name, t); break;
        case tc_msvc:
            run_tests_library_msvc    (name, t); break;
    }
}

static void diff_test_reference_library_common (sexpr name, struct target *t)
{
    if (falsep(p_diff))
    {
        return;
    }
    else
    {
        sexpr cur = t->test_reference;

        while (consp (cur))
        {
            sexpr r = car (cur);
            sexpr o = car (r), g = cdr (r);

            t->icemake->workstack = sx_set_add (t->icemake->workstack,
                              cons (p_diff, cons (o, cons (g, sx_end_of_list))));

            cur = cdr (cur);
        }
    }
}

static void diff_test_reference_library_gcc (sexpr name, struct target *t)
{
    diff_test_reference_library_common (name, t);
}

static void diff_test_reference_library_borland (sexpr name, struct target *t)
{
    diff_test_reference_library_common (name, t);
}

static void diff_test_reference_library_msvc (sexpr name, struct target *t)
{
    diff_test_reference_library_common (name, t);
}

static void diff_test_reference_library (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            diff_test_reference_library_gcc     (name, t); break;
        case tc_borland:
            diff_test_reference_library_borland (name, t); break;
        case tc_msvc:
            diff_test_reference_library_msvc    (name, t); break;
    }
}

static void do_run_tests_target(struct target *t)
{
    if (t->options & ICEMAKE_LIBRARY)
    {
        diff_test_reference_library (t->name, t);
        run_tests_library (t->name, t);
    }
}

int icemake_run_tests (struct icemake *im)
{
    sexpr cursor = im->buildtargets;
    sx_write (stdio, cons (sym_phase, cons (sym_run_tests, sx_end_of_list)));

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);
        struct tree_node *node =
            tree_get_node_string (&(im->targets), (char *)target);

        if (node != (struct tree_node *)0)
        {
            do_run_tests_target (node_get_value(node));
        }

        cursor = cdr(cursor);
    }

    return icemake_loop_processes (im);
}
