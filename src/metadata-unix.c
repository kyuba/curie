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
#include <sievert/time.h>
#include <sievert/metadata.h>

void metadata_from_unix
    (enum metadata_classification_unix classification,
     int uid, int gid, int mode, long atime, long mtime, long ctime,
     long size, int device, int attributes,
     void (*with_metadata) (struct metadata *, void *), void *aux)
{
    struct metadata_datetime datetime[3] =
        { { mdp_last_access,        { 0, 0 } },
          { mdp_last_modification,  { 0, 0 } },
          { mdp_last_status_change, { 0, 0 } } };
    struct metadata_acl acl[3] =
        { { mct_owning_user,  (const char *)0, 0 },
          { mct_owning_group, (const char *)0, 0 },
          { mct_default,      (const char *)0, 0 } };
    struct metadata_classification_unix_type classification_unix =
        { mdt_unix, classification };
    struct metadata_attribute_integer attribute_flags =
        { mat_flags, attributes };
    struct metadata_attribute_integer attribute_user_id =
        { mat_user_id, uid };
    struct metadata_attribute_integer attribute_group_id =
        { mat_group_id, gid };
    struct metadata_attribute_integer attribute_device =
        { mat_link_device_id, device };
    /* note: the double-cast is to kill gcc's type-punning/strict-aliasing
     *       warning. */
    struct metadata_classification *classification_a[2] =
        { (struct metadata_classification *)((void *)&classification_unix),
          (struct metadata_classification *)0 };
    struct metadata_attribute *attributes_a[5] =
        { (struct metadata_attribute *)((void *)&attribute_flags),
          (struct metadata_attribute *)((void *)&attribute_user_id),
          (struct metadata_attribute *)((void *)&attribute_group_id),
          (struct metadata_attribute *)((void *)&attribute_device),
          (struct metadata_attribute *)0 };
    struct metadata metadata =
        { 3, 0, 3,
          datetime,
          (struct metadata_relation *)0,
          acl,
          classification_a,
          attributes_a,
          (struct metadata_signature **)0 };

    dt_from_unix (&(datetime[0].datetime), atime);
    dt_from_unix (&(datetime[1].datetime), mtime);
    dt_from_unix (&(datetime[2].datetime), ctime);

    acl[0].access = MCT_SET | ((mode & 0100) ? MCT_EXECUTE : 0)
                            | ((mode & 0200) ? MCT_WRITE   : 0)
                            | ((mode & 0400) ? MCT_READ    : 0);

    acl[1].access = MCT_SET | ((mode & 0010) ? MCT_EXECUTE : 0)
                            | ((mode & 0020) ? MCT_WRITE   : 0)
                            | ((mode & 0040) ? MCT_READ    : 0);

    acl[2].access = MCT_SET | ((mode & 0001) ? MCT_EXECUTE : 0)
                            | ((mode & 0002) ? MCT_WRITE   : 0)
                            | ((mode & 0004) ? MCT_READ    : 0);

    with_metadata (&metadata, aux);
}

void metadata_to_unix
    (struct metadata *metadata,
     enum metadata_classification_unix *classification,
     int *uid, int *gid, int *mode, long *atime, long *mtime, long *ctime,
     long *size, int *device, int *attributes)
{
#pragma message("metadata_to_unix() incomplete")
}
