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

/*! \mainpage
 *
 *  \section introduction Introduction
 *
 *  This is libcurie, a small, minimalistic C library. It includes the bare
 *  minimum in functionality to get some use out of C, and some high-level
 *  concepts, such as a parser for s-expressions, trees and graphs. The reason
 *  for writing this C library is twofold. One reason is that if you try to
 *  write portable C code, you'll soon find that neither sticking to ANSI C nor
 *  POSIX C is portable enough to actually work, even on the more common
 *  operating systems. The other reason is that usually the main C library is
 *  quite large, and the functionality needed from it is quite small.
 *
 * \section portable Portable, you say?
 *
 *  As most C programmers are aware of, POSIX is a nice standard... but it's not
 *  implemented consistently. A lot of operating systems claim POSIX compliance,
 *  but mysteriously you still need to work around bugs or differences in
 *  implementations left and right. Or sometimes the interface of your choice is
 *  implemented in all the OSs you care for, but the implementation plain sucks
 *  on one of them.
 *
 *  This whole situation sucks... so this library is intended to help with that.
 *  It tries to do this by trying to come up with a new, fresh, consistent API
 *  that should be implementable across all operating systems; even tricky ones
 *  like DOS. For this reason, the build system is unusually complex, and it is
 *  able to use different source files based on a wide number of characteristics
 *  of the target architecture, such as operating system, cpu architecture and
 *  the compiler toolchain that is used.
 *
 *  Actually this C library's interface should even be portable to freestanding
 *  environments.
 *
 *  \section small We've had small libcs, haven't we?
 *
 *  We sure have seen a lot of small libcs... but there is only so small that
 *  you can get if you try to implement POSIX, or ANSI/ISO C. Luckily, we don't
 *  give a rat's arse about POSIX compliance. And the only C standards we dig
 *  are the language/syntax parts of the recent C standards, not the library
 *  part. Since it's perfectly legitimate to use C in freestanding environments,
 *  this shouldn't even violate the C standard itself.
 *
 *  To give some actual numbers of just how small curie applications can get, it
 *  might be fun to compile curie and its test suite on linux/amd64 or linux/ppc
 *  and to then examine the filesizes for libcurie.a and the test suite. Hint:
 *  the largest statically linked(!) binary in the test suite is 23 kilobytes on
 *  linux/amd64. Unfortunately the smallest is a rough 6.4 kilobytes on that
 *  architecture, mostly due to overly complex argv/environment handling there.
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

#if defined(__GNUC__)
#define NORETURN __attribute__((noreturn))
#endif

#if !defined(NORETURN)
#define NORETURN
#endif

/*! \brief Terminate Process
 *  \param[in] status The status code to return.
 *
 *  After calling this function, the process will terminate and return the given
 *  status code to the calling process.
 */
extern void cexit (int status) NORETURN;

/*! \brief Main Function
 *  \return The status code to return to the calling process.
 *
 *  This is the main entry point for plain curie applications. Once this
 *  function terminates, the process will also terminate.
 */
extern int cmain ( void );

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

#endif
