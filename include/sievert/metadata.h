/**\file
 * \brief File Metadata Header
 *
 * This file describes a general structure for file metadata, which can be used
 * by system-specific code or archive readers/writers to describe metadata for
 * files such as ownership, access rights, modification or access times and
 * similar attributes.
 *
 * Turns out it's rather complex if you keep it system-agnostic. It should
 * still be fairly easy to use, there's not too much to iterate over and most
 * of the time you'll just be interested in one or two bits of information
 * anyway.
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
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

/**\brief Metadata signature types
 *
 * Enumerates the possible signature types that a file could have.
 */
enum metadata_signature_type
{
    mst_unknown,     /**< Unknown signature; should not be used. */
    mst_hash_md4,    /**< MD4 hash */
    mst_hash_md5,    /**< MD5 hash */
    mst_hash_murmur, /**< Murmur hash */
    mst_hash_sha1,   /**< SHA1 hash */
    mst_hash_sha256, /**< SHA256 hash */
    mst_hash_crc8,   /**< CRC8 hash */
    mst_hash_crc16,  /**< CRC16 hash */
    mst_hash_crc32   /**< CRC32 hash */
};

/**\brief Metadata base struct: classification
 *
 * Contains the classification type; a pointer to a struct of this type would
 * then need to be cast to the appropriate type for the classification to be
 * used.
 */
struct metadata_classification
{
    /**\brief Classification type
     *
     * The type of classification contained in the struct; use the information
     * in this field to determine what to cast a metadata_classification struct
     * to.
     */
    enum metadata_classification_type type;
};

/**\brief File MIME type
 *
 * This is a classification type that contains a MIME type; MIME types are very
 * common these days to describe the actual content format of a file.
 */
struct metadata_classification_mime
{
    /**\copydoc metadata_classification::type */
    enum metadata_classification_type type;

    /**\brief MIME type
     *
     * The actual MIME type of a file.
     */
    const char *mime;
};

/**\brief Generic Unix file classification
 *
 * Contains a Unix-y classification of a file into different filesystem entry
 * types: file, directory, pipe, socket, etc.
 */
struct metadata_classification_unix_type
{
    /**\copydoc metadata_classification::type */
    enum metadata_classification_type type;

    /**\brief Unix file classification
     *
     * The file's classification.
     */
    enum metadata_classification_unix classification;
};

/**\brief Date/time information
 *
 * Contains a time stamp and the relation of that time stamp to the decribed
 * file. No need to cast pointers to this to anything else.
 */
struct metadata_datetime
{
    /**\brief Time stamp relation
     *
     * Determines the kind of relation that a time stamp has to the described
     * file, e.g. the time stamp could be an atime/mtime/ctime record.
     */
    enum metadata_datetime_purpose purpose;

    /**\brief Date/time information
     *
     * The time stamp in question.
     */
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

/**\brief Metadata base struct: signature
 *
 * Contains the signature type; a pointer to a struct of this type would then
 * need to be cast to the appropriate type for the signature to be used.
 */
struct metadata_signature
{
    /**\brief Signature type
     *
     * The type of signature contained in the struct; use the information in
     * this field to determine what to cast a metadata_signature struct to.
     */
    enum metadata_signature_type type;
};

/**\brief File Metadata Structure
 *
 * This structure is used to represent file attributes such as ownership,
 * access rights, modification and access records, etc. The structure is
 * designed to be able to represent as many different systems and situations as
 * possible.
 */
struct metadata
{
    /**\brief Number of datetime elements
     *
     * Contains the number of elements in the datetime array. Using elements
     * >= (datetime+datetime_count) will probably result in a segfault.
     */
    int datetime_count;

    /**\brief Number of relation elements
     *
     * Contains the number of elements in the relation array. Using elements
     * >= (relation+relation_count) will probably result in a segfault.
     */
    int relation_count;

    /**\brief Number of acl elements
     *
     * Contains the number of elements in the acl array. Using elements >=
     * (acl+acl_count) will probably result in a segfault.
     */
    int acl_count;

    /**\brief Date/Time information
     *
     * Contains datetime_count elements of information on date and time
     * attributes for a file, e.g. time of last access, modification and inode
     * changes.
     */
    struct metadata_datetime       *datetime;

