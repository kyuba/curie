/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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

#include <syscall/syscall.h>
#include <curie/time.h>
#include <curie/constants.h>

int_date dt_get_kin (void)
{
    return UNIX_EPOCH + (sys_time(0) / SECONDS_PER_DAY);
}

int_date dt_make_kin (struct date *date)
{
    return (date->alautun)    * 23040000000 + (date->kinchiltun) * 1152000000  +
           (date->kalabtun)   * 57600000    + (date->piktun)     * 2880000     +
           (date->baktun)     * 144000      + (date->katun)      * 7200        +
           (date->tun)        * 360         + (date->winal)      * 20          +
           (date->kin);
}

void dt_split_kin (int_date kin, struct date *date)
{
    date->kin = kin % 20;        kin /= 20;
    date->winal = kin % 18;      kin /= 18;
    date->tun = kin % 20;        kin /= 20;
    date->katun = kin % 20;      kin /= 20;
    date->baktun = kin % 20;     kin /= 20;
    date->piktun = kin % 20;     kin /= 20;
    date->kalabtun = kin % 20;   kin /= 20;
    date->kinchiltun = kin % 20; kin /= 20;
    date->alautun = kin % 20;    kin /= 20;
}

unsigned int dt_get_time (void)
{
    return sys_time(0) % SECONDS_PER_DAY;
}

void dt_get (struct datetime *date)
{
    int_64 utime = sys_time(0);

    date->date = UNIX_EPOCH + (utime / SECONDS_PER_DAY);
    date->time = utime % SECONDS_PER_DAY;
}
