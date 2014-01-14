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
 * A set of functions that implement a multiplexer. The whole multiplexer code
 * basically wraps around the POSIX select() call, so all the multiplexers can
 * do is wait for file descriptor statuses.
 */
struct multiplex_functions
{
    /**\brief Count Function
     *
     * \return mx_ok or mx_immediate_action if the callbacks can be run
     *         directly without waiting for anything.
     *
     * This function is used to count the file descriptors that will be needed
     * for the select() call. Basically any multiplexer will just count its own
     * file descriptors, and then add the number of fds to check for read
     * events to the first integer, and the number of fds to check for write
     * events to the second integer.
     */
    enum multiplex_result (*count)(int *, int *);

    /**\brief Augmenter
     *
     * This is used to add the actual file descriptors to check for to a
     * pre-allocated buffer. The first argument is a pre-allocated buffer for
     * the fds to check for reading, the second is the current position in the
     * buffer, which must be updated after adding things. The third and fourth
     * argument are the same, but for writing.
     */
    void (*augment)(int *, int *, int *, int *);

    /**\brief Select Callback
     *
     * After the main multiplexer's select() call, this function is called. The
     * First argument is an array with fds that can now be read from, the
     * second is the number of elements in the array. The third and fourth
     * arguments are the same for writable fds.
     */
    void (*callback)(int *, int, int *, int);

    /**\brief Next Set of Multiplexer Functions
     *
     * Always set this to (struct multiplex_functions *)0; the multiplexer code
     * uses this to implement a list of multiplexers.
     */
    struct multiplex_functions *next;
};

/**\brief Add Multiplexer
 * \param[in] mx Descriptor for the multiplexer to add.
 *
 * Adds a new multiplexer to the set of multiplexers currently in use. The
 * given mx argument will be used by the multiplexer, so make sure not to
 * deallocate it afterwards or allocate it a volatile function stack.
 */
void multiplex_add (struct multiplex_functions *mx);

/*\brief select() Wrapper
 *
 * Wraps around the select() system call in a system-agnostic way. May
 * substitute the select() syscall for a different, equivalent system call.
 * The function is supposed to wait until any of the read or write file
 * descriptors may be read from.
 *
 * \param[in] rfds The file descriptors that might be read from.
 * \param[in] rnum The number of file descriptors in 'rfds'.
 * \param[in] wfds The file descriptors that might be written to.
 * \param[in] wnum The number of file descriptors in 'wnum'.
 */
void a_select_with_fds (int *rfds, int rnum, int *wfds, int wnum);

#endif

/*! @} */
