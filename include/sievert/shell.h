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

/*! \file
 *  \brief Shell Helpers
 *
 *  Helper functions that emulate shell functionality.
 */

#ifndef LIBCURIE_SHELL_H
#define LIBCURIE_SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/sexpr.h>
#include <curie/main.h>

/*! \brief Look up a Programme File in the PATH
 *  \param[in] environment The environment to look up PATH in.
 *  \param[in] programme   The programme to search for.
 *  \return sx_false if the programme does was not found, otherwise a string
 *          with the absolute path to the programme.
 *
 *  This works analoguous to the shell command "which", thus the name.
 */
sexpr ewhich (char **environment, sexpr programme);

/*! \brief Look up a Programme File in the PATH
 *  \param[in] programme   The programme to search for.
 *  \return sx_false if the programme does was not found, otherwise a string
 *          with the absolute path to the programme.
 *
 *  Analoguous to ewhich(), but it defaults to curie's environment.
 */
#define which(programme) ewhich (curie_environment, programme)

#ifdef __cplusplus
}
#endif

#endif
