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

#include <curie/multiplex.h>
#include <curie/memory.h>
#include <curie/int.h>
#include <curie/regex.h>
#include <sievert/cpio.h>

enum cpio_options
{
    co_none,
    co_file_is_padded,
    co_have_end_of_archive
};

struct cpio_read_data
{
    enum cpio_options options;
    void (*on_new_file) (struct io *io, const char *name, void *aux);
    void (*on_end_of_archive) (void *aux);
    void *aux;
    int remaining_file_data;
    struct io *current_file;
    sexpr regex;
};

void multiplex_cpio ()
{
    static char installed = (char)0;

    if (installed == (char)0)
    {
        multiplex_io ();
        installed = (char)1;
    }
}

static void cpio_process_fragment
    (struct io *io, void *aux)
{
    struct cpio_read_data *data = (struct cpio_read_data *)aux;
    int pos = io->position, len = io->length, rem;
    int_16 ts;
    int_32 tl;
    const char *fname;
    unsigned char t1;

    if (data->options == co_have_end_of_archive)
    {
        return;
    }

    while (pos < len)
    {
        rem = (len - pos);

        if (data->remaining_file_data == 0)
        {
            if (rem < 26)
            {
                /* not enough remaining data to fill a complete header */
                break;
            }

            ts = *((int_16 *)(io->buffer + pos));

            if (ts == 0x71c7)
            {
                /* native header */

                /* not using most fields, but still pointing out which field
                 * would be where. */

                /* ts = *((int_16 *)(io->buffer + pos + 2));  */ /* dev */
                /* ts = *((int_16 *)(io->buffer + pos + 4));  */ /* ino */
                /* ts = *((int_16 *)(io->buffer + pos + 6));  */ /* mode */
                /* ts = *((int_16 *)(io->buffer + pos + 8));  */ /* uid */
                /* ts = *((int_16 *)(io->buffer + pos + 10)); */ /* gid */
                /* ts = *((int_16 *)(io->buffer + pos + 12)); */ /* nlink */
                /* ts = *((int_16 *)(io->buffer + pos + 14)); */ /* rdev */
                /* tl = *((int_32 *)(io->buffer + pos + 16)); */ /* mtime */
                ts = *((int_16 *)(io->buffer + pos + 20)); /* nsize */
                tl = ((*((int_16 *)(io->buffer + pos + 22))) << 16) |
                      (*((int_16 *)(io->buffer + pos + 24))); /* fsize */
            }
            else if (ts == 0xc771)
            {
                /* header with switched endinanness */
                t1 = *(io->buffer + pos);

                if (t1 == 0x71) /* non-native and big-endian */
                {
                    ts = ((*(io->buffer + pos + 20)) << 8) |
                          (*(io->buffer + pos + 21));
                    tl = ((*(io->buffer + pos + 22)) << 24) |
                         ((*(io->buffer + pos + 23)) << 16) |
                         ((*(io->buffer + pos + 24)) << 8) |
                          (*(io->buffer + pos + 25));
                }
                else /* non-native and little endian */
                {
                    ts = ((*(io->buffer + pos + 21)) << 8) |
                          (*(io->buffer + pos + 20));
                    /* this is a bit counter-intuitive, since the values are
                     * actually stored as two 16-bit integers with the first
                     * being the most-significant 16 bits. */
                    tl = ((*(io->buffer + pos + 23)) << 24) |
                         ((*(io->buffer + pos + 22)) << 16) |
                         ((*(io->buffer + pos + 25)) << 8) |
                          (*(io->buffer + pos + 24));
                }
            }
            else
            {
                /* invalid archive data, gotta bail */
                break;
            }
            
            ts += ((ts % 2) == 1) ? 1 : 0;

            if (rem < (26 + ts))
            {
                /* need to have enough data to at least read the whole
                 * header including the file name */
                break;
            }

            pos += 26;

            if ((*(io->buffer + pos + ts) == 0) ||
                (*(io->buffer + pos + ts - 1) == 0))
            {
                fname = (const char *)(io->buffer + pos);
            }
            else
            {
                fname = "invalid";
            }

            if ((ts >= 0xb) &&
                (fname[0] == 'T') && (fname[1] == 'R') && (fname[2] == 'A') &&
                (fname[3] == 'I') && (fname[4] == 'L') && (fname[5] == 'E') &&
                (fname[6] == 'R') && (fname[7] == '!') && (fname[8] == '!') &&
                (fname[9] == '!') && (fname[10] == 0))
            {
                if (data->on_end_of_archive != (void (*)(void *))0)
                {
                    data->on_end_of_archive (data->aux);
                }

                data->options = co_have_end_of_archive;
                io->position = pos;
                return;
            }

            pos += ts;
            rem = len - pos;

            if (truep (rx_match (data->regex, fname)))
            {
                if (rem >= tl) /* file data has been completely read in */
                {
                    data->on_new_file
                        (io_open_buffer (io->buffer + pos, tl), fname,
                         data->aux);
                    pos += tl + (((tl % 2) == 1) ? 1 : 0);
                }
                else
                {
                    data->options = ((tl % 2) == 1) ? co_file_is_padded
                                                    : co_none;

                    data->current_file = io_open_special ();
                    data->remaining_file_data = tl - rem;

                    io_write (data->current_file, io->buffer + pos, rem);

                    data->on_new_file
                        (data->current_file, fname, data->aux);

                    pos += rem;
                }
            }
            else
            {
                if (rem >= tl)
                {
                    pos += tl + (((tl % 2) == 1) ? 1 : 0);
                }
                else
                {
                    data->options = ((tl % 2) == 1) ? co_file_is_padded
                                                    : co_none;
                    pos += rem;
                }
            }
        }
        else
        {
            if (rem >= data->remaining_file_data)
            {
                if (data->current_file != (struct io *)0)
                {
                    io_write (data->current_file, io->buffer + pos,
                              data->remaining_file_data);
                    data->current_file->status = io_end_of_file;
                }

                pos += data->remaining_file_data;
                data->remaining_file_data = 0;

                if (data->options == co_file_is_padded)
                {
                    pos += 1;
                }

                data->current_file = (struct io *)0;
            }
            else
            {
                if (data->current_file != (struct io *)0)
                {
                    io_write (data->current_file, io->buffer + pos, rem);
                }

                data->remaining_file_data -= rem;
                pos += rem;
            }
        }
    }

    io->position = pos;

    if (((io->status == io_end_of_file) ||
         (io->status == io_unrecoverable_error)) &&
        (data->on_end_of_archive != (void (*)(void *))0))
    {
        data->on_end_of_archive (data->aux);
    }
}

