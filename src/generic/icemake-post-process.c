/*
 *  icemake-post-process.c
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

#include <stdio.h>

#include <icemake/icemake.h>

static void post_process_library_gcc (sexpr name, struct target *t)
{
    char buffer[BUFFERSIZE];

    snprintf (buffer, BUFFERSIZE, "build/%s/%s/lib%s.a", sx_string(t->name), archprefix, sx_string(name));

    workstack
        = cons (cons (p_archive_indexer,
                  cons (make_string (buffer),
                        sx_end_of_list))
                , workstack);
}

static void post_process_library (sexpr name, struct target *t)
{
    switch (uname_toolchain)
    {
        case tc_gcc:
            post_process_library_gcc (name, t); break;
    }
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
    else
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
