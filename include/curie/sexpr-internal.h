/**\file
 * \brief S-expressions (internal)
 * \internal
 *
 * Contains internal definitions used in the S-expression code.
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

#ifndef LIBCURIE_SEXPR_INTERNAL_H
#define LIBCURIE_SEXPR_INTERNAL_H

#include <curie/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**\brief S-Expression I/O Structure
 *
 * Programmes don't need to know how this one looks on the inside. This is just
 * a wrapper for an input and an output I/O structure for use with sx_read(),
 * sx_write(), etc.
 */
struct sexpr_io {
    struct io *in;  /**< Input Structure */
    struct io *out; /**< Output Structure */
};

/**\brief Custom type descriptor
 *
 * Contains function pointers to handle a custom type and a unicode code point
 * to identify instances of the custom type in a stream. Custom types have to be
 * serialised as (type ...) to be processed correctly when reading them back in.
 */
struct sexpr_type_descriptor
{
    /**\brief Unicode code point to identify type
     *
     * All custom sexpr types are identified with a Unicode code point, which is
     * used when serialising or unserialising data to identify the type that was
     * used internally. This member variable holds that code point.
     */
    unsigned int                   type;

    /**\brief Serialise S-expression
     *
     * Turns the provided instance of a custom S-expression type into a basic
     * S-expression so that it can be written to a stream and then parsed in
     * later.
     */
    sexpr                        (*serialise)  (sexpr);

    /**\brief Unserialise S-expression
     *
     * The opposite of the serialise() function, this function is used to turn a
     * previously serialised value back into an instance of a custom type.
     */
    sexpr                        (*unserialise)(sexpr);

    /**\brief Invoke gc_tag()
     *
     * This function is called so that a custom sexpr type can invoke gc_tag()
     * on any resources used by the given sexpr. For example the core cons type
     * will do quite the same by invoking gc_tag on the car and cdr portions of
     * a cons sexpr.
     */
    void                         (*tag)        (sexpr);

    /**\brief Destructor
     *
     * Called to release any additional resources associated with an
     * S-expression when it's being freed from memory.
     */
    void                         (*destroy)    (sexpr);

    /**\brief Invoke gc_call()
     *
     * This function is supposed to invoke gc_call() on all active instances of
     * the custom S-expression type. Providing this function is technically
     * optional and may not be necessary, e.g. when you don't intend for
     * instances to be pruned by the garbage collector.
     */
    void                         (*call)       ();

    /**\brief Equality comparison
     *
     * Called to determine if two S-expressions of a custom type are supposed to
     * compare as equal.
     */
    sexpr                        (*equalp)     (sexpr, sexpr);

    /**\brief Next type descriptor
     *
     * It is assumed that there will not be very many custom types in use at any
     * point in time in a Curie application, so the custom types are kept in a
     * linked list. This pointer points to the next custom type descriptor in
     * this list.
     */
    struct sexpr_type_descriptor  *next;
};

/**\brief Invoke gc_call() on core-type sexprs
 *
 * Calling this function will invoke the gc_call() function on all sexprs of a
 * core type - e.g. strings or symbols. This is used internally in the garbage
 * collection phase.
 */
void sx_call_all ( void );

/**\brief Invoke gc_call() on custom sexprs
 *
 * Calling this function will invoke the call() function of custom sexpr types,
 * which should invoke gc_call() on all instances of custom sexpr types.
 */
void sx_call_custom ( void );

/**\brief Query type descriptor
 *
 * Used internally to obtain the struct sexpr_type_descriptor of a custom sexpr
 * type. Type must match the Unicode code point that was used to register the
 * type.
 *
 * \param[in] type The Unicode code point with which the type was registered.
 *
 * \returns Type descriptor of the 'type' parameter, if it exists.
 */
struct sexpr_type_descriptor *sx_get_descriptor (unsigned int type);

#ifdef __cplusplus
}
#endif

#endif
