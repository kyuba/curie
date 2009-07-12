/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008, 2009, Kyuba Project Members
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

/*! \defgroup sexpr S-Expressions
 *  \brief Symbolic Expression Handling
 */

/*! \file
 *  \ingroup sexpr
 *  \brief S-expressions
 *
 *  Functions to parse, write and otherwise handle symbolic expressions, as used
 *  in lisp-like programming languages. The particular set of s-expressions that
 *  curie supports mimics those in scheme and derivatives.
 *
 *  Extensions, such as quotes, are not supported.
 */

#ifndef LIBCURIE_SEXPR_H
#define LIBCURIE_SEXPR_H

#include <curie/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief S-Expression
 *  \ingroup sexpr
 *
 *  Outside of the library, s-expressions should always be referenced by
 *  pointers. The library is nice enough to make sure never to return (sexpr)0
 *  pointers, so this ought to be alright.
 */
typedef int_pointer sexpr;

/*! \internal
 *  \defgroup sexprRepresentation In-Memory Representation
 *  \ingroup sexpr
 *  \brief In-Memory Representation of S-Expressions
 *
 *  These data structures define the runtime memory layout for s-expressions.
 *  You really shouldn't need to grok into this too much.
 *
 *  @{
 */

/*! \brief S-Expression Type
 *  \internal
 *
 *  All (Pointer-)S-expressions have a type, which is part of their in-memory
 *  representation.
 */
enum sx_type {
    sxt_string                  = 4,  /*!< String, i.e. "string" */
    sxt_symbol                  = 5,  /*!< Symbol, i.e. symbol */
    sxt_cons                    = 6,  /*!< Cons, i.e. (x . y) */
};

/*! \brief Cons S-Expression
 *  \internal
 *
 *  This structure is used for conses.
 */
struct sexpr_cons {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all sexprs.
     */
    enum sx_type type;

    /*! \brief First Cell
     *
     *  The first cell of the cons, or 'car' in lisp.
     */
    sexpr car;

    /*! \brief Second Cell
     *
     *  The second cell of the cons, or 'cdr' in lisp.
     */
    sexpr cdr;
};

/*! \brief String-/Symbol S-Expression
 *  \internal
 *
 *  This structure carries a string as its payload, and as such is used to
 *  represent strings and symbols.
 */
struct sexpr_string_or_symbol {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all sexprs.
     */
    enum sx_type type;

    /*! \brief String
     *
     *  The contained string itself. It's 0-terminated, too.
     */
    char character_data[];
};

struct sexpr_partial {
    unsigned int type;
};

/*! @} */

/*! \defgroup sexprIO In-/Output
 *  \ingroup sexpr
 *  \brief S-Expression In- and Output
 *
 *  These functions deal with that pesky in- and output that makes normal
 *  programmes so useful.
 *
 *  @{
 */

/*! \brief S-Expression I/O Context
 *
 *  This structure encapsulates two I/O structures that are used by s-expression
 *  functions for in- and output.
 */
struct sexpr_io;

/*! \brief Create S-Expression I/O Context
 *  \param[in] in  The io structure to use for input.
 *  \param[in] out The io structure to use for output.
 *  \return The new I/O context, or (struct sexpr_io *)0 if no memory could be
 *          allocated.
 *
 *  This function creates a new I/O context to use with s-expression functions.
 */
struct sexpr_io *sx_open_io
        (struct io *in, struct io *out);

/*! \brief Create Standard I/O as S-Expression I/O Context
 *  \return The new I/O context, or (struct sexpr_io *)0 if no memory could be
 *          allocated.
 *
 *  Analoguous to sx_open_io(), but it uses the standard file descriptors '0'
 *  and '1'.
 */
struct sexpr_io *sx_open_stdio ( void );

/*! \brief Create Standard I/O as S-Expression I/O Context
 *  \return The new I/O context, or (struct sexpr_io *)0 if no memory could be
 *          allocated.
 *
 *  Same as sx_open_stdio(), but only opens stdout.
 */
struct sexpr_io *sx_open_stdout ( void );

/*! \brief Create Standard I/O as S-Expression I/O Context
 *  \return The new I/O context, or (struct sexpr_io *)0 if no memory could be
 *          allocated.
 *
 *  Same as sx_open_stdio(), but only opens stdin.
 */