static void cpio_closing
    (struct io *io, void *aux)
{
    struct cpio_read_data *data = (struct cpio_read_data *)aux;

    free_pool_mem ((void *)data);
}

void cpio_read_archive
    ( struct io *io, const char *regex,
      void (*on_new_file) (struct io *io, const char *name, void *aux),
      void (*on_end_of_archive) (void *aux),
      void *aux )
{
    if (io->type == iot_buffer)
    {
        struct cpio_read_data data =
            { co_none, on_new_file, on_end_of_archive, aux, 0,
              (struct io *)0, rx_compile (regex) };

        cpio_process_fragment (io, (void *)&data);
    }
    else
    {
        struct memory_pool pool =
            MEMORY_POOL_INITIALISER (sizeof (struct cpio_read_data));
        struct cpio_read_data *data = get_pool_mem (&pool);

        data->options             = co_none;
        data->on_new_file         = on_new_file;
        data->on_end_of_archive   = on_end_of_archive;
        data->aux                 = aux;
        data->remaining_file_data = 0;
        data->current_file        = (struct io *)0;
        data->regex               = rx_compile (regex);

        multiplex_add_io
            (io, cpio_process_fragment, cpio_closing, (void *)data);
    }
}

struct cpio *cpio_create_archive
    ( struct io *out )
{
#warning cpio_create_archive(): not implemented
}

void cpio_next_file
    ( struct cpio *cpio, const char *filename, struct io *file )
{
#warning cpio_next_file(): not implemented
}

void cpio_close
    ( struct cpio *cpio )
{
#warning cpio_close(): not implemented
}

