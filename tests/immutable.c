/*
 *  immutable.c
 *  curie-libc
 *
 *  Created by Magnus Deininger on 14/06/2008.
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

#include "curie/immutable.h"

#define STRING1 "hello world"
#define STRING1_LENGTH (unsigned int)(sizeof(STRING1)-1)

int a_main(void) {
    const char *immutable_ref1 = str_immutable_unaligned (STRING1);
    const char *immutable_ref2 = str_immutable_unaligned (immutable_ref1),
               *immutable_ref3 = str_immutable_unaligned (STRING1);
    unsigned int i;

    if (immutable_ref1 == (const char *)0) return 1;
    if (immutable_ref2 == (const char *)0) return 2;
    if (immutable_ref1 != immutable_ref2) return 3;

    for (i = 0; i < STRING1_LENGTH; i++) {
        if (immutable_ref1[i] != STRING1[i]) return 4;
    }

    if (immutable_ref1 != immutable_ref3) return 5;

    return 0;
}
