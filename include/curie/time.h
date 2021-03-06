/**\file
 * \brief Basic Date and Time Functions
 *
 * Contains basic date and time functions. It's not the Gregorian calendar, but
 * this stuff should be easier to calculate in... and it should last a lot
 * longer with respect to date wrapping issues.
 *
 * I'm just not sure yet how long an int_64 will suffice.
 *
 * Oh yeah, in case you're not familiar with the calendar: it's the
 * mesoamerican long count. And no, the mayan calendar doesn't "end" on
 * 13.0.0.0.0, that's just bollocks. In fact, it'll happily last a lot
 * longer than that.
 *
 * It is my understanding that the way I'm querying the OS will result in time
 * data in GMT. So, unless otherwise noted, all date and time functions return
 * their data in GMT.
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

#ifndef LIBCURIE_TIME_H
#define LIBCURIE_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/int.h>

/**\brief Date Type
 *
 * This type is used to hold the day portion of dates.
 */
typedef int_64 int_date;

/**\brief Date Type (Split Dates)
 *
 * This structure is used to hold split dates from dt_split_kin().
 */
struct date
{
    unsigned int alautun    : 5; /*!< 20 k'inchiltun, 23040000000 k'in*/
    unsigned int kinchiltun : 5; /*!< 20 kalabtun,    1152000000 k'in*/
    unsigned int kalabtun   : 5; /*!< 20 piktun,      57600000 k'in */
    unsigned int piktun     : 5; /*!< 20 b'ak'tun,    2880000 k'in */
    unsigned int baktun     : 5; /*!< 20 k'atun,      144000 k'in */
    unsigned int katun      : 5; /*!< 20 tun,         7200 k'in */
    unsigned int tun        : 5; /*!< 18 winal,       360 k'in */
    unsigned int winal      : 5; /*!< 20 k'in */
    unsigned int kin        : 5; /*!< k'in, basic unit */
};

/**\brief Date/Time Type
 *
 * This structure is used to hold date and time values together.
 */
struct datetime
{
    int_date      date;      /*!< Days since the epoch */
    unsigned int  time;      /*!< Seconds since the start of the day */
};

/**\brief Get the current Date
 * \return Days since the epoch.
 *
 * Queries the OS for the current date.
 */
int_date        dt_get_kin   (void);

/**\brief Convert a struct date to an int_date.
 * \param[in] date The date to join up.
 * \return Days since the epoch.
 *
 * Creates an int_date using the data from the given struct date.
 */
int_date        dt_make_kin  (struct date date);

/**\brief Split a Date
 * \param[in]  kin  Days since the epoch.
 * \return The date.
 *
 * This function splits up dates (as represented by the days since the epoch)
 * into its components (for output to humans).
 */
struct date     dt_split_kin (int_date kin);

/**\brief Get Current Time
 * \return How far into the day we are already, in % (UTC).
 *
 * Queries the OS for the current time of day.
 */
unsigned int    dt_get_time  (void);

/**\brief Get Current Date and Time
 * \return date A struct datetime to hold the current date and time.
 *
 * Queries the OS for the current date and time.
 */
struct datetime dt_get       (void);

#ifdef __cplusplus
}
#endif

#endif
