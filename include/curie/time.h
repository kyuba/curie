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

/*! \file
 *  \brief Basic Date and Time Functions
 *
 *  This stuff should be easier to calculate in... and it should last a lot
 *  longer with respect to date wrapping issues.
 *
 *  I'm just not sure yet how long an int_64 will suffice.
 */

#ifndef LIBCURIE_TIME_H
#define LIBCURIE_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/int.h>

typedef int_64 int_time;

struct date
{
    unsigned char alautun;    /*!< 20 k'inchiltun per alautun */
    unsigned char kinchiltun; /*!< 20 kalabtun per k'inchiltun */
    unsigned char kalabtun;   /*!< 20 piktun per kalabtun */
    unsigned char piktun;     /*!< 20 b'ak'tun per piktun */
    unsigned char baktun;     /*!< 20 k'atun per b'ak'tun */
    unsigned char katun;      /*!< 20 tun per k'atun */
    unsigned char tun;        /*!< 18 winal per tun */
    unsigned char winal;      /*!< 20 k'in per winal */
    unsigned char kin;        /*!< k'in, basic unit */
};

struct dt
{
    int_time      date;
    double        time;
};

int_time dt_get_kin   ();
int_time dt_make_kin  (struct date *date);
void     dt_split_kin (int_time kin, struct date *date);

/*! \brief Get Current Time
 *  \return How far into the day we are already, in % (UTC).
 */
double dt_get_time  ();

void   dt_get (struct dt *dt);

#ifdef __cplusplus
}
#endif

#endif
