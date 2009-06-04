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

#include <curie/tree.h>

#include <stdio.h>

#include <icemake/icemake.h>

/*static void post_process_library_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", archprefix, sx_string(t->name), sx_string(name));

    workstack
        = cons (cons (p_archive_indexer,
                  cons (make_string (buffer),
                        sx_end_of_list))
                , workstack);
}*/

static void post_process_library (sexpr name, struct target *t)
{
/*    switch (uname_toolchain)
    {
        case tc_gcc:
            post_process_library_gcc (name, t); break;
    }*/
}

static void post_process_programme (sexpr name, struct target *t)
{
/*    switch (uname_toolchain)
    {
        case tc_gcc:
            post_process_programme_gcc (name, t); break;
    }*/
}

static void do_post_process_target(struct target *t)
{
    if (truep(t->library))
    {
        post_process_library (t->name, t);
    }
    else if (truep(t->programme))
    {
        post_process_programme (t->name, t);
    }
}

static void post_process_target (const char *target)
{
    struct tree_node *node = tree_get_node_string(&targets, (char *)target);

    if (node != (struct tree_node *)0)
    {
        do_post_process_target (node_get_value(node));
    }
}

static void target_map_post_process (struct tree_node *node, void *u)
{
    do_post_process_target(node_get_value(node));
}

void post_process (sexpr buildtargets)
{
    sexpr cursor = buildtargets;
    sx_write (stdio, cons (sym_phase, cons (sym_post_process, sx_end_of_list)));

    if (eolp(cursor))
    {
        tree_map (&targets, target_map_post_process, (void *)0);
    }
    else while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        post_process_target (sx_string(sxcar));
        cursor = cdr(cursor);
    }

    loop_processes();
}