struct sexpr_io *sx_open_stdin ( void );

/*! \brief Close S-Expression I/O Context
 *  \param[in] io The context to close.
 *
 *  This function will destroy the given I/O context and close and deallocate
 *  all resources used by it, including any involved io structures and file
 *  descriptors. Using the structure after calling this function on it may kill
 *  your programmes or set your cat on fire.
 */
void sx_close_io
        (struct sexpr_io *io);

/*! \brief Read S-Expression from I/O Context
 *  \param[in] io The context to read from.
 *  \return The s-expression that was read, or sx_nonexistent if there is no
 *          s-expression to read, or sx_end_of_file if nothing may be read from
 *          the I/O context anymore.
 *
 *  This functions reads from the input of the I/O context and returns the
 *  s-expression that was read.
 */
sexpr sx_read
        (struct sexpr_io *io);

/*! \brief Write S-Expression to I/O Context
 *  \param[in] io The context to write to.
 *  \param[in] sx The s-expression to write.
 *
 *  This functions writes the given s-expression to the output of the given
 *  context.
 */
void sx_write
        (struct sexpr_io *io, sexpr sx);

/*! @} */

/*! \defgroup sexprConstructors Constructors
 *  \ingroup sexpr
 *  \brief Making new S-Expressions and destroying old ones
 *
 *  @{
 */

/*! \brief Create a new Cons
 *  \param[in] car The car of the new cons.
 *  \param[in] cdr The cdr of the new cons.
 *  \return The new cons, or sx_nonexistent if there's not enough memory.
 *
 *  This function takes two s-expressions, car and cons, and creates a new cons
 *  s-expression with the two arguments as its contents.
 */
sexpr cons
        (sexpr car, sexpr cdr);

/*! \brief Encode an Integer
 *  \param[in] integer The value of the new s-expression.
 *  \return The s-expression.
 *
 *  This macro takes a C integer and returns a new s-expression with the
 *  integer as its value.
 */
#define make_integer(integer)\
        ((sexpr)((((int_pointer_s)(integer)) << 3) | 0x3))

/*! \brief Encode a Special S-Expression
 *  \param[in] code The value of the new s-expression.
 *  \return The s-expression.
 *
 *  These s-expressions may be used as sentinel values and the like.
 */
#define make_special(code)\
        ((sexpr)((((int_pointer)(code)) << 3) | 0x5))

/*! \brief Define String or Symbol statically
 *  \internal
 *  \param[in] t The type of what is to be defined.
 *  \param[in] n The name to give the new variable.
 *  \param[in] s The string itself.
 *
 *  This defines either a string or a symbol statically in the scope it was
 *  called in. Most often you would use this in either a header file or in a
 *  .c file for strings or symbols that you need to use throughout the file.
 *  You should never call this but instead you should call define_string() or
 *  define_symbol().
 */
