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

#define WIDTH 78
static int items_total  = 1;
static int items_have   = 0;
static int items_failed = 0;
static sexpr phase      = sx_false;

static void update_screen
    (struct icemake *im)
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

    io_write (im->visualiser.meta.ice.out, buffer, i);
}

static void describe_failure
    (struct icemake *im, unsigned int code, const char *programme)
{
    char buffer[0x1000] = " [ 000 ] ";
    int i = 9, p = 10, j, x;

    buffer[5] = '0' + (code % 10);
    code /= 10;
    buffer[4] = '0' + (code % 10);
    code /= 10;
    buffer[3] = '0' + (code % 10);

    for (j = 0, x = (WIDTH - p);
         (j < x) && (programme[j] != (char)0); j++, i++, p++)
    {
        buffer[i] = programme[j];
    }

    for (j = 0, x = WIDTH - p + 2; j < x; j++, i++, p++)
    {
        buffer[i] = ' ';
    }

    buffer[i]   = '\n';
    i++;

    io_write (im->visualiser.meta.ice.out, buffer, i);
}

static void complete
    (struct icemake *im)
{
    io_write (im->visualiser.meta.ice.out,
              (char*)"\r                                              "
                     "                                 \r", 81);
}

static void items
    (struct icemake *im, int count)
{
    items_total = count;
    if (items_total < 1)
    {
        items_total = 1;
    }
    items_have = 0;
}

static void icemake_read (struct icemake *im, sexpr sx)
{
    if (consp (sx))
    {
        sexpr c = car (sx);

        if (truep(equalp (c, sym_items_remaining)))
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

            if (falsep (equalp (r, sym_completed)))
            {
                phase = r;
            }
        }

        update_screen (im);
    }
}

static void on_command
    (struct icemake *im, sexpr cmd)
{
    icemake_read (im, cmd);
}

static void on_command_done
    (struct icemake *im, sexpr cmd)
{
    items_have++;

    if (items_have > items_total)
    {
        items_have = items_total;
    }

    if (items_have == items_total)
    {
        complete (im);
    }
}

static void on_error
    (struct icemake *im, enum icemake_error error, sexpr sx)
{
    sexpr c;

    if (error != ie_problematic_signal)
    {
        sexpr code;
        sexpr programme;

        c         = cdr (sx);
        code      = car (c);
        programme = car (cdr (c));

        describe_failure (im, sx_integer (code), sx_string (programme));

        items_failed++;
        if (items_have > items_total)
        {
            items_have = items_total;
        }
    }
}

static void on_warning
    (struct icemake *im, enum icemake_error error, sexpr sx)
{
    on_error (im, error, sx);
}

int icemake_prepare_visualiser_ice (struct icemake *im)
{
    im->visualiser.visualiser      = vs_ice;

    im->visualiser.items           = items;
    im->visualiser.on_command      = on_command;
    im->visualiser.on_command_done = on_command_done;
    im->visualiser.on_error        = on_error;
    im->visualiser.on_warning      = on_warning;
    im->visualiser.meta.ice.out    = io_open_stdout();

    multiplex_add_io_no_callback (im->visualiser.meta.ice.out);

    return 0;
}