    /**\brief Relation information
     *
     * Contains relation_count elements of information on relations the file has
     * to other entities, such as users or groups.
     */
    struct metadata_relation       *relation;

    /**\brief Access control information
     *
     * Contains acl_count elements of information on who may or may not access
     * the file in which way.
     */
    struct metadata_acl            *acl;

    /**\brief File classifications
     *
     * A list of file classifications. May be 0, in which case the file has not
     * been classified. Otherwise this points to an array of classifications
     * which end when an element pointing to 0 is encountered.
     */
    struct metadata_classification **classification;

    /**\brief File attributes
     *
     * A list of file attributes. May be 0, in which case the file does not have
     * any additional attributes. Otherwise this points to an array of
     * attributes which end when an element pointing to 0 is encountered.
     */
    struct metadata_attribute      **attribute;

    /**\brief File signatures
     *
     * A list of cryptographic signatures of the file. May be 0, in which case
     * there are no hashes or signatures available for the file contents.
     * Otherwise this points to an array of signatures which end when an element
     * pointing to 0 is encountered.
     */
    struct metadata_signature      **signature;
};

/**\brief Call metadata function with Unix attributes
 *
 * Calls the with_metadata callback with a metadata object constructed from
 * standard UNIX attributes. The struct metadata is created on the function's
 * stack, so using it after the with_metadata callback returns will most likely
 * result in an error.
 *
 * This function is primarily intended as a helper for other library functions
 * which would like to provide a metadata callback and it would be convenient to
 * use common UNIX metadata.
 *
 * \param[in] classification File classification. 
 * \param[in] uid            File owner's user ID.
 * \param[in] gid            File owner's group ID.
 * \param[in] mode           File mode.
 * \param[in] atime          Time of last file access.
 * \param[in] mtime          Time of last file modification.
 * \param[in] ctime          Time of last inode modification.
 * \param[in] size           File size.
 * \param[in] device         File device.
 * \param[in] attributes     File attributes.
 * \param[in] with_metadata  Function to call after creating the metadata.
 * \param[in] aux            Auxiliary data; passed to with_metadata.
 */
void metadata_from_unix
    (enum metadata_classification_unix classification,
     int uid, int gid, int mode, long atime, long mtime, long ctime,
     long size, int device, int attributes,
     void (*with_metadata) (struct metadata *, void *), void *aux);

/**\brief Call metadata function metadata from path
 *
 * Calls the with_metadata callback after successfully figuring out any common
 * information about a file that the filesystem provides. On UNIX, for example,
 * this will call a variant of the stat() function and use the
 * metadata_from_unix() function to call with_metadata.
 *
 * \param[in] path           The path for which you want to gather metadata.
 * \param[in] with_metadata  Function to call after creating the metadata.
 * \param[in] aux            Auxiliary data; passed to with_metadata.
 */
void metadata_from_path
    (const char *path,
     void (*with_metadata) (struct metadata *, void *), void *aux);

/**\brief Convert metadata to Unix attributes
 *
 * The inverse of metadata_from_unix(), this function will use a metadata struct
 * and turn the contained information into something that roughly resembles UNIX
 * file metadata.
 *
 * \param[out] classification File classification. 
 * \param[out] uid            File owner's user ID.
 * \param[out] gid            File owner's group ID.
 * \param[out] mode           File mode.
 * \param[out] atime          Time of last file access.
 * \param[out] mtime          Time of last file modification.
 * \param[out] ctime          Time of last inode modification.
 * \param[out] size           File size.
 * \param[out] device         File device.
 * \param[out] attributes     File attributes.
 */
void metadata_to_unix
    (struct metadata *metadata,
     enum metadata_classification_unix *classification,
     int *uid, int *gid, int *mode, long *atime, long *mtime, long *ctime,
     long *size, int *device, int *attributes);

/**\brief Set file metadata
 *
 * Uses the metadata in the first argument to update the metadata of the file
 * referenced in the second parameter. The metadata that is applied depends on
 * the filesystem. Certain things, like the file size or signatures, are never
 * applied, although signatures could be set as extended attributes.
 *
 * \param[in] metadata The metadata to set for the given file.
 * \param[in] path     Path to the file to update.
 */
void metadata_to_path
    (struct metadata *metadata,
     const char *path);

#ifdef __cplusplus
}
#endif

#endif