#define define_sosi(t,n,s) \
    static const struct sexpr_string_or_symbol sexpr_payload_ ## n = { t, s };\
    static const sexpr n = ((const sexpr)&(sexpr_payload_ ## n))

/*! \brief Define String statically
 *  \param[in] name  The name to give the new variable.
 *  \param[in] value The string itself.
 *
 *  This defines a string statically in the scope it was called in. Most often
 *  you would use this in either a header file or in a .c file for strings that
 *  you need to use throughout the file. You should use this whenever you'd
 *  want to use make_string() with a constant value, so as to reduce the runtime
 *  overhead of the programme.
 */
#define define_string(name,value) define_sosi(sxt_string,name,value)

/*! \brief Define Symbol statically
 *  \param[in] name  The name to give the new variable.
 *  \param[in] value The symbol name.
 *
 *  This defines a symbol statically in the scope it was called in. Most often
 *  you would use this in either a header file or in a .c file for symbols that
 *  you need to use throughout the file. You should use this whenever you'd
 *  want to use make_symbol() with a constant value, so as to reduce the runtime
 *  overhead of the programme.
 */
#define define_symbol(name,value) define_sosi(sxt_symbol,name,value)

/*! \brief Create a new String
 *  \param[in] string The string to use.
 *  \return The new s-expression, or sx_nonexistent if there's not enough
 *          memory.
 *
 *  This function takes a C string and returns a new s-expression with the
 *  string as its value.
 */
sexpr make_string
        (const char *string);

/*! \brief Create a new Symbol
 *  \param[in] symbol The string to use.
 *  \return The new s-expression, or sx_nonexistent if there's not enough
 *          memory.
 *
 *  This function takes a C string and returns a new symbol-type s-expression
 *  with the string as its value.
 */
sexpr make_symbol
        (const char *symbol);

/*! \brief Destroy S-Expression
 *  \param[in] sx The s-expression to destroy.
 *
 *  This function deallocates all storage associated with the s-expression;
 *  unlike previous versions, there's no reference counting involved, so this'll
 *  deallovate the storage immediately (but not recursively).
 */
void sx_destroy
        (sexpr sx);

/*! \brief NIL (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_nil make_special(1)

/*! \brief Boolean False (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_false make_special(2)

/*! \brief Boolean True (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_true make_special(3)

/*! \brief Empty List (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_empty_list make_special(4)

/*! \brief End of List (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_end_of_list make_special(5)

/*! \brief End of File (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_end_of_file make_special(6)

/*! \brief Not-a-Number (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_not_a_number make_special(7)

/*! \brief Nonexistent (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_nonexistent make_special(8)

/*! \brief Dot Operator (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_dot make_special(9)

/*! \brief Quote Operator (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_quote make_special(10)

/*! \brief Quasiquote Operator (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_quasiquote make_special(11)

/*! \brief Unquote Operator (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_unquote make_special(12)

/*! \brief Splice Operator (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie
 *  programme, so it is kept as a constant.
 */
#define sx_splice make_special(13)

/*! \brief S-Expression Pointer Flag
 *
 *  This flag is not set in the memory encoding of an s-expression if it's a
 *  pointer instead of the literal value.
 */
#define sx_mask_no_pointer 0x1

/*! \brief Register Custom S-Expression Type
 *  \param[in]     type        Type ID to register.
 *  \param[in,out] serialise   This callback is called when an S-expression of
 *                             this type needs to be written with sx_write(), or
 *                             someone felt like getting a serialised version of
 *                             the expression.
 *  \param[in,out] unserialise This callback is called when an S-expression
 *                             encoded by serialise() is read in or otherwise
 *                             needs to be restored.
 *  \param[in,out] tag         Called when an S-expression of this type is
 *                             tagged. Use this to tag sub-expressions.
 *  \param[in,out] destroy     Called when sx_destroy() is invoked on an
 *                             S-expression of this type.
 *  \param[in,out] call        Called when the garbage collector runs, this
 *                             function is expected to invoke gc_call() on all
 *                             S-expressions of this type that may be eligible
 *                             for garbage collection.
 *  \param[in,out] equalp      Called when two S-exprssions of this type need
 *                             to be compared for equality. Not providing this
 *                             function means two S-expressions of this type are
 *                             only equalp if they point to the same location.
 *
 *  The type ID you use should be greater than 32. Coincidentally the serialised
 *  form of your S-expression will be of the form (type ...) with type being
 *  represented as an UTF-8 character, so you should use the number of a fitting
 *  UTF-8 glyph for your purposes.
 */
void sx_register_type
        (unsigned int type,
         sexpr (*serialise) (sexpr), sexpr (*unserialise) (sexpr),
         void (*tag) (sexpr), void (*destroy) (sexpr), void (*call) (),
         sexpr (*equalp) (sexpr, sexpr));

/*! @} */

/*! \defgroup sexprPredicates Predicates
 *  \ingroup sexpr
 *  \brief S-Expression Predicates
 *
 *  Predicates are functions (or function-like macros) that return a boolean
 *  value for the given arguments. For example consp() checks if a given
 *  s-expression is a cons, and equalp() checks if two s-expressions match up.
 *
 *  In Curie we're using the CL naming convention for predicates, in that all
 *  predicates end in 'p'.
 *
 *  @{
 */

/*! \brief Check if the two S-Expressions are equal
 *  \param[in] a S-expression to compare with b.
 *  \param[in] b S-expression to compare with a.
 *  \return sx_true if the two sexprs are equal or sx_false otherwise.
 *
 *  This function is used to determine whether the two s-expressions are the
 *  same, like the C '==' operator. However, the C type operator would only
 *  check if the two s-expressions were the exact same object; this function
 *  will try this test first, and if it fails it will examine the s-expressions
 *  themselves to find out if they're equivalent. If they are, sx_true is
 *  returned.
 *
 *  \note Unlike the type predicates, this function returns an s-expression, so
 *        if(equalp()) will not work in C code. Instead, use if(truep(equalp()))
 *        to test things.
 */
sexpr equalp
        (sexpr a, sexpr b);

/*! \brief Check if the S-Expression is a Custom Expression
 *  \param[in] sx The s-expression to check.
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is an expression of any custom type, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define customp(sx) (pointerp(sx) && (((struct sexpr_partial *)sx_pointer(sx))->type > 32))

/*! \brief Check if the S-Expression is a Custom Expression
 *  \param[in] sx The s-expression to check.
 *  \param[in] sxtype The s-expression to check.
 *  \return 1 if it is an expression of a certain custom type, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 *
 *  The primary intention of this particular macro is to aid in writing
 *  predicates for custom types.
 */
#define sx_customp(sx,sxtype) (pointerp(sx) && (((struct sexpr_partial *)sx_pointer(sx))->type == sxtype))

/*! \brief Check if the S-Expression is a Special Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a special expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define specialp(sx) ((((int_pointer)(sx)) & 0x7) == 0x5)

/*! \brief Check if the S-Expression is a Pointer
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a pointer, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define pointerp(sx) ((((int_pointer)(sx)) & sx_mask_no_pointer) == 0x0)

/*! \brief Check if the S-Expression is the NIL Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nilp(sx)   ((sexpr)(sx) == sx_nil)

/*! \brief Check if the S-Expression is the Boolean True Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is true, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define truep(sx)  ((sexpr)(sx) == sx_true)

/*! \brief Check if the S-Expression is is the Boolean False Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is false, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define falsep(sx) ((sexpr)(sx) == sx_false)

/*! \brief Check if the S-Expression is the empty List Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is an empty list, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define emptyp(sx) ((sexpr)(sx) == sx_empty_list)

/*! \brief Check if the S-Expression is the End-of-List Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the end-of-list expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define eolp(sx)   ((sexpr)(sx) == sx_end_of_list)

/*! \brief Check if the S-Expression is the End-of-File Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the end-of-file expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define eofp(sx)   ((sexpr)(sx) == sx_end_of_file)

/*! \brief Check if the S-Expression is the Not-a-Number Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the not-a-number expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nanp(sx)   ((sexpr)(sx) == sx_not_a_number)

/*! \brief Check if the S-Expression is the nonexistent Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the nonexistent expression, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nexp(sx)   ((sexpr)(sx) == sx_nonexistent)

/*! \brief Check if the S-Expression is the Dot Operator
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the dot operator, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define dotp(sx)   ((sexpr)(sx) == sx_dot)

/*! \brief Check if the S-Expression is the Quote Operator
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the dot operator, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define quotep(sx)   ((sexpr)(sx) == sx_quote)

/*! \brief Check if the S-Expression is the Quasiquote Operator
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the dot operator, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define qqp(sx)   ((sexpr)(sx) == sx_quasiquote)

/*! \brief Check if the S-Expression is the Unquote Operator
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the dot operator, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define unquotep(sx)   ((sexpr)(sx) == sx_unquote)

/*! \brief Check if the S-Expression is the Splice Operator
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is the dot operator, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define splicep(sx)   ((sexpr)(sx) == sx_splice)

/*! \brief Check if the S-Expression is a Cons
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a cons, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define consp(sx)    (pointerp(sx) && (((struct sexpr_cons *)sx_pointer(sx))->type == sxt_cons))

/*! \brief Check if the S-Expression is a String
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a string, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define stringp(sx)  (pointerp(sx) && (((struct sexpr_string_or_symbol *)sx_pointer(sx))->type == sxt_string))

/*! \brief Check if the S-Expression is a Symbol
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is a symbol, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define symbolp(sx)  (pointerp(sx) && (((struct sexpr_string_or_symbol *)sx_pointer(sx))->type == sxt_symbol))

/*! \brief Check if the S-Expression is an Integer
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is an integer, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define integerp(sx) ((((int_pointer)(sx)) & 0x7) == 0x3)

/*! @} */

/*! \defgroup sexprAccessors Accessors
 *  \ingroup sexpr
 *  \brief Accessing S-Expression Contents
 *
 *  These functions (and function-macros) allow you to get to the actual
 *  contents of the s-expressions that you've obtained in one way or another.
 *
 *  @{
 */

/*! \brief Access the Pointer Value of an S-Expression
 *  \param[in] sx The s-expression.
 *  \return The pointer.
 *
 *  For s-expressions that are used to encode a pointer, this macro retrieves
 *  that particular pointer.
 */
#define sx_pointer(sx) ((void *)sx)

/*! \brief Access the car of a Cons
 *  \param[in] sx The cons.
 *  \return The car of sx, or sx_nonexistent if sx is not a cons.
 *
 *  This macro returns the car of a cons. This macro is needed because the
 *  generic sexpr structure doesn't actually include the car, so some type
 *  juggling is in order to get this working.
 */
#define car(sx)        (consp(sx) ? (((struct sexpr_cons *)sx_pointer(sx))->car) : sx_nonexistent)

/*! \brief Access the cdr of a Cons
 *  \param[in] sx The cons.
 *  \return The cdr of sx, or sx_nonexistent if sx is not a cons.
 *
 *  Analoguous to car(), but it returns the cdr.
 */
#define cdr(sx)        (consp(sx) ? (((struct sexpr_cons *)sx_pointer(sx))->cdr) : sx_nonexistent)

/*! \brief Access the Integer Value of an Integer
 *  \param[in] sx The integer.
 *  \return The integer value, or an unspecified value if sx is not an integer.
 *
 *  Remember to check for the actual type if it's important whether you're
 *  examining an integer or not. This macro will also return the ID for special
 *  s-expressions (true, false, eof, eol, custom type identifiers, etc...)
 */
#define sx_integer(sx) ((int_pointer_s)(sx & (~0x7)) >> 3)

/*! \brief Access the String Value of a String
 *  \param[in] sx The string.
 *  \return The string value, or "#nonexistent" if sx is not a string.
 *
 *  Since the "#nonexistent" return value for non-string objects is a valid C
 *  string itself, you should always make sure to check the type of sx yourself
 *  if it's actually important.
 */
#define sx_string(sx)  (const char *)(stringp(sx) ? (((struct sexpr_string_or_symbol *)sx_pointer(sx))->character_data) : "#nonexistent")

/*! \brief Access the String Value of a Symbol
 *  \param[in] sx The symbol.
 *  \return The string value, or "#nonexistent" if sx is not a symbol.
 *
 *  Since the "#nonexistent" return value for non-symbol objects is a valid C
 *  string itself, you should always make sure to check the type of sx yourself
 *  if it's actually important.
 */
#define sx_symbol(sx)  (const char *)(symbolp(sx) ? (((struct sexpr_string_or_symbol *)sx_pointer(sx))->character_data) : "#nonexistent")


#define sx_type(sx) (customp(sx) ? (((struct sexpr_partial *)sx_pointer(sx))->type) : 0)

/*! @} */

/*! \brief List Map
 *  \param[in] list The list to map.
 *  \param[in] f    The function to apply.
 *
 *  This function applies the given function to each element of f.
 */
void sx_list_map
        (sexpr list, void (*f)(sexpr));

/*! \brief List Fold
 *  \param[in] list The list to fold.
 *  \param[in] f    The function to apply.
 *  \param[in] seed The seed value to use.
 *  \return The value constructed by f().
 *
 *  This function applies the given function to each element of f, passing the
 *  return value of f to the next function call and ultimately returning the
 *  return value of the last call to f().
 */
sexpr sx_list_fold
        (sexpr list, sexpr (*f)(sexpr, sexpr), sexpr seed);

/*! \brief Join Strings/Symbols
 *  \param[in] a The first string or symbol.
 *  \param[in] b The second string or symbol.
 *  \param[in] c The third string or symbol.
 *  \return Concatenation of the arguments, or sx_nil for errors.
 *
 *  This function joins the three arguments into a single string or symbol. The
 *  type of the return value is the same as the type of the first argument. All
 *  arguments except for the first one may be something other than a string or
 *  a symbol, which omits them in the generated string. sx_nil is recommended
 *  for those arguments.
 */
sexpr sx_join
        (sexpr a, sexpr b, sexpr c);

#ifdef __cplusplus
}
#endif

#endif
