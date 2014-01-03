/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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
 *  \brief Date and Time Conversion Functions
 *
 *  Since there's at least a dozen different calendar and time systems in use
 *  these days, the functions in this header allow you to convert date and time
 *  between different formats.
 */

#ifndef LIBSIEVERT_TIME_H
#define LIBSIEVERT_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/time.h>

#define LEAP_YEAR (1 << 0)

struct gregorian_date
{
    long int      year;
    unsigned char month;
    unsigned char day;
    unsigned char modifiers;
};

struct julian_date
{
    long int      year;
    unsigned char month;
    unsigned char day;
    unsigned char modifiers;
};

struct datetime dt_from_unix
    (unsigned long long timestamp);
struct datetime dt_from_julian_day
    (long double julian_day);
struct datetime dt_from_julian_date
    (struct julian_date julian_date);
struct datetime dt_from_gregorian_date
    (struct gregorian_date gregorian_date);

unsigned long long    dt_to_unix           (struct datetime date);
long double           dt_to_julian_day     (struct datetime date);
struct julian_date    dt_to_julian_date    (struct datetime date);
struct gregorian_date dt_to_gregorian_date (struct datetime date);

#ifdef __cplusplus
}
#endif

#endif
