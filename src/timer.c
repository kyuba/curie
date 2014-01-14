/**\file
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
*/

#include <curie/constants.h>
#include <curie/main.h>
#include <curie/memory.h>
#include <curie/multiplex.h>
#include <curie/signal.h>
#include <sievert/sexpr.h>
#include <sievert/time.h>
#include <syscall/syscall.h>

define_symbol (sym_link_initialised, "link-initialised");
define_symbol (sym_repeat,           "repeat");
define_symbol (sym_indefinitely,     "indefinitely");
define_symbol (sym_times,            "times");
define_symbol (sym_in,               "in");
define_symbol (sym_every,            "every");
define_symbol (sym_do,               "do");
define_symbol (sym_seconds,          "seconds");
define_symbol (sym_second,           "second");
define_symbol (sym_s,                "s");
define_symbol (sym_tick,             "tick");
define_symbol (sym_then,             "then");
define_symbol (sym_quit,             "quit");

struct output_channel
{
    struct sexpr_io       *io;
    struct output_channel *next;
};

enum event_model
{
    em_seconds
};

struct event
{
    struct datetime          queue_time;
    signed long long         repeat;
    enum   event_model       model;
    union
    {
        unsigned long long seconds;
    } model_data;
    sexpr                    output;
    sexpr                    then;
    struct event            *next;
};

static struct output_channel *output_channels = (struct output_channel *)0;
static struct event          *events          = (struct event *)0;

static void event_queue (void);
static void event_add   (sexpr);

static void output (sexpr sx)
{
    struct output_channel *c = output_channels;

    while (c != (struct output_channel *)0)
    {
        sx_write (c->io, sx);

        c = c->next;
    }
}

static void output_remove (struct sexpr_io *io)
{
    struct output_channel *c  =  output_channels;
    struct output_channel **p = &output_channels;

    while (c != (struct output_channel *)0)
    {
        if (c->io == io)
        {
            *p = c->next;
            free_pool_mem (c);
            return;
        }

        p = &(c->next);
        c =   c->next;
    }
}

static void event_repeat (struct event *e, struct event **p)
{
    if (e->repeat < 0)
    {
        return;
    }

    e->repeat--;

    if (e->repeat == 0)
    {
        *p = e->next;
        free_pool_mem (e);
    }
}

static void event_dispatch
    (struct event *e, struct event **p, unsigned long long ts)
{
    sexpr t = e->then;
    e->queue_time = dt_from_unix (ts);
    output (e->output);
    event_repeat (e, p);

    if (!eolp (t))
    {
        event_add (t);
    }

    event_queue ();
}

static void event_set_signal (unsigned long long cts, unsigned long long nts)
{
#if defined(have_sys_alarm)
    sys_alarm (nts - cts);
#elif defined(have_sys_setitimer)
    struct sc_val { long sec; long usec; } tv = { nts - cts, 0 };
    struct ti_val { struct sc_val i; struct sc_val c; } ti = { tv, tv };

    sys_setitimer (0 /* ITIMER_REAL */, &ti, 0);
#endif
}

static void event_queue ()
{
    struct event *e  =  events;
    struct event **p = &events;

    unsigned long long cts = dt_to_unix (dt_get ());
    unsigned long long nts = 0 - 1;

    while (e != (struct event *)0)
    {
        unsigned long long ts = dt_to_unix (e->queue_time);

        if (e->model == em_seconds)
        {
            ts += e->model_data.seconds;

            if (ts <= cts) /* trigger event */
            {
                event_dispatch (e, p, ts);
                return;
            }
            else if (ts < nts)
            {
                nts = ts;
            }
        }

        p = &(e->next);
        e =   e->next;
    }

    event_set_signal (cts, nts);
}

static struct event *get_event ()
{
    struct memory_pool pool = MEMORY_POOL_INITIALISER (sizeof (struct event));
    struct event      *ev   = get_pool_mem (&pool);

    ev->queue_time         = dt_get ();
    ev->repeat             = 1;
    ev->model              = em_seconds;
    ev->model_data.seconds = 5;
    ev->output             = sx_list1 (sym_tick);
    ev->then               = sx_end_of_list;
    ev->next               = events;

    events                 = ev;

    return ev;
}

