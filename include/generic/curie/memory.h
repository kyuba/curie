/*
 *  memory.h
 *  libcurie
 *
 *  Created by Magnus Deininger on 01/06/2008.
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
 *  \brief Memory Management
 *
 *  Functions to allocate and deallocate memory.
 */

#ifndef LIBCURIE_MEMORY_H
#define LIBCURIE_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/constants.h>

void set_get_mem_recovery_function (void *(*handler)(unsigned long int));
void set_resize_mem_recovery_function (void *(*handler)(unsigned long int, void *, unsigned long int));

void *get_mem(unsigned long int);
void *resize_mem(unsigned long int, void *, unsigned long int);
void free_mem(unsigned long int, void *);

void mark_mem_ro (unsigned long int, void *);
void mark_mem_rw (unsigned long int, void *);

void *get_mem_chunk();

#define free_mem_chunk(p) free_mem(LIBCURIE_PAGE_SIZE, p)

/* might need to find a better way to calculate this */
#define BITSPERBITMAPENTITY (unsigned short)(sizeof(unsigned int)*8)

/* this'll allow managing 512 entries per poolblock */
#define BITMAPMAPSIZE (unsigned short)16
#define BITMAPMAXBLOCKENTRIES (unsigned short)(BITMAPMAPSIZE * BITSPERBITMAPENTITY)

typedef unsigned int bitmap[BITMAPMAPSIZE];

struct memory_pool {
    unsigned long int entitysize;
    unsigned short maxentities;
    unsigned short optimise_counter;

    /*@null@*/ /*@only@*/ struct memory_pool *next;

    bitmap map;
};

#define MEMORY_POOL_INITIALISER(size) {\
  .entitysize = (size), \
  .maxentities = 0, \
  .optimise_counter = 300, \
  .map = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0}, \
  .next = (struct memory_pool *)0 }

struct memory_pool *create_memory_pool (unsigned long int entitysize);
void free_memory_pool (struct memory_pool *);

void *get_pool_mem(struct memory_pool *);
void free_pool_mem(void *);

void optimise_memory_pool (struct memory_pool *);

void *aalloc   (unsigned long);
void *arealloc (unsigned long, void *, unsigned long);
void afree     (unsigned long, void *);

#ifdef __cplusplus
}
#endif

#endif
