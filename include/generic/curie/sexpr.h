/*
 *  sexpr.h
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

/*! \defgroup sexpr S-Expressions
 *  \brief Symbolic Expression Handling
 *
 *  @{
 */

/*! \file
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

struct sexpr_generic;
/*! \brief S-Expression
 *
 *  Outside of the library, s-expressions should always be referenced by
 *  pointers. The library is nice enough to make sure never to return (sexpr)0
 *  pointers, so this ought to be alright.
 */
typedef struct sexpr_generic * sexpr;

/*! \defgroup sexpr_representation In-Memory Representation
 *  \brief In-Memory Representation of S-Expressions
 *  \internal
 *
 *  These data structures define the runtime memory layout for s-expressions.
 *  You really shouldn't need to grok into this too much.
 *
 *  @{
 */

/*! \brief S-Expression Type
 *
 *  All S-expressions have a type, which is part of their in-memory
 *  representation.
 */
enum sx_type {
    sxt_nil = 0,              /*!< NIL */
    sxt_false = 1,            /*!< Boolean 'False', i.e. \#f */
    sxt_true = 2,             /*!< Boolean 'True', i.e. \#t */
    sxt_integer = 3,          /*!< Integral Number, i.e. 42 */
    sxt_string = 4,           /*!< String, i.e. "string" */
    sxt_symbol = 5,           /*!< Symbol, i.e. symbol */
    sxt_cons = 6,             /*!< Cons, i.e. (x . y) */
    sxt_empty_list = 7,       /*!< \internal
                               *   Empty List */
    sxt_end_of_list = 8,      /*!< End of a List, i.e. () */
    sxt_end_of_file = 9,      /*!< End of File */
    sxt_not_a_number = 10,    /*!< Not-a-Number */
    sxt_nonexistent = 11,     /*!< Nonexistent (used in return values) */
    sxt_dot = 12              /*!< \internal
                               *   Dot, i.e. . */
};

/*! \brief Generic S-Expressions
 *  \internal
 *
 *  Base type for symbolic expressions. You should use the sexpr typedef instead
 *  in application code.
 */
struct sexpr_generic {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all sexprs.
     */
    enum sx_type type;

    /*! \brief Reference Count
     *  \internal
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;
};

/*! \brief Integer S-Expression
 *  \internal
 *
 *  This structure is used to represent integers as sexprs.
 */
struct sexpr_integer {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all sexprs.
     */
    enum sx_type type;

    /*! \brief Reference Count
     *  \internal
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;

    /*! \brief Integer Value
     *
     *  This is the integer value of this s-expression.
     */
    signed long integer;
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

    /*! \brief Reference Count
     *  \internal
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;

    /*! \brief First Cell
     *
     *  The first cell of the cons, or 'car' in lisp.
     */
    /*@shared@*/ sexpr car;

    /*! \brief Second Cell
     *
     *  The second cell of the cons, or 'cdr' in lisp.
     */
    /*@shared@*/ sexpr cdr;
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

    /*! \brief Reference Count
     *  \internal
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;

    /*! \brief String Length
     *  \todo Implement this.
     *
     *  This is the length of the contained string.
     */
    unsigned int length;

