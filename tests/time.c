/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2009, Kyuba Project Members
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
#include <curie/sexpr.h>
#include <curie/time.h>
#include <curie/time-system.h>

int cmain()
{
    int_time date = dt_get_kin ();
/*
    unsigned int time = dt_get_time ();
    struct sexpr_io *io = sx_open_stdio ();
    struct date d;

    dt_split_kin (date, &d);

    sx_write (io, make_integer (date));
    sx_write (io, cons (make_integer (d.alautun),
                  cons (make_integer (d.kinchiltun),
                  cons (make_integer (d.kalabtun),
                  cons (make_integer (d.piktun),
                  cons (make_integer (d.baktun),
                  cons (make_integer (d.katun),
                  cons (make_integer (d.tun),
                  cons (make_integer (d.winal),
                  cons (make_integer (d.kin), sx_end_of_list))))))))));
    sx_write (io, make_integer((time / 100000)));
*/

    return date < UNIX_EPOCH;
}
