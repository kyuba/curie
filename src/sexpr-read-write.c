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

#include <curie/memory.h>
#include <curie/sexpr.h>
#include <curie/io.h>
#include <curie/constants.h>
#include <curie/utf-8.h>

#include <curie/sexpr-internal.h>

static sexpr sx_read_dispatch (unsigned int *i, char *buf, unsigned int length);
static unsigned int sx_write_dispatch (struct sexpr_io *io, sexpr sx);

struct sexpr_io *sx_open_io(struct io *in, struct io *out)
{
    static struct memory_pool pool
            = MEMORY_POOL_INITIALISER(sizeof (struct sexpr_io));
    struct sexpr_io *rv = get_pool_mem (&pool);

    if (rv == (struct sexpr_io *)0)
    {
        io_close (in);
        io_close (out);
        return (struct sexpr_io *)0;
    }

    rv->in = in;
    rv->out = out;

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

void sx_close_io (struct sexpr_io *io)
{
    io_close (io->in);
    io_close (io->out);

    free_pool_mem (io);
}

static sexpr sx_read_string_long
        (unsigned int *i, char *buf, unsigned int length)
{
    unsigned int j = *i, k = 0, m = *i;
    char *newstring;

    do {
        if (buf[m] == '"') {
            m -= j - 3;
            goto allocate;
        } else if (buf[m] == '\\') {
            m++;
        }

        m++;
    } while (m < length);

    return sx_nonexistent;

  allocate:
    newstring = aalloc (m);
    if (newstring == (char *)0)
    {
        return sx_nonexistent;
    }

    do {
        if (buf[j] == '"') {
            sexpr res;
            j++;
            *i = j;
            newstring[k] = (char)0;

            res = make_string (newstring);
            afree (m, newstring);

            return res;
        } else if (buf[j] == '\\') {
            j++;
        }

        newstring[k] = buf[j];
        k++;
        j++;
    } while (j < length);

    return sx_nonexistent;
}

static sexpr sx_read_string
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
        else
        {
            return sx_read_string_long (i, buf, length);
        }

        j++;
    } while (j < length);

    return sx_nonexistent;
}

static sexpr sx_read_number
        (unsigned int *i, char *buf, unsigned int length)
{
    define_symbol (sym_plus,  "+");
    define_symbol (sym_minus, "-");
    unsigned int j = *i;
    signed long number = 0;
    char number_is_negative = (char)0, hadnum = (char)0;

    switch (buf[j])
    {
        case '-':
            number_is_negative = (char)1;
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
                hadnum = (char)1;
                break;
            default:
                /* end of number */
                *i = j;
                if (hadnum == (char)0)
                {
                    return ((number_is_negative == (char)1) ?
                            sym_minus : sym_plus);
                }
                else
                {
                    if (number_is_negative == (char)1) {
                        number *= -1;
                    }

                    return make_integer (number);
                }
        }
        j++;
    } while (j < length);

    return sx_nonexistent;
}

static sexpr sx_read_symbol
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
            case '(': /* starting a new cons will also end a symbol */
                /* end of symbol */

                *i = j;
                newsymbol[k] = (char)0;

                if (utf8_get_character ((int_8*)newsymbol, 0, &j) == k)
                {
                    if (j > 127)
                    {
                        return make_special (j);
                    }
                }

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

static sexpr sx_read_cons_finalise (sexpr oreverse)
{
    sexpr result = sx_reverse (oreverse);
    sexpr reverse = car (result);

    if (specialp (reverse))
    {
        unsigned int i = sx_integer (reverse);
        struct sexpr_type_descriptor *d;

        reverse = cdr (result);

        if (i > 127)
        {
            if (((d = sx_get_descriptor (i))
                  != (struct sexpr_type_descriptor *)0) &&
                ((d->unserialise) != (void *)0))
            {
                return d->unserialise (reverse);
            }
        }
    }

    return result;
}

static sexpr sx_read_cons
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
        if (sexpr->type == sxt_string) {
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
    } else if (sexpr->type == sxt_string) {
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
    else if (customp(sx))
    {
        int type = sx_type (sx);
        struct sexpr_type_descriptor *d = sx_get_descriptor (type);

        if ((d != (struct sexpr_type_descriptor *)0) &&
             (d->serialise != (void *)0))
        {
            sx_write_dispatch (io, cons (make_special (type),
                               d->serialise (sx)));
        }

        return 1;
    }
    else if (specialp(sx))
    {
        unsigned int i = sx_integer (sx);
        char t[4];

        i = utf8_encode ((int_8*)t, i);

        io_collect (io->out, t, i);
        return 1;
    }
    else
    {
        (void)io_collect (io->out, "#?", 2);
        return 1;
    }
}

void sx_write(struct sexpr_io *io, sexpr sx)
{
    if (sx_write_dispatch(io, sx) == 1)
    {
        (void)io_write (io->out, "\n", 1);
    }
}
