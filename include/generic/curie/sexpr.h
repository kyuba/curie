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

/*! \brief S-Expression Type
 *
 *  All S-expressions have a type, which is part of their in-memory
 *  representation.
 */
enum sx_type {
    sxt_nil = 0,              /*!< NIL */
    sxt_false = 1,            /*!< Boolean 'False', i.e. #f */
    sxt_true = 2,             /*!< Boolean 'True', i.e. #t */
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
    sxt_dot = 12              /*!< Dot, i.e. . */
};

/*! \brief Generic S-Expressions
 *  \internal
 *
 *  Base type for symbolic expressions. You should use the typedef instead in
 *  application code.
 */
struct sexpr_header {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all sexprs.
     */
    enum sx_type type;

    /*! \brief Reference Count
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;
};

/*! \brief S-Expression Type
 *
 *  Outside of the library, s-expressions should always be referenced by
 *  pointers. The library is nice enough to make sure never to return (sexpr)0
 *  pointers, so this ought to be alright.
 */
typedef struct sexpr_header * sexpr;

/*! \brief Integer Sexpr
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
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;

    signed long integer;
};

/*! \brief Cons-Sexpr
 *  \internal
 */
struct sexpr_cons {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all sexprs.
     */
    enum sx_type type;

    /*! \brief Reference Count
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;

    /*@shared@*/ sexpr car;
    /*@shared@*/ sexpr cdr;
};

/*! \brief String-/Symbol-Sexpr
 *  \internal
 */
struct sexpr_string_or_symbol {
    /*! \brief S-Expression Type
     *
     *  This field defines the type of the symbolic expression. It's present in
     *  all sexprs.
     */
    enum sx_type type;

    /*! \brief Reference Count
     *
     *  This keeps track of how often this particular expression is in use.
     */
    signed int references;

    unsigned int length;

    char character_data[];
};

struct sexpr_io;

/*@null@*/ /*@only@*/ struct sexpr_io *sx_open_io
        (/*@notnull@*/ /*@only@*/ struct io *,
         /*@notnull@*/ /*@only@*/ struct io *);
/*@null@*/ /*@only@*/ struct sexpr_io *sx_open_stdio ();

void sx_close_io
        (/*@notnull@*/ /*@only@*/ struct sexpr_io *);

/*@notnull@*/ /*@shared@*/ sexpr sx_read
        (/*@notnull@*/ struct sexpr_io *);
void sx_write
        (/*@notnull@*/ struct sexpr_io *,
         /*@notnull@*/ sexpr );

/*@notnull@*/ /*@shared@*/ sexpr cons
        (/*@notnull@*/ /*@shared@*/ sexpr,
         /*@notnull@*/ /*@shared@*/ sexpr );
/*@notnull@*/ /*@shared@*/ sexpr make_integer
        (signed long);
/*@notnull@*/ /*@shared@*/ sexpr make_string
        (/*@notnull@*/ const char *);
/*@notnull@*/ /*@shared@*/ sexpr make_symbol
        (/*@notnull@*/ const char *);

void sx_destroy
        (/*@notnull@*/ /*@shared@*/ sexpr );
void sx_xref
        (/*@notnull@*/ /*@shared@*/ sexpr );

void *sx_list_map
        (/*@notnull@*/ sexpr,
         /*@notnull@*/ void (*)(sexpr, void *),
         /*@null@*/ void *);
/*@notnull@*/ /*@shared@*/ sexpr sx_list_fold
        (/*@notnull@*/ sexpr,
         /*@notnull@*/ void (*)(sexpr ));

/*@notnull@*/ /*@shared@*/ sexpr equalp
        (/*@notnull@*/ sexpr,
         /*@notnull@*/ sexpr );

/*@notnull@*/ /*@shared@*/ extern sexpr const sx_nil;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_false;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_true;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_empty_list;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_end_of_list;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_end_of_file;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_not_a_number;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_nonexistent;
/*@notnull@*/ /*@shared@*/ extern sexpr const sx_dot;

#define nilp(sx)   (((sexpr )(sx) == sx_nil)          || ((sx)->type == sxt_nil))
#define truep(sx)  (((sexpr )(sx) == sx_true)         || ((sx)->type == sxt_true))
#define falsep(sx) (((sexpr )(sx) == sx_false)        || ((sx)->type == sxt_false))
#define emptyp(sx) (((sexpr )(sx) == sx_empty_list)   || ((sx)->type == sxt_empty_list))
#define eolp(sx)   (((sexpr )(sx) == sx_end_of_list)  || ((sx)->type == sxt_end_of_list))
#define eofp(sx)   (((sexpr )(sx) == sx_end_of_file)  || ((sx)->type == sxt_end_of_file))
#define nanp(sx)   (((sexpr )(sx) == sx_not_a_number) || ((sx)->type == sxt_not_a_number))
#define nexp(sx)   (((sexpr )(sx) == sx_nonexistent)  || ((sx)->type == sxt_nonexistent))
#define dotp(sx)   (((sexpr )(sx) == sx_dot)          || ((sx)->type == sxt_dot))

#define consp(sx)    ((sx)->type == sxt_cons)
#define stringp(sx)  ((sx)->type == sxt_string)
#define symbolp(sx)  ((sx)->type == sxt_symbol)
#define integerp(sx) ((sx)->type == sxt_integer)

#define car(sx)        (((sx)->type == sxt_cons) ? (((struct sexpr_cons *)(sx))->car) : sx_nonexistent)
#define cdr(sx)        (((sx)->type == sxt_cons) ? (((struct sexpr_cons *)(sx))->cdr) : sx_nonexistent)

#define sx_integer(sx) (((sx)->type == sxt_integer) ? (((struct sexpr_integer *)(sx))->integer) : -1)
#define sx_string(sx)  (const char *)(((sx)->type == sxt_string) ? (((struct sexpr_string_or_symbol *)(sx))->character_data) : "#nonexistent")
#define sx_symbol(sx)  (const char *)(((sx)->type == sxt_symbol) ? (((struct sexpr_string_or_symbol *)(sx))->character_data) : "#nonexistent")

#ifdef __cplusplus
}
#endif

#endif