    /*! \brief String
     *
     *  The contained string itself. It's 0-terminated, too.
     */
    char character_data[];
};

/*! @} */

/*! \defgroup sexpr_io In-/Output
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
/*@null@*/ /*@only@*/ struct sexpr_io *sx_open_io
        (/*@notnull@*/ /*@only@*/ struct io *in,
         /*@notnull@*/ /*@only@*/ struct io *out);

/*! \brief Create Standard I/O as S-Expression I/O Context
 *  \return The new I/O context, or (struct sexpr_io *)0 if no memory could be
 *          allocated.
 *
 *  Analoguous to sx_open_io(), but it uses the standard file descriptors '0'
 *  and '1'.
 */
/*@null@*/ /*@only@*/ struct sexpr_io *sx_open_stdio ();

/*! \brief Close S-Expression I/O Context
 *  \param[in] io The context to close.
 *
 *  This function will destroy the given I/O context and close and deallocate
 *  all resources used by it, including any involved io structures and file
 *  descriptors. Using the structure after calling this function on it may kill
 *  your programmes or set your cat on fire.
 */
void sx_close_io
        (/*@notnull@*/ /*@only@*/ struct sexpr_io *io);

/*! \brief Read S-Expression from I/O Context
 *  \param[in] io The context to read from.
 *  \return The s-expression that was read, or sx_nonexistent if there is no
 *          s-expression to read, or sx_end_of_file if nothing may be read from
 *          the I/O context anymore.
 *
 *  This functions reads from the input of the I/O context and returns the
 *  s-expression that was read.
 */
/*@notnull@*/ /*@shared@*/ sexpr sx_read
        (/*@notnull@*/ struct sexpr_io *io);

/*! \brief Write S-Expression to I/O Context
 *  \param[in] io The context to write to.
 *  \param[in] sx The s-expression to write.
 *
 *  This functions writes the given s-expression to the output of the given
 *  context.
 */
void sx_write
        (/*@notnull@*/ struct sexpr_io *io,
         /*@notnull@*/ sexpr sx);

/*! @} */

/*! \defgroup sexpr_constructors Constructors
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
/*@notnull@*/ /*@shared@*/ sexpr cons
        (/*@notnull@*/ /*@shared@*/ sexpr car,
         /*@notnull@*/ /*@shared@*/ sexpr cdr);

/*! \brief Create a new Integer
 *  \param[in] integer The value of the new s-expression.
 *  \return The new s-expression, or sx_nonexistent if there's not enough
 *          memory.
 *
 *  This function takes a C integer and returns a new s-expression with the
 *  integer as its value.
 */
/*@notnull@*/ /*@shared@*/ sexpr make_integer
        (signed long integer);

/*! \brief Create a new String
 *  \param[in] string The string to use.
 *  \return The new s-expression, or sx_nonexistent if there's not enough
 *          memory.
 *
 *  This function takes a C string and returns a new s-expression with the
 *  string as its value.
 */
/*@notnull@*/ /*@shared@*/ sexpr make_string
        (/*@notnull@*/ const char *string);

/*! \brief Create a new Symbol
 *  \param[in] symbol The string to use.
 *  \return The new s-expression, or sx_nonexistent if there's not enough
 *          memory.
 *
 *  This function takes a C string and returns a new symbol-type s-expression
 *  with the string as its value.
 */
/*@notnull@*/ /*@shared@*/ sexpr make_symbol
        (/*@notnull@*/ const char *symbol);

/*! \brief Destroy S-Expression
 *  \param[in] sx The s-expression to destroy.
 *
 *  This function deallocates all storage associated with the s-expression, if
 *  its reference counter drops to zero. Otherwise it just decreases the
 *  reference counter.
 */
void sx_destroy
        (/*@notnull@*/ /*@shared@*/ sexpr sx);

/*! \brief Add Reference
 *  \param[in] sx The s-expression to modify.
 *
 *  This function increases the reference counter of the given s-expression, so
 *  that a subsequent sx_destroy would not actually kill it immediately. Use
 *  this if you need independent references to an s-expression and you still
 *  want things to stay clean.
 */
void sx_xref
        (/*@notnull@*/ /*@shared@*/ sexpr sx);

/*! \brief NIL (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_nil;

/*! \brief Boolean False (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_false;

/*! \brief Boolean True (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_true;

/*! \brief Empty List (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_empty_list;

/*! \brief End of List (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_end_of_list;

/*! \brief End of File (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_end_of_file;

/*! \brief Not-a-Number (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_not_a_number;

/*! \brief Nonexistent (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_nonexistent;

/*! \brief Dot Operator (S-Expression)
 *
 *  There is always only one instance of this s-expression in a running Curie 
 *  programme, so it is kept as a constant.
 */
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_dot;

/*! @} */

/*! \defgroup predicates Predicates
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
/*@notnull@*/ /*@shared@*/ sexpr equalp
        (/*@notnull@*/ sexpr a,
         /*@notnull@*/ sexpr b);

/*! \brief Check if the S-Expression is the NIL Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nilp(sx)   (((sexpr)(sx) == sx_nil)          || ((sx)->type == sxt_nil))

/*! \brief Check if the S-Expression is the Boolean True Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define truep(sx)  (((sexpr)(sx) == sx_true)         || ((sx)->type == sxt_true))

/*! \brief Check if the S-Expression is is the Boolean False Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define falsep(sx) (((sexpr)(sx) == sx_false)        || ((sx)->type == sxt_false))

/*! \brief Check if the S-Expression is the empty List Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define emptyp(sx) (((sexpr)(sx) == sx_empty_list)   || ((sx)->type == sxt_empty_list))

/*! \brief Check if the S-Expression is the End-of-List Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define eolp(sx)   (((sexpr)(sx) == sx_end_of_list)  || ((sx)->type == sxt_end_of_list))

/*! \brief Check if the S-Expression is the End-of-File Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define eofp(sx)   (((sexpr)(sx) == sx_end_of_file)  || ((sx)->type == sxt_end_of_file))

/*! \brief Check if the S-Expression is the Not-a-Number Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nanp(sx)   (((sexpr)(sx) == sx_not_a_number) || ((sx)->type == sxt_not_a_number))

/*! \brief Check if the S-Expression is the nonexistent Expression
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define nexp(sx)   (((sexpr)(sx) == sx_nonexistent)  || ((sx)->type == sxt_nonexistent))

/*! \brief Check if the S-Expression is the Dot Operator
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define dotp(sx)   (((sexpr)(sx) == sx_dot)          || ((sx)->type == sxt_dot))

/*! \brief Check if the S-Expression is a Cons
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define consp(sx)    ((sx)->type == sxt_cons)

/*! \brief Check if the S-Expression is a String
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define stringp(sx)  ((sx)->type == sxt_string)

/*! \brief Check if the S-Expression is a Symbol
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define symbolp(sx)  ((sx)->type == sxt_symbol)

/*! \brief Check if the S-Expression is an Integer
 *  \param[in] sx The s-expression to check.
 *  \return 1 if it is nil, 0 otherwise.
 *
 *  This macro determines the type of the given s-expression, and the result is
 *  usable as a C boolean.
 */
#define integerp(sx) ((sx)->type == sxt_integer)

/*! @} */

/*! \defgroup sexpr_accessors Accessors
 *  \brief Accessing S-Expression Contents
 *
 *  These functions (and function-macros) allow you to get to the actual
 *  contents of the s-expressions that you've obtained in one way or another.
 *
 *  @{
 */

/*! \brief Access the car of a Cons
 *  \param[in] sx The cons.
 *  \return The car of sx, or sx_nonexistent if sx is not a cons.
 *
 *  This macro returns the car of a cons. This macro is needed because the
 *  generic sexpr structure doesn't actually include the car, so some type
 *  juggling is in order to get this working.
 */
#define car(sx)        (((sx)->type == sxt_cons) ? (((struct sexpr_cons *)(sx))->car) : sx_nonexistent)

/*! \brief Access the cdr of a Cons
 *  \param[in] sx The cons.
 *  \return The cdr of sx, or sx_nonexistent if sx is not a cons.
 *
 *  Analoguous to car(), but it returns the cdr.
 */
#define cdr(sx)        (((sx)->type == sxt_cons) ? (((struct sexpr_cons *)(sx))->cdr) : sx_nonexistent)

/*! \brief Access the Integer Value of an Integer
 *  \param[in] sx The integer.
 *  \return The integer value, or -1 if sx is not an integer.
 *
 *  Since the -1 return value for non-integer objects is a valid integer itself,
 *  you should always make sure to check the type of sx yourself if it's
 *  actually important.
 */
#define sx_integer(sx) (((sx)->type == sxt_integer) ? (((struct sexpr_integer *)(sx))->integer) : -1)

/*! \brief Access the String Value of a String
 *  \param[in] sx The string.
 *  \return The string value, or "#nonexistent" if sx is not a string.
 *
 *  Since the "#nonexistent" return value for non-string objects is a valid C
 *  string itself, you should always make sure to check the type of sx yourself
 *  if it's actually important.
 */
#define sx_string(sx)  (const char *)(((sx)->type == sxt_string) ? (((struct sexpr_string_or_symbol *)(sx))->character_data) : "#nonexistent")

/*! \brief Access the String Value of a Symbol
 *  \param[in] sx The symbol.
 *  \return The string value, or "#nonexistent" if sx is not a symbol.
 *
 *  Since the "#nonexistent" return value for non-symbol objects is a valid C
 *  string itself, you should always make sure to check the type of sx yourself
 *  if it's actually important.
 */
#define sx_symbol(sx)  (const char *)(((sx)->type == sxt_symbol) ? (((struct sexpr_string_or_symbol *)(sx))->character_data) : "#nonexistent")

/*! @} */

#if 0
/* not implemented yet. */

void *sx_list_map
        (/*@notnull@*/ sexpr,
         /*@notnull@*/ void (*)(sexpr, void *),
         /*@null@*/ void *);
/*@notnull@*/ /*@shared@*/ sexpr sx_list_fold
        (/*@notnull@*/ sexpr,
         /*@notnull@*/ void (*)(sexpr));
#endif

#ifdef __cplusplus
}
#endif

#endif

/*! @} */
