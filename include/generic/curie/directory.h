/*
 *  directory.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 27/12/2008.
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
 *  \brief Directory Listing/Filtering
 *
 *  Functions to retrieve the contents of directories.
 */

#ifndef LIBCURIE_DIRECTORY_H
#define LIBCURIE_DIRECTORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/regex.h>

/*! \brief Read Directory, filtered with a Regular Expression
 *  \param[in] base The base directory to search.
 *  \param[in] rx   Pattern to match against the directory entries.
 *  \return A list of files in the directory.
 *
 *  Search the given directory and create a list of all files in the directory
 *  that match the given regular expression. The list of files doesn't contain
 *  the base path, just the filenames.
 *
 *  Example: read_directory_rx("/", rx_compile(".*"));
 *  Would return something like this list:
 *  ("bin" "lib" "dev" "sbin" "usr" "opt" "var" "etc" "root" "home")
 */
sexpr read_directory_rx (const char *base, struct graph *rx);

/*! \brief Read Directory, filtered with an S-expression
 *  \param[in] rx   The Pattern to use.
 *  \return A list of files matching the pattern.
 *
 *  Simple wrapper around read_directory_rx() and read_directory(). If rx is a
 *  string, the string is passed to read_directory. If it's a cons, it's assumed
 *  to be an NFA graph in s-expression form, the graph is converted and passed
 *  to read_directory() with "." as the base path.
 */
sexpr read_directory_sx (sexpr rx);

/*! \brief Read Directory, filtered with a C-style string
 *  \param[in] p    The Pattern to use.
 *  \return A list of files matching the pattern.
 *
 *  The string is split by '/' and each of the tokens is used as a regular
 *  expression to ultimately specify a number of files to find. It's annoying to
 *  describe verbally, so an example is probably best:
 *
 *  read_directory("/.+/lib");
 *  should yield: ("/././lib" "/./usr/lib" "/./var/lib")
 *
 *  and
 *
 *  read_directory(".+/lib");
 *  could yield (if you're in the root directory):
 *  ("././lib" "./usr/lib" "./var/lib")
 *
 *  Note that a leading "/" always results in a "/." prefix of the resulting
 *  list and any other character results in a simple "." prefix. This is
 *  technically overly verbose, but it shouldn't really matter. Also, the list
 *  of files that is returned are full paths, relative to the current directory.
 */
sexpr read_directory    (const char *p);

#ifdef __cplusplus
}
#endif

#endif
