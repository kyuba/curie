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

/*! \file
 *  \brief File Metadata Header
 *
 *  This file describes a general structure for file metadata, which can be used
 *  by system-specific code or archive readers/writers to describe metadata for
 *  files such as ownership, access rights, modification or access times and
 *  similar attributes.
 *
 *  Turns out it's rather complex if you keep it system-agnostic. It should
 *  still be fairly easy to use, there's not too much to iterate over and most
 *  of the time you'll just be interested in one or two bits of information
 *  anyway.
 */

#ifndef LIBSIEVERT_METADATA_H
#define LIBSIEVERT_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/time.h>
#include <curie/sexpr.h>

#define MCT_ADDITIVE          (1 << 0x0)
#define MCT_SUBTRACTIVE       (1 << 0x1)
#define MCT_SET               (1 << 0x2)
#define MCT_LIMIT             (1 << 0x3)
#define MCT_READ              (1 << 0x5)
#define MCT_WRITE             (1 << 0x6)
#define MCT_EXECUTE           (1 << 0x7)
#define MCT_SEARCH            (1 << 0x8)
#define MCT_CREATE            (1 << 0x9)

#define MAT_SET_UID           (1 << 0x0)
#define MAT_SET_GID           (1 << 0x1)
#define MAT_STICKY            (1 << 0x2)
#define MAT_MAY_ARCHIVE       (1 << 0x3)
#define MAT_READONLY          (1 << 0x4)
#define MAT_HIDDEN            (1 << 0x5)
#define MAT_ENCRYPTED         (1 << 0x6)

enum metadata_classification_type
{
    mdt_unknown,
    mdt_mime,
    mdt_unix
};

enum metadata_classification_unix
{
    mcu_unknown,
    mcu_file,
    mcu_directory,
    mcu_fifo,
    mcu_pipe,
    mcu_socket,
    mcu_symbolic_link,
    mcu_block_device,
    mcu_character_device
};

enum metadata_datetime_purpose
{
    mdp_unknown,
    mdp_creation,
    mdp_link_creation,
    mdp_last_access,
    mdp_last_modification,
    mdp_last_status_change,
    mdp_expiry
};

enum metadata_relation_type
{
    mrt_unknown,
    mrt_owning_user,
    mrt_owning_group,
    mrt_creating_user,
    mrt_creating_group
};

enum metadata_acl_target_type
{
    mct_unknown,
    mct_creating_user,
    mct_creating_group,
    mct_owning_user,
    mct_owning_group,
    mct_user,
    mct_group,
    mct_default
};

enum metadata_attribute_type
{
    mat_unknown,
    mat_key_value,
    mat_description,
    mat_link_target,
    mat_link_device_id,
    mat_source_device_id,
    mat_inode,
    mat_link_count,
    mat_size,
    mat_user_id,
    mat_group_id,
    mat_flags
};

enum metadata_signature_type
{
    mst_unknown,
    mst_hash_md4,
    mst_hash_md5,
    mst_hash_murmur,
    mst_hash_sha1,
    mst_hash_sha256,
    mst_hash_crc8,
    mst_hash_crc16,
    mst_hash_crc32
};

struct metadata_classification
{
    enum metadata_classification_type type;
};

struct metadata_classification_mime
{
    enum metadata_classification_type type;
    const char *mime;
};

struct metadata_classification_unix_type
{
    enum metadata_classification_type type;
    enum metadata_classification_unix classification;
};

struct metadata_datetime
{
    enum metadata_datetime_purpose purpose;
    struct datetime datetime;
};

struct metadata_relation
{
    enum metadata_relation_type type;
    const char *target;
};

struct metadata_acl
{
    enum metadata_acl_target_type target_type;
    const char *target;
    int access;
};

struct metadata_attribute
{
    enum metadata_attribute_type type;
};

struct metadata_attribute_key_value
{
    enum metadata_attribute_type type;
    const char *key;
    const char *value;
};

struct metadata_attribute_string
{
    enum metadata_attribute_type type;
    const char *string;
};

struct metadata_attribute_integer
{
    enum metadata_attribute_type type;
    long long integer;
};

struct metadata_signature
{
    enum metadata_signature_type type;
};

/*! \brief File Metadata Structure
 *
 *  This structure is used to represent file attributes such as ownership,
 *  access rights, modification and access records, etc. The structure is
 *  designed to be able to represent as many different systems and situations as
 *  possible.
 */
struct metadata
{
    int datetime_count;
    int relation_count;
    int acl_count;
    struct metadata_datetime       *datetime;
    struct metadata_relation       *relation;
    struct metadata_acl            *acl;
    struct metadata_classification **classification;
    struct metadata_attribute      **attribute;
    struct metadata_signature      **signature;
};

struct metadata *metadata_from_unix
    (enum metadata_classification_unix classification,
     int uid, int gid, int mode, int atime, int mtime, int ctime,
     int attributes);

struct metadata *metadata_from_sexpr
    (sexpr data);

struct metadata *metadata_from_path
    (const char *path);

void metadata_from_path_closure
    (const char *path, void (*with_metadata) (struct metadata *, void *aux),
     void *aux);

void metadata_to_unix
    (struct metadata *metadata,
     enum metadata_classification_unix *classification,
     int *uid, int *gid, int *mode, int *atime, int *mtime, int *ctime,
     int *attributes);

sexpr metadata_to_sexpr
    (struct metadata *metadata);

void metadata_apply_to_path
    (struct metadata *metadata,
     const char *path);

void metadata_destroy
    (struct metadata *metadata);

#ifdef __cplusplus
}
#endif

#endif
