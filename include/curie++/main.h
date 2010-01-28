/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

#ifndef LIBCURIEPP_MAIN_H
#define LIBCURIEPP_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Main Function
 *  \return The status code to return to the calling process.
 *
 *  This is the main entry point for plain curie applications. Once this
 *  function terminates, the process will also terminate.
 */
int cxxmain ( void );

/*! \brief Command-line Arguments
 *
 *  This array contains the command-line arguments passed to the application, if
 *  applicable.
 */
extern char **curie_argv;

/*! \brief Process Environment
 *
 *  This array contains the process environment that the prgramme was run in, if
 *  applicable.
 */
extern char **curie_environment;

#ifdef __cplusplus
}
#endif

#endif /* LIBCURIE_MAIN_H */
