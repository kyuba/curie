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

#include <curie/multiplex.h>
#include <curie/memory.h>
#include <curie/int.h>
#include <curie/regex.h>
#include <sievert/cpio.h>
#include <sievert/time.h>

enum cpio_options
{
    co_none,
    co_file_is_padded,
    co_have_end_of_archive
};

struct cpio
{
    struct io *output;
    struct io *current_file;
    const char *current_file_name;
    int_16 device;
    int_16 inode;
    int_16 mode;
    int_16 uid;
    int_16 gid;
    int_16 links;
    int_16 device_id;
    int_32 mtime;
};

struct cpio_read_data
{
    enum cpio_options options;
    void (*on_new_file) (struct io *io, const char *name,
                         struct metadata *metadata, void *aux);
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
    int pos = io->position, len = io->length, rem, i;
    int_16 ts, *header;
    int_32 tl;
    const char *fname;
    unsigned char header_copy [26];

    /* file metadata structure; we're pre-allocating this since it's the same
     * for each header and creating this on the stack shouldn't take too long
     * either. */
    struct metadata_datetime datetime[1] =
        { { mdp_last_modification, { 0, 0 } } };
    struct metadata_acl acl[3] =
        { { mct_owning_user,  (const char *)0, 0 },
          { mct_owning_group, (const char *)0, 0 },
          { mct_default,      (const char *)0, 0 } };
    struct metadata_classification_unix_type classification_unix =
        { mdt_unix, mcu_unknown };
    struct metadata_attribute_integer attribute_source_device =
        { mat_source_device_id, 0 };
    struct metadata_attribute_integer attribute_inode =
        { mat_inode, 0 };
    struct metadata_attribute_integer attribute_flags =
        { mat_flags, 0 };
    struct metadata_attribute_integer attribute_user_id =
        { mat_user_id, 0 };
    struct metadata_attribute_integer attribute_group_id =
        { mat_group_id, 0 };
    struct metadata_attribute_integer attribute_link_count =
        { mat_link_count, 0 };
    struct metadata_attribute_integer attribute_device =
        { mat_link_device_id, 0 };
    /* note: the double-cast is to kill gcc's type-punning/strict-aliasing
     *       warning. */
    struct metadata_classification *classification[2] =
        { (struct metadata_classification *)((void *)&classification_unix),
          (struct metadata_classification *)0 };
    struct metadata_attribute *attributes[8] =
        { (struct metadata_attribute *)((void *)&attribute_source_device),
          (struct metadata_attribute *)((void *)&attribute_inode),
          (struct metadata_attribute *)((void *)&attribute_flags),
          (struct metadata_attribute *)((void *)&attribute_user_id),
          (struct metadata_attribute *)((void *)&attribute_group_id),
          (struct metadata_attribute *)((void *)&attribute_link_count),
          (struct metadata_attribute *)((void *)&attribute_device),
          (struct metadata_attribute *)0 };
    struct metadata metadata =
        { 1, 0, 3,
          datetime,
          (struct metadata_relation *)0,
          acl,
          classification,
          attributes,
          (struct metadata_signature **)0 };

    if (data->options == co_have_end_of_archive)
    {
        return;
    }

