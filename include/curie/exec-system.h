/**\internal
 *
 * @{
 */

/**\file
 * \brief Glue Code Header for exec.h
 *
 * Contains system-specific or internal definitions used to implement the exec.h
 * functionality.
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

#ifndef LIBCURIE_EXEC_SYSTEM_H
#define LIBCURIE_EXEC_SYSTEM_H

/**\brief Generalised Return Status Codes */
enum wait_return {
    wr_running = 0, /**< Process is still running. */
    wr_exited = 1,  /**< Process has terminated. */
    wr_killed = 2   /**< Process has terminated due to a signal. */
};

/**\brief Wrapper for the wait4() System Call
 *
 * Wraps around wait() or an equivalent system call to wait on a specific
 * process.
 *
 * \param[in]  pid    The PID to wait for.
 * \param[out] status The location of an integer to store the result in.
 *
 * \return The current status of the process.
 */
enum wait_return a_wait(int pid, int *status);

/**\brief Wait for any process to terminate
 *
 * Wrapper function to wait for any child process to change state.
 *
 * \param[out] status The location of an integer to store the result in.
 *
 * \returns The pid of the process that changed status.
 */
int a_wait_all (int *status);

#endif

/** @} */
