/**\internal
 *
 * @{
 */

/**\file
 * \brief Glue Code Header for multiplex.h
 * \internal
 *
 * \note These descriptions apply to the multiplexer for POSIX-ish systems, for
 *       the equivalent functionality on Windows system, see the header files
 *       in the include/windows/ directory.
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

#ifndef LIBCURIE_MULTIPLEX_SYSTEM_H
#define LIBCURIE_MULTIPLEX_SYSTEM_H

#include <curie/multiplex.h>

/**\brief Multiplexer Functions
 * \internal
 *
 * A set of functions that implement a multiplexer. The windows version of the
 * multiplexer is basically a wrapper around the WaitForMultipleObjects() call,
 * so all the multiplexer functions need to do is shuffle around handles.
 */
struct multiplex_functions {
    /**\brief Count Function
     * \return mx_ok or mx_immediate_action if the callbacks can be run
     *         directly without waiting for anything.
     *
     * This function is used to count the handles that will be needed for the
     * WaitForMultipleObjects() call. Count your own and add them to the first
     * paramteter.
     */
    enum multiplex_result (*count)(int *);

    /**\brief Augmenter
     *
     * This is used to add the actual handles to check for to a pre-allocated
     * buffer. The first argument is this pre-allocated buffer for the handles,
     * the second argument is the current position in the buffer, which needs
     * to be updated upon returning from the function.
     */
    void (*augment)(void **, int *);

    /**\brief Callback
     *
     * After the main multiplexer's WaitForMultipleObjects() call, this
     * function is called. The first argment is the list of handles for the
     * call, and the second argument is the number of handles in this list.
     * You'll need to find out for yourself if there are any pending updates.
     */
    void (*callback)(void **, int);

    /**\brief Next Set of Multiplexer Functions
     * \internal
     *
     * Always set this to (struct multiplex_functions *)0; the multiplexer code
     * uses this to implement a list of multiplexers.
     */
    struct multiplex_functions *next;
};

/**\brief Add Multiplexer
 * \internal
 * \param[in] mx Descriptor for the multiplexer to add.
 *
 * Adds a new multiplexer to the set of multiplexers currently in use. The
 * given mx argument will be used by the multiplexer, so make sure not to
 * deallocate it afterwards or allocate it a volatile function stack.
 */
void multiplex_add (struct multiplex_functions *mx);

#endif

/*! @} */
