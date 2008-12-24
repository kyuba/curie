/*
 *  magic-constants.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 20/09/2008.
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
 *  \brief Magic Constants (Internal)
 *
 *  I got these constants on forums.gentoo.org in a thread on how to isolate
 *  zeroes in an integer. They've been devised by Akkara.
 */

#ifndef LIBCURIE_MAGIC_CONSTANTS_H
#define LIBCURIE_MAGIC_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define HASH_MAGIC_MULTIPLIER 0x0458654Du
#define HASH_MAGIC_SHIFT (32 - 6)
#define HASH_MAGIC_TABLE ((int_8 const *)(\
            "\xFF\00\01\15\02\06\16\00\03\00\23\07\17\32\00\00"\
            "\36\04\00\30\26\24\10\00\12\20\33\00\00\00\00\00"\
            "\37\14\05\00\00\22\31\00\35\27\25\00\11\00\00\00"\
            "\13\00\21\00\34\00\00\00\00\00\00\00\00\00\00\00"))
#define HASH_MAGIC_TABLE_MASK 0x3F

#ifdef __cplusplus
}
#endif

#endif
