/*
 *  int.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 12/06/2008.
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
 *  \brief Specific-length integers
 *
 *  This file is rather platform/architecture specific, so this header file
 *  may need porting, depending on the target architecture. for this very
 *  purpose, the build system is capable of feeding the compiler with
 *  appropriate path information to allow overrides in the include/ directory.
 *  this means, instead of mucking with wickedass macromagic, just add an
 *  appropriate directory under include/ and copy this file there.
 *
 *  This specific file is the generic version which so far seems to work across
 *  all the tested targets.
 */

#ifndef LIBCURIE_INT_H
#define LIBCURIE_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Unsigned Integer with the Length of a Pointer
 *
 * Longs should be the same length as pointers, really... we'll see compiler
 * warnings if it ain't anyway, but the long<>pointer size thing holds true on
 * all the archs i've tried so far (which means x86, amd64 and ppc).
 */
typedef unsigned long int_pointer;

/*! \brief Unsigned Integer with a Length of 64 Bits
 *
 *  This could be most troublesome between archs. I see trouble for this on at
 *  least DOS/x86 in real mode.
 */
typedef unsigned long long int_64;

/*! \brief Unsigned Integer with a Length of 32 Bits
 *
 *  Most archs these days seem to have 32-bit ints.
 */
typedef unsigned int int_32;

/*! \brief Unsigned Integer with a Length of 16 Bits
 *
 *  This should be true on just about anything, really.
 */
typedef unsigned short int_16;

/*! \brief Unsigned Integer with a Length of 8 Bits
 *
 *  I know of at least one arch that uses 9-bit chars, but it's rare and the
 *  arch is way old.
 */
typedef unsigned char int_8;

#ifdef __cplusplus
}
#endif

#endif
