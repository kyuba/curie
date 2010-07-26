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

#include <curie/memory.h>
#include <sievert/metadata.h>
#include <syscall/syscall.h>

#include <asm/stat.h>

struct metadata_indirection
{
    void (*with_metadata)(struct metadata *, void *);
    void *aux;
};

static void with_metadata_indirect (struct metadata *metadata, void *aux)
{
    struct metadata_indirection *i = (struct metadata_indirection *)aux;

    i->with_metadata (metadata, i->aux);
}

void metadata_from_path
    (const char *path,
     void (*with_metadata) (struct metadata *, void *), void *aux)
{
#if defined(have_sys_newstat)
    struct stat st;

    if (sys_newstat ((char *)path, &st) != 0)
#elif defined(have_sys_stat64)
    struct stat64 st;

    if (sys_stat64 ((char *)path, &st) != 0)
#elif defined(have_sys_stat)
    struct stat st;

    if (sys_stat ((char *)path, &st) != 0)
#else
#warning stat() not available on this platform
    struct { int st_mode, st_uid, st_gid, st_atime, st_mtime, st_ctime, st_size,
                 st_dev } st;

    if (1)
#endif
    {
        struct metadata metadata = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        with_metadata (&metadata, aux);
    }
    else
    {
        struct metadata_indirection i = { with_metadata, aux };
        enum metadata_classification_unix c;
        int attributes;

             if (0xc000 & st.st_mode) { c = mcu_socket;           }
        else if (0xa000 & st.st_mode) { c = mcu_symbolic_link;    }
        else if (0x8000 & st.st_mode) { c = mcu_file;             }
        else if (0x6000 & st.st_mode) { c = mcu_block_device;     }
        else if (0x4000 & st.st_mode) { c = mcu_directory;        }
        else if (0x2000 & st.st_mode) { c = mcu_character_device; }
        else if (0x1000 & st.st_mode) { c = mcu_fifo;             }
        else                          { c = mcu_unknown;          }

        if (0x0800 & st.st_mode) { attributes |= MAT_SET_UID; }
        if (0x0400 & st.st_mode) { attributes |= MAT_SET_GID; }
        if (0x0200 & st.st_mode) { attributes |= MAT_STICKY;  }

        metadata_from_unix
            (c, st.st_uid, st.st_gid, st.st_mode, st.st_atime, st.st_mtime,
             st.st_ctime, st.st_size, st.st_dev, attributes,
             with_metadata_indirect, &i);
    }
}

void metadata_to_path
    (struct metadata *metadata,
     const char *path)
{
#pragma message("metadata_to_path() incomplete")
}

