/*
 *  filesystem.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 23/10/2008.
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
 *  \brief Filesystem Information
 *
 *  The functions in this header file are used to query the OS about certain
 *  pieces of filesystem information.
 */

#ifndef LIBCURIE_FILESYSTEM_H
#define LIBCURIE_FILESYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/sexpr.h>

/*! \brief Check if a File exists
 *  \param[in] path The path to check.
 *  \return sx_false if the file does not exist, sx_true if it does exist.
 *
 *  This function basically uses stat() to figure out if the given file exists.
 */
sexpr filep(sexpr path);

/*! \brief Check if a Link exists
 *  \param[in] path The path to check.
 *  \return sx_false if the link does not exist, sx_true if it does exist.
 *
 *  Same as filep(), but for symlinks it only checks the presence of the
 *  symlink, as opposed to checking for the actual file referenced by the
 *  symlink.
 */
sexpr linkp(sexpr path);

#ifdef __cplusplus
}
#endif

#endif
