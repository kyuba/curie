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

/*! \internal
 *
 * @{
 */

/*! \file
 *  \brief Glue Code Header for exec.h
 *
 */

#ifndef LIBCURIE_EXEC_SYSTEM_H
#define LIBCURIE_EXEC_SYSTEM_H

/*! \brief Generalised Return Status Codes */
enum wait_return {
    wr_running = 0,
    wr_exited = 1,
    wr_killed = 2
};

/*! \brief Wrapper for the fork() System Call
 *  \return -1 if the call failed. If the call succeeded, 0 if we're in the
 *          child process or the PID of the child process if we're in the parent
 *          process.
 */
int a_fork();

/*! \brief Wrapper for the wait4() System Call
 *  \param[in]  pid    The PID to wait for.
 *  \param[out] status The location of an integer to store the result in.
 *  \return The current status of the process.
 */
enum wait_return a_wait(int pid, int *status);

int a_wait_all (int *status);

/*! \brief Wrapper for the execve() System Call
 *  \param[in] image The process image to execute.
 *  \param[in] argv  The argument vector for the new process.
 *  \param[in] env   The environment vector for the new process.
 */
void a_exec(const char *image, char **argv, char **env);

int a_set_sid ();

#endif

/*! @} */
