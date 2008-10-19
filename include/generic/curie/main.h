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
 *  Actually the characteristics of the C library are so loose, it should even
 *  be possible to port libcurie's functionality to freestanding environments.
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
