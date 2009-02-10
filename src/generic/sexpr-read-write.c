/*
 *  sexpr-read-write.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 15/06/2008.
 *  Copyright 2008, 2009 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, 2009, Magnus Deininger All rights reserved.
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

#include <curie/memory.h>
#include <curie/sexpr.h>
#include <curie/io.h>
#include <curie/io-system.h>
#include <curie/constants.h>

#include <curie/sexpr-internal.h>

static struct memory_pool sx_io_pool = MEMORY_POOL_INITIALISER(sizeof (struct sexpr_io));

static unsigned int sx_write_dispatch (struct sexpr_io *io, sexpr sx);
/*@notnull@*/ /*@shared@*/ static sexpr sx_read_dispatch
        (unsigned int *i, char *buf, unsigned int length);

struct sexpr_io *sx_open_io(struct io *in, struct io *out) {
    struct sexpr_io *rv = get_pool_mem (&sx_io_pool);

    if (rv == (struct sexpr_io *)0)
    {
        io_close (in);
        io_close (out);
        return (struct sexpr_io *)0;
    }

    /*@-mustfree@*/
    rv->in = in;
    rv->out = out;
    /*@=mustfree@*/

    if ((in->type != iot_read) &&
        (in->type != iot_special_read) &&
        (in->type != iot_special_write))
    {
        in->type = iot_read;
    }

    if ((out->type != iot_write) &&
        (out->type != iot_special_read) &&
        (out->type != iot_special_write))
    {
        out->type = iot_write;
    }

    return rv;
}

struct sexpr_io *sx_open_stdio() {
    struct io *in, *out;

    if ((in = io_open (0)) == (struct io *)0)
    {
        return (struct sexpr_io *)0;
    }

    if ((out = io_open (1)) == (struct io *)0)
    {
        io_close (in);
        return (struct sexpr_io *)0;
    }

    return sx_open_io (in, out);
}

void sx_close_io (struct sexpr_io *io) {
    io_close (io->in);
    io_close (io->out);

    free_pool_mem (io);
}

/*@notnull@*/ /*@shared@*/ static sexpr sx_read_string
        (unsigned int *i, char *buf, unsigned int length)
{
    unsigned int j = *i, k = 0;
    char newstring [SX_MAX_STRING_LENGTH];

    do {
        if (buf[j] == '"') {
            /* closing ", end of string */
            j++;
            *i = j;
            newstring[k] = (char)0;

            /* return the newly created string */
            return make_string (newstring);
        } else if (buf[j] == '\\') {
            /* literal inclusion of the next character... */
            j++;
            if (j >= length) return sx_nonexistent;
        }

        if (k < (SX_MAX_STRING_LENGTH - 2)) {
            /* make sure we still have enough room, then add the character */
            newstring[k] = buf[j];
            k++;
        }
        j++;
    } while (j < length);

    return sx_nonexistent;
}

/*@notnull@*/ /*@shared@*/ static sexpr sx_read_number
        (unsigned int *i, char *buf, unsigned int length)
{
    unsigned int j = *i;
    signed long number = 0;
    char number_is_negative = (char)0;

    switch (buf[j])
    {
        case '-':
            number_is_negative = (char)1;
            /*@fallthrough@*/
        case '+':
            j++;
            if (j >= length) return sx_nonexistent;
    }

    do {
        switch (buf[j])
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                /* append to the number */
                number = (number * 10) + (long int)(buf[j] - '0');
                break;
            default:
                /* end of number */
                *i = j;
                if (number_is_negative == (char)1) {
                    number *= -1;
                }

                return make_integer (number);
        }
        j++;
    } while (j < length);

    return sx_nonexistent;
}

/*@notnull@*/ /*@shared@*/ static sexpr sx_read_symbol
        (unsigned int *i, char *buf, int unsigned length)
{
    unsigned int j = *i, k = 0;
    char newsymbol [SX_MAX_SYMBOL_LENGTH];

    do {
        switch (buf[j]) {
            case '\n':
            case '\t':
            case '\v':
            case ' ':
            case ';': /* beginning a comment will also end the symbol */
            case 0:
            case ')': /* this is also a terminating character for a symbol */
            case '(': /* starting a new cons is also end a symbol */
                /* end of symbol */

                *i = j;
                newsymbol[k] = (char)0;

                /* return the newly created string */
                return make_symbol (newsymbol);
            default:
                if (k < (SX_MAX_SYMBOL_LENGTH - 2)) {
                    /* make sure we still have enough room, then add the
                       character */
                    newsymbol[k] = buf[j];
                    k++;
                }
        }

        j++;
    } while (j < length);

    return sx_nonexistent;
}

