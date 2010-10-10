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
#include <syscall/syscall.h>

#if defined(have_sys_ioctl)
#include <sys/ioctl.h>
#endif

static int items_total  = 1;
static int items_have   = 0;
static int items_failed = 0;
static int width        = 80;
static sexpr phase      = sx_false;

static void complete
    (struct icemake *im)
{
    int i, j;

    io_collect (im->visualiser.meta.ice.out, (char *)"\r", 1);

    for (i = 0; i < width; i += 10)
    {
        j = width - i;

        if (j > 10)
        {
            io_collect (im->visualiser.meta.ice.out,
                        (char *)"          ", 10);
        }
        else
        {
            io_collect (im->visualiser.meta.ice.out,
                        (char *)"          ", j);
        }
    }

    io_write (im->visualiser.meta.ice.out, (char *)"\r", 1);
}

static void update_screen
    (struct icemake *im)
{
    if (items_have > items_total)
    {
        items_have = items_total;
    }

    if (items_have == items_total)
    {
        complete (im);
    }
    else if (symbolp (phase))
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
                            * (width - p - 4);
             j < x; j++, i++, p++)
        {
            buffer[i] = '#';
        }

        for (j = 0, x = width - p - 4; j < x; j++, i++, p++)
        {
            buffer[i] = ' ';
        }

        buffer[i]     = ' ';
        buffer[i+1]   = ']';
        buffer[i+2]   = '\r';
        i += 3;

        io_write (im->visualiser.meta.ice.out, buffer, i);
    }
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

    for (j = 0, x = (width - p - 2);
         (j < x) && (programme[j] != (char)0); j++, i++, p++)
    {
        buffer[i] = programme[j];
    }

    for (j = 0, x = (width - p + 1); j < x; j++, i++, p++)
    {
        buffer[i] = ' ';
    }

    buffer[i]   = '\n';
    i++;

    io_write (im->visualiser.meta.ice.out, buffer, i);
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

static void icemake_read
    (struct icemake *im, sexpr sx)
{
    if (consp (sx))
    {
        sexpr c = car (sx);

        if (truep (equalp (c, sym_install)) ||
            truep (equalp (c, sym_symlink)))
        {
            items_have++;
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
    update_screen (im);
}

static void on_command_done
    (struct icemake *im, sexpr cmd)
{
    items_have++;
    update_screen (im);
}

static void on_error
    (struct icemake *im, enum icemake_error error, sexpr sx)
{
    if (error != ie_problematic_signal)
    {
        sexpr code;
        sexpr programme;

        code      = car (sx);
        programme = cdr (sx);

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

int icemake_prepare_visualiser_ice
    (struct icemake *im)
{
#if defined(have_sys_ioctl) && defined(TIOCGWINSZ)
    struct winsize size;
#endif

    im->visualiser.visualiser      = vs_ice;

    im->visualiser.items           = items;
    im->visualiser.on_command      = on_command;
    im->visualiser.on_command_done = on_command_done;
    im->visualiser.on_error        = on_error;
    im->visualiser.on_warning      = on_warning;
    im->visualiser.meta.ice.out    = io_open_stdout();

    multiplex_add_io_no_callback (im->visualiser.meta.ice.out);

#if defined(have_sys_ioctl) && defined(TIOCGWINSZ)
    if (sys_ioctl (0, TIOCGWINSZ, (long)&size) == 0)
    {
        width = size.ws_col;
    }
#endif

    return 0;
}
