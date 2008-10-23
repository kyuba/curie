/*
 *  exec-system.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 06/08/2008.
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
enum wait_return a_wait(int pid, /*@notnull@*/ /*@out@*/ int *status);

int a_wait_all (int *status);

/*! \brief Wrapper for the execve() System Call
 *  \param[in] image The process image to execute.
 *  \param[in] argv  The argument vector for the new process.
 *  \param[in] env   The environment vector for the new process.
 */
void a_exec(/*@notnull@*/ const char *image,
            /*@notnull@*/ char **argv,
            /*@notnull@*/ char **env);

#endif

/*! @} */