    while (pos < len)
    {
        rem = (len - pos);

        if (data->remaining_file_data == 0)
        {
            header = (int_16 *)(io->buffer + pos);

            if (rem < 26)
            {
                /* not enough remaining data to fill a complete header */
                break;
            }

            ts = header[0];

            if (ts == 0xc771)
            {
                /* header with switched endianness */

                for (i = 0; i < 26; i += 2)
                {
                    /* copy the header with switched endianness*/

                    header_copy [i] =
                        *((unsigned char *)(io->buffer + pos + i + 1));
                    header_copy [(i+1)] =
                        *((unsigned char *)(io->buffer + pos + i));
                }

                header = (int_16 *)header_copy;
                ts = header[0];
            }

            if (ts == 0x71c7)
            {
                /* native header */

                attribute_source_device.integer = header[1];
                attribute_inode.integer         = header[2];

                ts = header[3];

                switch (ts & 0xf000) /* file type mask */
                {
                    case 0xc000:
                        classification_unix.classification=mcu_socket;
                        break;
                    case 0xa000:
                        classification_unix.classification=mcu_symbolic_link;
                        break;
                    case 0x8000:
                        classification_unix.classification=mcu_file;
                        break;
                    case 0x6000:
                        classification_unix.classification=mcu_block_device;
                        break;
                    case 0x4000:
                        classification_unix.classification=mcu_directory;
                        break;
                    case 0x2000:
                        classification_unix.classification=mcu_character_device;
                        break;
                    case 0x1000:
                        classification_unix.classification=mcu_fifo;
                        break;
                    default:
                        classification_unix.classification=mcu_unknown;
                        break;
                }

                attribute_flags.integer = ((ts & 0x800) ? MAT_SET_UID : 0)
                                        | ((ts & 0x400) ? MAT_SET_GID : 0)
                                        | ((ts & 0x200) ? MAT_STICKY  : 0);

                acl[0].access = MCT_SET | ((ts & 0100) ? MCT_EXECUTE : 0)
                                        | ((ts & 0200) ? MCT_WRITE   : 0)
                                        | ((ts & 0400) ? MCT_READ    : 0);

                acl[1].access = MCT_SET | ((ts & 0010) ? MCT_EXECUTE : 0)
                                        | ((ts & 0020) ? MCT_WRITE   : 0)
                                        | ((ts & 0040) ? MCT_READ    : 0);

                acl[2].access = MCT_SET | ((ts & 0001) ? MCT_EXECUTE : 0)
                                        | ((ts & 0002) ? MCT_WRITE   : 0)
                                        | ((ts & 0004) ? MCT_READ    : 0);

                attribute_user_id.integer       = header[4];
                attribute_group_id.integer      = header[5];
                attribute_link_count.integer    = header[6];
                attribute_device.integer        = header[7];

                datetime[0].datetime =dt_from_unix
                    (((header[8]) << 16) | (header[9]));

                ts = header[10]; /* nsize */
                tl = ((header[11]) << 16) | (header[12]); /* fsize */
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
                         &metadata, data->aux);
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
                        (data->current_file, fname, &metadata, data->aux);

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
    cpio_process_fragment (io, aux);

    free_pool_mem (aux);
}

void cpio_read_archive
    ( struct io *io, const char *regex,
      void (*on_new_file) (struct io *io, const char *name,
                           struct metadata *metadata, void *aux),
      void (*on_end_of_archive) (void *aux),
      void *aux )
{
    if (io->type == iot_buffer)
    {
        struct cpio_read_data data =
            { co_none, on_new_file, on_end_of_archive, aux, 0,
              (struct io *)0, rx_compile (regex) };

        cpio_process_fragment (io, (void *)&data);

        io_close (io);
    }
    else
    {
        static struct memory_pool pool =
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
    static struct memory_pool pool =
        MEMORY_POOL_INITIALISER (sizeof (struct cpio));
    struct cpio *cpio = get_pool_mem (&pool);

    cpio->output            = out;
    cpio->current_file      = (struct io *)0;
    cpio->current_file_name = (const char *)0;
    cpio->inode             = 0;

    return cpio;
}

static void reset_cpio (struct cpio *cpio, struct metadata *metadata)
{
    cpio->device    = 1;
    cpio->mode      = 0100644;
    cpio->uid       = 0;
    cpio->gid       = 0;
    cpio->links     = 1;
    cpio->device_id = 0;
    cpio->mtime     = 0;

    if (metadata)
    {
        enum metadata_classification_unix classification = mcu_file;
        int uid = 0, gid = 0, mode = 0100644, device = 1, attributes = 0;
        long atime, mtime, ctime, size;

        metadata_to_unix
            (metadata, &classification, &uid, &gid, &mode, &atime,
             &mtime, &ctime, &size, &device, &attributes);

        cpio->mode   = mode & 0xf000;
        cpio->mtime  = mtime;
        cpio->uid    = uid;
        cpio->gid    = gid;
        cpio->device = device;

        cpio->mode   = (cpio->mode & 0xf000);

        switch (classification)
        {
            case mcu_socket:           cpio->mode |= 0xc000; break;
            case mcu_symbolic_link:    cpio->mode |= 0xa000; break;
            case mcu_file:             cpio->mode |= 0x8000; break;
            case mcu_block_device:     cpio->mode |= 0x6000; break;
            case mcu_directory:        cpio->mode |= 0x4000; break;
            case mcu_character_device: cpio->mode |= 0x2000; break;
            case mcu_fifo:             cpio->mode |= 0x1000; break;
            default:                   cpio->mode |= 0x8000; break;
        }

        if (metadata->attribute != (struct metadata_attribute **)0)
        {
            struct metadata_attribute **a = metadata->attribute;
            struct metadata_attribute_integer *ai;

            while ((*a) != (struct metadata_attribute *)0)
            {
                ai = (struct metadata_attribute_integer *)(*a);

                switch ((*a)->type)
                {
                    case mat_inode:
                        cpio->inode = ai->integer;
                        break;
                    case mat_link_count:
                        cpio->links = ai->integer;
                        break;
                    case mat_link_device_id:
                        cpio->device_id = ai->integer;
                        break;
                    default:
                        break;
                }

                a++;
            }
        }
    }
}

static void cpio_write_file
    (struct cpio *cpio, char *b, int size, const char *fname)
{
    struct io *out = cpio->output;
    int_16 ts = 0x71c7, ts2;
    int_32 tl = size;

    io_collect (out, (char *)&ts, 2); /* magic */

    io_collect (out, (char *)&(cpio->device), 2);
    io_collect (out, (char *)&(cpio->inode), 2);
    io_collect (out, (char *)&(cpio->mode), 2);
    io_collect (out, (char *)&(cpio->uid), 2);
    io_collect (out, (char *)&(cpio->gid), 2);
    io_collect (out, (char *)&(cpio->links), 2);
    io_collect (out, (char *)&(cpio->device_id), 2);
    ts = (cpio->mtime & 0xffff0000) >> 16;
    io_collect (out, (char *)&ts, 2);
    ts = (cpio->mtime & 0xffff);
    io_collect (out, (char *)&ts, 2);

    for (ts = 0; fname[ts]; ts++);
    ts++;

    io_collect (out, (char *)&ts, 2); /* namesize */

    ts2 = (tl & 0xffff0000) >> 16;
    io_collect (out, (char *)&ts2, 2); /* filesize[0] */

    ts2 = (tl & 0x0000ffff);
    io_collect (out, (char *)&ts2, 2); /* filesize[1] */

    io_collect (out, (char *)fname, ts);

    if ((ts % 2) == 1)
    {
        io_collect (out, (char *)"", 1);
    }

    if (b != (char *)0)
    {
        io_write (out, (char *)b, size);

        if ((size % 2) == 1)
        {
            io_collect (out, (char *)"", 1);
        }
    }
    
    cpio->inode++;
}

static void cpio_complete_last_file
    (struct cpio *cpio)
{
    if (cpio->current_file)
    {
        struct io *io = cpio->current_file;

        cpio_write_file
            (cpio, io->buffer, io->length, cpio->current_file_name);

        io_close (io);

        cpio->current_file      = (struct io *)0;
        cpio->current_file_name = (const char *)0;
    }
}

void cpio_next_file
    ( struct cpio *cpio, const char *filename, struct metadata *metadata,
      struct io *file )
{
    cpio_complete_last_file (cpio);

    reset_cpio (cpio, metadata);

    if (file->type == iot_buffer)
    {
        cpio_write_file
            (cpio, file->buffer, file->length, filename);

        io_close (file);
    }
    else
    {
        cpio->current_file      = file;
        cpio->current_file_name = filename;
    }
}

void cpio_close
    ( struct cpio *cpio )
{
    cpio_complete_last_file (cpio);
    reset_cpio (cpio, (struct metadata *)0);
    cpio_write_file (cpio, (char *)0, 0, "TRAILER!!!");

    io_close (cpio->output);
}

