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
 *  \brief Programme Execution
 *
 *  Functions from this header are used to execute other programmes, fork the
 *  process and to query a child process's status.
 */

#ifndef LIBCURIE_EXEC_H
#define LIBCURIE_EXEC_H

#include <curie/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Do not create a stdio Connection
 *
 *  This flag tells the execute() function not to create the typical I/O sockets
 *  that are created without this flag. In essence this leaves the three stdio
 *  file descriptors in peace and it means that context->in and context->out
 *  will be undefined.
 */
#define EXEC_CALL_NO_IO 0x0001
/*! \brief Purge open File Descriptors
 *
 *  With this flag set, ALL file descriptors are closed in the child process.
 */
#define EXEC_CALL_PURGE 0x0002

/*! \brief Create Session
 *
 *  This flag makes execute() create a new session after forking.
 */
#define EXEC_CALL_CREATE_SESSION 0x0004

/*! \brief Description of a Process's Status */
enum process_status {
    /*! \brief Process is still running
     *
     *  This value is used to express that the checked process has not been
     *  terminated just yet.
     */
    ps_running = 0,

    /*! \brief Process has terminated
     *
     *  This value may mean that the process was killed, or that it exited
     *  voluntarily.
     */
    ps_terminated = 1
};

/*! \brief Programme Execution Context
 *
 *  This struct is used to keep track of a running child process.
 */
struct exec_context {
    /*! \brief The PID of a Process
     *
     *  This is a unique identifier for a process.
     */
    int pid;

    /*! \brief The Exit-Code of a Process
     *
     *  You should only count on being able to use exit codes in the char range.
     */
    int exitstatus;

    /*! \brief The Process's Status
     *
     *  The current status of the process, as determined by the last time
     *  check_exec_context() was run, or the last time mutex() was run when
     *  using the process multiplexer on this context.
     */
    enum process_status status;

    /*! \brief An IO Context to read Data from the Process's stdout
     *
     *  Usually sockets are used for this IO context, so you shouldn't fear any
     *  SIGPIPEs. Then again, if you use the process multiplexer, you need not
     *  worry about SIGPIPEs anyway.
     *
     *  \note This is undefined when using the EXEC_CALL_NO_IO flag.
     */
    /*@null@*/ /*@only@*/ struct io *in;

    /*! \brief An IO Context to write Data to the Process's stdin
     *
     *  Usually sockets are used for this IO context, so you shouldn't fear any
     *  SIGPIPEs. Then again, if you use the process multiplexer, you need not
     *  worry about SIGPIPEs anyway.
     *
     *  \note This is undefined when using the EXEC_CALL_NO_IO flag.
     */
    /*@null@*/ /*@only@*/ struct io *out;
};

/*! \brief Execute a new Process
 *  \param[in] options     OR-combination of EXEC_* Flags to control the
 *                         behaviour of the function.
 *  \param[in] command     The command to execute, as an array of strings.
 *  \param[in] environment The environment to pass to the new process, as an
 *                         array of strings (ignored if command == (char **)0).
 *  \return A new exec_context.
 *
 *  Command and environment may both be (char **)0 instead of arrays. If
 *  command is (char **)0, the function essentially just forks instead of
 *  running a programme.
 */
/*@null@*/ /*@only@*/
struct exec_context *execute
        (unsigned int options, /*@notnull@*/ char **command,
         /*@notnull@*/ char **environment);

/*! \brief Update an exec_context Structure
 *  \param[in] context The context to update.
 *
 *  This function will basically perform a wait() on the child process
 *  described with the given context.
 */
void check_exec_context
        (/*@notnull@*/ struct exec_context *context);

/*! \brief Free an exec_context Structure
 *  \param[in] context The context to free.
 *
 *  This should only be called after context->status has the value ps_terminated
 *  as after the context has been freed, it is impossible for the process to
 *  perform a wait on that child process.
 */
void free_exec_context
        (/*@notnull@*/ /*@only@*/ struct exec_context *context);

#ifdef __cplusplus
}
#endif

#endif

/*! @} */
