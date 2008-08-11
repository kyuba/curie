/*
 *  environment.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 11/08/2008.
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

#include <atomic/main.h>

#define ARGV0 "./tests/environment"

int a_main(void) {
    unsigned int i;

    if (atomic_argv == (char **)0)
        return 1;

    for (i = 0; (atomic_argv[0][i] == ARGV0[i]) &&
                (ARGV0[i] != (char)0); i++);

    if (ARGV0[i] != (char)0) return 3;

    if (atomic_environment == (char **)0)
        return 2;

    for (i = 0; atomic_environment[i] != (char *)0; i++) {
        if ((atomic_environment[i][0] == 'P') &&
            (atomic_environment[i][1] == 'A') &&
            (atomic_environment[i][2] == 'T') &&
            (atomic_environment[i][3] == 'H') &&
            (atomic_environment[i][4] == '=')) {

            return 0;
        }
    }

    return 4; /* reach when there's no PATH= environment variable */
}
