/*
 *  exec.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 03/06/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*! \defgroup ProgrammeExecution Programme Execution
 *
 *  Functions to execute a different process or to fork the current one.
 *
 *  @{
 */

/*! \file
 *  \brief Programme Execution
 *
 *  Functions from this header are used to execute other programmes and to
 *  query a child process's status.
 */

#ifndef LIBCURIE_EXEC_H
#define LIBCURIE_EXEC_H

#include <curie/io.h>

/*! \brief Flag for exec_call::options: Do not create a stdio Connection */
#define EXEC_CALL_NO_IO 0x0001
/*! \brief Flag for exec_call::options: Purge open File Descriptors */
#define EXEC_CALL_PURGE 0x0002

/*! \brief Description of a Process's Status */
enum process_status {
    /*! \brief Process is still running */
    ps_running = 0,
    /*! \brief Process has terminated */
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

    /*! \brief The Exit-Code of a Process */
    int exitstatus;

    /*! \brief The Process's Status */
    enum process_status status;

    /*! \brief An IO Context to write Data to the Process's stdin */
    struct io *in;

    /*! \brief An IO Context to read Data from the Process's stdout */
    struct io *out;
};

/*! \brief Programme Execution Parameters */
struct exec_call {
    /*! \brief Flags to modify the execution Behaviour. */
    unsigned int options;

    /*! \brief The Command to execute
     *
     *  This is a list of strings, the end of the list must be a 0-pointer, and
     *  the first element of the list is used as the binary to execute.
     *
     *  If this field is 0 itself, then instead of executing a programme the
     *  execute() function will rather fork.
     */
    char **command;

    /*! \brief The Environment to use for the Child Process
     *
     *  A 0-terminated list of KEY=value pairs that will compose a child
     *  process's environment. Only used when command is set to something other
     *  than 0.
     */
    char **environment;
};

/*! \brief Initialise an exec_call Structure
 *  \return A new exec_call structure.
 */
struct exec_call *create_exec_call ();

/*! \brief Free an exec_call Structure
 *  \param[in] call The call to free.
 *
 *  This funtion should rarely be needed, as execute() will already free the
 *  memory associated with its parameter.
 */
void free_exec_call (struct exec_call *call);

/*! \brief Execute a new Process
 *  \param[in] call The structure with parameters of what to execute.
 *  \return A new exec_context.
 *
 *  After calling this function, the given call structure will be invalid.
 */
struct exec_context *execute(struct exec_call *call);

/*! \brief Update an exec_context Structure
 *  \param[in] context The context to update.
 *
 *  This function will basically perform a wait() on the child process
 *  described with the given context.
 */
void check_exec_context (struct exec_context *context);

/*! \brief Free an exec_context Structure
 *  \param[in] context The context to free.
 *
 *  This should only be called after context->status has the value ps_terminated
 *  as after the context has been freed, it is impossible for the process to
 *  perform a wait on that child process.
 */
void free_exec_context (struct exec_context *context);

#endif

/*! @} */