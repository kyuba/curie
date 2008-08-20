/*
 *  immutable.h
 *  curie-libc
 *
 *  Created by Magnus Deininger on 10/06/2008.
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

#ifndef ATOMIC_IMMUTABLE_H
#define ATOMIC_IMMUTABLE_H

/* making immutable copies of strings may help reduce the likelihood of
   memory leaks by making sure all immutable copies only exist exactly once
   in the process space, as well as prevent improper access to these strings
   (such as inadvertedly modifying them */

/*@notnull@*/ /*@observer@*/ const char *str_immutable (/*@notnull@*/ /*@returned@*/ const char *);

/* similarly, non-string data should be storable in this way, so here we go. */

/*@notnull@*/ /*@observer@*/ const void *immutable (/*@notnull@*/ /*@returned@*/ const void *, unsigned long);

/* this function is used to force locking of all the current pages that are used
   to store new immutable data. the idea is that if you know you wont be storing
   (much) new stuff anytime soon, you can call this function and immediately get
   the memory protection effect. */

void lock_immutable_pages ( void );

/* the str_immutable() function expects its parameter to be aligned to an 8-byte
   boundary, as well as zero-padded to the next 8-byte boundary, unless its a
   previous return value of itself. this function is a bit of a helper to ensure
   these alignment-constraints in case you dont know if your input meets these
   criteria. */

/*@notnull@*/ /*@observer@*/ const char *str_immutable_unaligned (/*@notnull@*/ /*@returned@*/ const char *);

#endif
