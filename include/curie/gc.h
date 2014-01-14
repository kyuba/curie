/**\file
 * \brief Garbage Collector
 *
 * We're trying to implement a rather conservative GC here, all it does is walk
 * from the stack start address to the current end of it and mark available
 * objects based on the pointers it sees (assuming all pointers are aligned to
 * their size, which is a fair assumption).
 *
 * Note that the gc is never invoked automatically, it needs to be run
 * manually. This should prevent most problems due to optimisations, and if for
 * some reason you know that the GC will screw things up, just don't call it.
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
 * \see Projct Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
 */

#ifndef LIBCURIE_GC_H
#define LIBCURIE_GC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curie/sexpr.h>

/**\brief Add Garbage Collector Root
 *
 * \param[in] sx The root to add.
 *
 * The garbage collector uses roots to allow programmers to designate
 * s-expressions that should not get killed by the garbage collector, even if
 * they're not on the stack anywhere.
 */
void          gc_add_root    (sexpr *sx);

/**\brief Remove Garbage Collector Root
 *
 * \param[in] sx The root to remove.
 *
 * Analoguous to gc_add_root(), but instead of adding a new root, it removes
 * an old one.
 */
void          gc_remove_root (sexpr *sx);

/**\brief "Tag" an S-Expression
 *
 * \param[in] sx The root to tag.
 *
 * "Tagging" with this garbage collector refers to marking areas as used. This
 * will also mark cars and cdrs for conses, and it will call the tag() function
 * for custom types.
 *
 * \note This function is only meaningful if called in the context of a gc run,
 *       which means you shouldn't call this function outside of a tag()
 *       function of a custom type.
 */
void          gc_tag         (sexpr sx);

/**\brief "Call" an S-Expression
 *
 * \param[in] sx The root to call.
 *
 * "Calling" is used to give the gc an s-expression to check. The gc expects
 * s-expression code to call their s-expressions so that the garbage collector
 * can check if they're in use. This is done automatically for built-in types,
 * and by the call() function of custom types.
 *
 * \note In your custom call() function, do not, ever, call the same
 *       s-expression twice. This will upset the gc gravely and may lead to
 *       data that is in use to be freed.
 *
 * \note This function is only meaningful if called in the context of a gc run,
 *       which means you shouldn't call this function outside of a call()
 *       function of a custom type.
 */
void          gc_call        (sexpr sx);

/**\brief Invoke the Garbage Collector
 *
 * As has been mentioned in the global notes for this header file, the garbage
 * collector needs to be called manually. Calling this function will do so.
 *
 * \returns The number of items that have been free'd.
 */
unsigned long gc_invoke      ();

/**\brief Garbage Collector Item Count Hint
 *
 * To make the garbage collector a bit more efficient, this variable is used to
 * hint the garbage collector to how many s-expressions are eligible for
 * garbage collection. This saves on some re-allocations.
 */
extern unsigned long gc_base_items;

#ifdef __cplusplus
}
#endif

#endif