/*@-branchstate@*/
/*@notnull@*/ /*@shared@*/ static sexpr sx_read_cons_finalise
        (/*@shared@*/ sexpr oreverse)
{
    sexpr result = sx_end_of_list;
    sexpr reverse = oreverse;

    while (consp(reverse)) {
        sexpr ncar = car (reverse);
        if (dotp (ncar)) {
            sexpr nresult = car(result);
            sx_xref (nresult);
            sx_destroy (result);
            result = nresult;
        } else {
            sx_xref(ncar);
            result = cons(ncar, result);
        }
        reverse = cdr (reverse);
    }

    sx_destroy(oreverse);

    return result;
}
/*@=branchstate@*/

/*@notnull@*/ /*@shared@*/ static sexpr sx_read_cons
        (unsigned int *i, char *buf, unsigned int length)
{
    /* i think the best we can do here, is to construct the list in reverse
       order using cons(), then once it's done we simply reverse it. */
    unsigned int j = *i;
    sexpr result = sx_end_of_list, next;

    do {
        char comment = (char)0;
         /* skip all currently-leading whitespace */
        retry:
        while (comment == (char)1) {
            j++;
            if (j >= length) return sx_nonexistent;

            if (buf[j] == '\n') {
                comment = (char)0;
                break;
            }
        }

        switch (buf[j]) {
            case '\n':
            case '\t':
            case '\v':
            case ' ':
            case 0:
                j++;
                if (j >= length) return sx_nonexistent;
                goto retry;

            case ';':
                comment = (char)1;
                goto retry;

            default:
                break;
        }

        switch (buf[j]) {
            case ')':
                /* end of list */
                j++;
                *i = j;
                return sx_read_cons_finalise (result);

            default:
                next = sx_read_dispatch (&j, buf, length);
        }

        if (next == sx_nonexistent) return sx_nonexistent;

        result = cons (next, result);
    } while (j < length);

    return sx_nonexistent;
}

static sexpr sx_read_dispatch
        (unsigned int *i, char *buf, unsigned int length)
{
    switch (buf[(*i)]) {
        case '"':
            /* string */
            (*i)++; /* string will start one byte after this character */
            return sx_read_string(i, buf, length);
        case '-':
        case '+':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            /* number */
            return sx_read_number(i, buf, length);
        case '(':
            /* cons, or list */
            (*i)++; /* cons will start one byte after this character */
            return sx_read_cons(i, buf, length);
        case '.':
            (*i)++;
            return sx_dot;
        case '\'':
            (*i)++;
            return sx_quote;
        case '`':
            (*i)++;
            return sx_quasiquote;
        case ',':
            if ((*i) < (length + 1))
            {
                (*i)++;
                if (buf[(*i)] == '@')
                {
                    (*i)++;
                    return sx_splice;
                }
                return sx_unquote;
            }
            else
            {
                return sx_nonexistent;
            }
        case '#':
            if ((*i) < (length + 1))
            {
                int j = (*i)+1;
                (*i) += 2;

                switch (buf[j]) {
/* the internal control sequences are merely listed as a reference here, they
   will not be read in 'properly' */
/*                    case '.':
                        return sx_end_of_file;
                    case 'x':
                        return sx_nonexistent;
                    case 'e':
                        return sx_end_of_list;*/

                    case '-':
                        return sx_not_a_number;

                    case 't':
                        return sx_true;
                    case 'f':
                        return sx_false;

                    default:
                        return sx_nil;
                }
            }

            return sx_nonexistent;
        default:
            /* symbol */
            return sx_read_symbol(i, buf, length);
    }
}

sexpr sx_read(struct sexpr_io *io) {
    enum io_result r;
    char *buf;
    unsigned int i, length;
    sexpr result = sx_nonexistent;
    char comment = (char)0;

    do {
        r = io_read (io->in);
        i = io->in->position;
        length = io->in->length;
    } while ((r == io_changes) && (length < SX_MAX_READ_THRESHOLD));

    if (length == 0) {
        return sx_nonexistent;
    }

    buf = io->in->buffer;
    if (buf == (char *)0) return sx_nonexistent;

    /* remove leading whitespace */
    do {
        if (comment == (char)1) {
            if (buf[i] == '\n') {
                comment = (char)0;
            }
            i++;
            continue;
        }

        switch (buf[i]) {
            case '\n':
            case '\t':
            case '\v':
            case ' ':
            case 0:
                /* whitespace characters */
            case ')': /* stray closing parentheses are also ignored, yarr */
                break;
            case ';':
                comment = (char)1;
                break;
            default:
                /* update current position, so the whitespace will be removed
                   next time around. */
                io->in->position = i;
                goto done_skipping_whitespace;
        }
        i++;
    } while (i < length);

    done_skipping_whitespace:

    /* check that there actually /is/ something to parse, bail if not */
    if (i == length) {
        io->in->length = 0;
        io->in->position = 0;

        switch (io->in->status) {
            case io_end_of_file:
            case io_unrecoverable_error:
                return sx_end_of_file;
            default:
                return sx_nonexistent;
        }
    }

    result = sx_read_dispatch (&i, buf, length);

    if (result != sx_nonexistent) {
        io->in->position = i;
    }

    return result;
}

