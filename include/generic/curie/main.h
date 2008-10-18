/*
 *  main.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 26/05/2008.
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

/*! \file
 *  \brief Main Function
 *
 *  This header declares the cmain() function for libcurie applications and the
 *  cexit() function.
 */

#ifndef LIBCURIE_MAIN_H
#define LIBCURIE_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Terminate Process
 *  \param[in] status The status code to return.
 *
 *  After calling this function, the process will terminate and return the given
 *  status code to the calling process.
 */
/*@noreturn@*/ void cexit (int status);


/*! \brief Main Function
 *  \return The status code to return to the calling process.
 *
 *  This is the main entry point for plain curie applications. Once this
 *  function terminates, the process will also terminate.
 */
int cmain();

/*! \brief Command-line Arguments
 *
 *  This array contains the command-line arguments passed to the application, if
 *  applicable.
 */
/*@null@*/ extern char **curie_argv;

/*! \brief Process Environment
 *
 *  This array contains the process environment that the prgramme was run in, if
 *  applicable.
 */
/*@null@*/ extern char **curie_environment;

#ifdef __cplusplus
}
#endif

#endif /* LIBCURIE_MAIN_H */
