/**\file
 * \brief Date and Time Conversion Functions
 *
 * Since there's at least a dozen different calendar and time systems in use
 * these days, the functions in this header allow you to convert date and time
 * between different formats.
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

#ifndef LIBSIEVERT_TIME_H
#define LIBSIEVERT_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/time.h>

/**\brief Convert UNIX time to Curie date/time
 *
 * This function converts a UNIX time stamp to a Curie-style date/time record.
 *
 * \param[in] timestamp The UNIX time stamp to convert.
 *
 * \returns A Curie-style datetime struct that corresponds to the given UNIX
 *          time stamp.
 */
struct datetime dt_from_unix
    (unsigned long long timestamp);

/**\brief Convert Curie date/time record to UNIX time
 *
 * Converts a Curie-style date/time record to a UNIX time stamp, which is the
 * number of seconds that have passed since the first of January, 1970.
 *
 * \param[in] date A Curie-style datetime struct to convert.
 *
 * \returns The UNIX time stamp that corresponds to the given date/time record.
 */
unsigned long long    dt_to_unix           (struct datetime date);

#ifdef __cplusplus
}
#endif

#endif