#define STAT_HAD_REPEAT  (1 << 0)
#define STAT_SKIP_IN_POP (1 << 1)
#define STAT_PROCESS_IN  (1 << 2)

static void event_add (sexpr sx)
{
    struct event *ev;
    
    if (consp (sx))
    {
        sexpr a    = car (sx);
        sexpr c    = cdr (sx);
        int repeat = 1;
        int stat   = 0;

        if (truep (equalp (a, sym_quit)))
        {
            cexit (0);
        }

        if (truep (equalp (a, sym_repeat)))
        {
            a = car (c);
            c = cdr (c);

            repeat = -1;
            stat  |= STAT_HAD_REPEAT;

            if (integerp (a))
            {
                repeat = sx_integer (a);

                a      = car (c);
                c      = cdr (c);
            }
            else if (truep (equalp (a, sym_indefinitely)))
            {
                repeat = -1;

                a      = car (c);
                c      = cdr (c);
            }
            else
            {
                stat |= STAT_SKIP_IN_POP;
            }
        }

        if (truep (equalp (a, sym_every)))
        {
            a = car (c);
            c = cdr (c);

            if (!(stat & STAT_HAD_REPEAT))
            {
                repeat = -1;
            }

            stat |= STAT_PROCESS_IN;
            stat |= STAT_SKIP_IN_POP;
        }

        if (truep (equalp (a, sym_in)) || (stat & STAT_PROCESS_IN))
        {
            if (!(stat & STAT_SKIP_IN_POP))
            {
                a = car (c);
                c = cdr (c);
            }

            if (integerp (a))
            {
                ev = get_event ();
                ev->repeat             = repeat;
                ev->model_data.seconds = sx_integer (a);

                a = car (c);
                c = cdr (c);

                if (truep (equalp (a, sym_seconds)) ||
                    truep (equalp (a, sym_second)) ||
                    truep (equalp (a, sym_s)))
                {
                    a = car (c);
                    c = cdr (c);
                    /* nothing to do*/
                }

                if (!nexp (a) && falsep (equalp (a, sym_then)))
                {
                    ev->output = a;

                    a = car (c);
                    c = cdr (c);
                }

                if (truep (equalp (a, sym_then)))
                {
                    ev->then = c;
                }
            }
        }
    }
}

static void timer_io_read (sexpr sx, struct sexpr_io *io, void *aux)
{
    if (eofp (sx))
    {
        output_remove (io);
        return;
    }

    event_add   (sx);
    event_queue ();
}

static void output_add (struct sexpr_io *io)
{
    struct memory_pool pool =
        MEMORY_POOL_INITIALISER (sizeof (struct output_channel));
    struct output_channel *out =
        get_pool_mem (&pool);
    struct datetime dt;

    out->io         = io;
    out->next       = output_channels;

    output_channels = out;

    multiplex_add_sexpr (io, timer_io_read, (void *)0);

    dt = dt_get ();

    sx_write (io, sx_list3 (sym_link_initialised,
                            make_integer (dt.date), make_integer (dt.time)));
}

static enum signal_callback_result timer_signal (enum signal signal, void *aux)
{
    event_queue ();

    return scr_keep;
}

static enum signal_callback_result term_signal (enum signal signal, void *aux)
{
    cexit (0);

    return scr_keep;
}

int cmain ()
{
    int i;

    multiplex_signal();
    multiplex_sexpr();

    output_add (sx_open_stdio ());

    for (i = 1; curie_argv [i] != (char *)0; i++)
    {
        int j = 0;
        struct io *in;
        struct sexpr_io *io;
        sexpr sx;

        while (curie_argv[i][j] != (char)0) j++;
        j++;

        in = io_open_buffer (curie_argv[i], j);
        io = sx_open_i (in);

        while (!eofp (sx = sx_read (io)) && !nexp (sx))
        {
            timer_io_read (sx, (struct sexpr_io *)0, (void *)0);
        }
    }

    multiplex_add_signal (sig_alrm,   timer_signal, (void *)0);
    multiplex_add_signal (sig_vtalrm, timer_signal, (void *)0);

    multiplex_add_signal (sig_hup,    term_signal,  (void *)0);
    multiplex_add_signal (sig_segv,   term_signal,  (void *)0);
    multiplex_add_signal (sig_term,   term_signal,  (void *)0);

    while (multiplex() != mx_nothing_to_do);

    return 0;
}