static void sx_write_string_or_symbol (struct io *io, struct sexpr_string_or_symbol *sexpr) {
    unsigned int i, j;

    for (i = 0; sexpr->character_data[i] != (char)0; i++);

    if (i != 0) {
        if (sexpr->header.type == sxt_string) {
            (void)io_collect (io, "\"", 1);
            /* TODO: this is actually super-inefficient... but it works */
            for (j = 0; j < i; j++) {
                if ((sexpr->character_data[j] == '"') || (sexpr->character_data[j] == '\\')) {
                    (void)io_collect (io, "\\", 1);
                }
                (void)io_collect (io, sexpr->character_data + j, 1);
            }
            (void)io_collect (io, "\"", 1);
        } else
            (void)io_collect (io, sexpr->character_data, i);
    } else if (sexpr->header.type == sxt_string) {
        (void)io_collect (io, "\"\"", 2);
    }
}

static void sx_write_cons (struct sexpr_io *io, struct sexpr_cons *sx) {
    unsigned int r;

    (void)io_collect (io->out, "(", 1);

    retry:

    r = sx_write_dispatch (io, sx->car);

    if (consp(sx->cdr)) {
        if (r == 1)
        {
            (void)io_collect (io->out, " ", 1);
        }
        sx = (struct sexpr_cons *)sx_pointer(sx->cdr);
        goto retry;
    }

    if (!eolp(sx->cdr)) {
        (void)io_collect (io->out, " . ", 3);
        (void)sx_write_dispatch (io, sx->cdr);
    }

    (void)io_collect (io->out, ")", 1);
}

static void sx_write_integer (struct io *io, int_pointer_s i) {
    char num [SX_MAX_NUMBER_LENGTH];

    int neg = 0;
    unsigned int j = 0;

    if(i < 0) {
        neg = 1;
        i *= -1;
    }

    do {
        char s;
        s = '0' + (char)(i % 10);

        num[(SX_MAX_NUMBER_LENGTH-2)-j] = s;

        i /= 10;
        j++;
    } while ((i != 0) && (j < (SX_MAX_NUMBER_LENGTH-2)));

    if(neg == 1) {
        num[SX_MAX_NUMBER_LENGTH-2-j] = '-';
        j++;
    }
    num[(SX_MAX_NUMBER_LENGTH-1)] = (char)0;

    (void)io_collect (io, num+((SX_MAX_NUMBER_LENGTH-1) -j), j);
}

static unsigned int sx_write_dispatch (struct sexpr_io *io, sexpr sx)
{
    if (symbolp(sx) || stringp(sx))
    {
        sx_write_string_or_symbol (io->out, (struct sexpr_string_or_symbol *)sx_pointer(sx));
        return 1;
    }
    else if (consp(sx))
    {
        sx_write_cons (io, (struct sexpr_cons *)sx_pointer(sx));
        return 1;
    }
    else if (integerp(sx))
    {
        sx_write_integer (io->out, sx_integer(sx));
        return 1;
    }
    else if (nilp(sx))
    {
        (void)io_collect (io->out, "#v", 2);
        return 1;
    }
    else if (falsep(sx))
    {
        (void)io_collect (io->out, "#f", 2);
        return 1;
    }
    else if (truep(sx))
    {
        (void)io_collect (io->out, "#t", 2);
        return 1;
    }
    else if (eolp(sx) || emptyp(sx))
    {
        (void)io_collect (io->out, "()", 2);
        return 1;
    }
    else if (eofp(sx))
    {
        (void)io_collect (io->out, "#.", 2);
        return 1;
    }
    else if (nanp(sx))
    {
        (void)io_collect (io->out, "#-", 2);
        return 1;
    }
    else if (nexp(sx))
    {
        (void)io_collect (io->out, "#x", 2);
        return 1;
    }
    else if (dotp(sx))
    {
        (void)io_collect (io->out, ".", 1);
        return 0;
    }
    else if (quotep(sx))
    {
        (void)io_collect (io->out, "'", 1);
        return 0;
    }
    else if (qqp(sx))
    {
        (void)io_collect (io->out, "`", 1);
        return 0;
    }
    else if (unquotep(sx))
    {
        (void)io_collect (io->out, ",", 1);
        return 0;
    }
    else if (splicep(sx))
    {
        (void)io_collect (io->out, ",@", 2);
        return 0;
    }
    else
    {
        (void)io_collect (io->out, "#?", 2);
        return 1;
    }
}

void sx_write(struct sexpr_io *io, sexpr sx) {
    if (sx_write_dispatch(io, sx) == 1)
    {
        (void)io_write (io->out, "\n", 1);
    }
}
