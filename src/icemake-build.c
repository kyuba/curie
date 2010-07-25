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

#include <curie/filesystem.h>
#include <icemake/icemake.h>

static int build_object(sexpr desc, struct target *t)
{
    sexpr type = car(desc);
    sexpr source = car(cdr(desc));
    sexpr target = car(cdr(cdr(desc)));

    if (t->toolchain->build_object !=
            (int (*)(struct target *, sexpr, sexpr, sexpr))0)
    {
        return t->toolchain->build_object (t, type, source, target);
    }

    return 1;
}

static void create_special_files (struct target *t)
{
    char buffer[BUFFERSIZE];
    int i = 0;
    struct io *io;
    char c;
    sexpr b = sx_join (t->buildbase, t->toolchain->uname,
                sx_join (str_sincludes, t->name, str_sversiondh)),
          n, o;
    const char *nx = sx_string (t->name), *ox;

    io = io_open_write (sx_string (b));

    io_collect (io, "/* this file was generated by icemake */\n\n", 42);

    for (i = 0; (i < (BUFFERSIZE - 2)) && nx[i]; i++)
    {
        c = nx[i];

        if (c == '+')
        {
            buffer[i] = 'x';
        }
        else if ((c < 'a') && (c > 'z') && (c < 'A') && (c > 'Z'))
        {
            buffer[i] = '_';
        }
        else
        {
            buffer[i] = c;
        }
    }

    buffer[i] = 0;

    n = make_string (buffer);

    o = sx_join (str_hdefines, n,
          sx_join (str_uname, str_sq,
            sx_join (t->name, str_qnl, sx_nil)));

    if (stringp (t->durl))
    {
        o = sx_join (o, str_hdefines,
              sx_join (n, str_uurl,
                sx_join (str_sq, t->durl, str_qnl)));
    }
        
    o = sx_join (o, str_hdefines,
          sx_join (n, str_uversion,
            sx_join (str_space, t->dversion, str_nl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_uversion_s,
            sx_join (str_sq, t->dversion, str_qnl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_ubuild_number,
            sx_join (str_space, t->buildnumber, str_nl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_ubuild_number_s,
            sx_join (str_sq, t->buildnumber, str_qnl)));

    o = sx_join (o, str_hdefines,
          sx_join (n, str_uversion_long,
            sx_join (str_sq, t->name,
              sx_join (str_dash, t->dversion,
                sx_join (str_dot, t->buildnumber, str_qnl)))));

    ox = sx_string (o);
    for        (i = 0; ox[i]; i++);
    io_collect (io, ox, i);

    io_flush   (io);
    io_close   (io);
}

static int do_build_target (struct target *t)
{
    sexpr c = t->code;

    create_special_files (t);

    t->buildnumber = make_integer (sx_integer (t->buildnumber) + 1);

    if (t->toolchain->build != (int (*)(struct target *))0)
    {
        return t->toolchain->build (t);
    }

    while (consp (c))
    {
        build_object(car(c), t);

        c = cdr (c);
    }

    return 0;
}

int icemake_build (struct icemake *im)
{
    sexpr cursor = im->buildtargets;

    im->workstack =
        cons (cons (sym_phase, cons (sym_build, sx_end_of_list)),
              im->workstack);

    while (consp(cursor))
    {
        sexpr sxcar = car(cursor);
        const char *target = sx_string (sxcar);
        struct tree_node *node =
            tree_get_node_string (&(im->targets), (char *)target);

        if (node != (struct tree_node *)0)
        {
            do_build_target (node_get_value(node));
        }

        cursor = cdr(cursor);
    }
    
    im->workstack =
        cons (cons (sym_phase, cons (sym_completed, sx_end_of_list)),
              im->workstack);

    return 0;
}
