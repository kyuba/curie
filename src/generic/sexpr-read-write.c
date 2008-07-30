/*
 *  sexpr-read-write.c
 *  atomic-libc
 *
 *  Created by Magnus Deininger on 15/06/2008.
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

#include <atomic/memory.h>
#include <atomic/sexpr.h>
#include <atomic/io.h>

/*@-nullinit@*/
/* need some sentinel values... */

/*@-mustfreeonly@*/
/* somehow can't seem to write initialisers without this... */

/*@notnull@*/ /*@only@*/ static struct memory_pool *sx_io_pool = (struct memory_pool *)0;

static void sx_write_dispatch (struct sexpr_io *io, struct sexpr *sexpr);

struct sexpr_io *sx_open_io(struct io *in, struct io *out) {
    struct sexpr_io *rv;

    if (sx_io_pool == (struct memory_pool *)0) {
        sx_io_pool = create_memory_pool (sizeof (struct sexpr_io));
    }

    rv = get_pool_mem (sx_io_pool);

    rv->in = in;
    rv->out = out;

    return rv;
}

struct sexpr_io *sx_open_io_fd(int in, int out) {
    return sx_open_io (io_open (in), io_open(out));
}

void sx_close_io (struct sexpr_io *io) {
    io_close (io->in);
    io_close (io->out);

    free_pool_mem (io);
}


static struct sexpr *sx_read_string(const char *buf, int len) {

    int i = 0;
    int cnt = 0;
    char * str[len];
		
    do {
        if(buf[i] == '\\') {
			//escapes		
        }
        else {
            str[cnt] = buf[i];
        }
		
        i++;
        cnt++;
    } while ((buf[i] != '"') && (cnt < len-1));
    str[cnt] = 0;
	
    struct sexpr *retval = make_string((const char*) str);	
	
    return retval;
}

static struct  sexpr *sx_read_integer (char *buf, int len) {
	
    int i = 0;
    signed long nr = 0;
	
    int neg = 0;
	
    if((buf[0] == '-')) {
	
        neg = 1;
        i++;	
    }
	
    do {
        nr += (long) buf[i];
        nr *= 10;	
        i++;
    } while((buf[i] != ' ') && (buf[i] != '\t') && (buf[i] != '\n') && (buf[i] != ')') && i < len);
	
    if(neg) {
        nr *= -1;
    }
	
    struct sexpr *retval = make_integer(nr);
    return retval;
}

struct sexpr *sx_read(struct sexpr_io *io) {
    enum io_result r;

    r = io_read(io->in);
    char* buf = io->in->buffer;
    for(int i = 0; i < io->in->length; i++) {
        switch(buf[i]) {
            case '"':
                return sx_read_string(buf, io->in->length);
                break;
				
            case '(':
				
                break;
			
            case '#':
			
                break;
			
            default:
                if((buf[i] >= '1' && buf[i] <= '9') || (buf[i] == '-')) {
                    return sx_read_integer(buf, io->in->length);
                }
				
                if(buf[i] >= 'A' && buf[i] <= 'z') {
						//symbol
                }
					
                return sx_nonexistent; 
                break;
        }		
    }
		
}



static void sx_write_string_or_symbol (struct io *io, struct sexpr_string_or_symbol *sexpr) {
    int i;

    for (i = 0; sexpr->character_data[i] != 0; i++);

    if (sexpr->type == sxt_string) {
        (void)io_write (io, "\"", 1);
        (void)io_write (io, sexpr->character_data, i);
        (void)io_write (io, "\"", 1);
    } else
        (void)io_write (io, sexpr->character_data, i);
}

static void sx_write_cons (struct sexpr_io *io, struct sexpr_cons *sexpr) {
    (void)io_write (io->out, "(", 1);

  retry:

  sx_write_dispatch (io, car(sexpr));
  sexpr = (struct sexpr_cons *)cdr ((struct sexpr *)sexpr);

  if (sexpr->type == sxt_cons) {
      (void)io_write (io->out, " ", 1);
      goto retry;
  }

  if (sexpr->type != sxt_end_of_list) {
      (void)io_write (io->out, " . ", 3);
      sx_write_dispatch (io, (struct sexpr *)sexpr);
  }

  (void)io_write (io->out, ")", 1);
}

static void sx_write_integer (struct io *io, struct sexpr_integer *sexpr) {
    char num [33];

    int neg = 0;
    signed long i = (signed long)sexpr->integer;
    unsigned int j = 0;

    if(i < 0) {
        neg = 1;
        i *= -1;
    }

    do {
        char s;
        s = '0' + (i % 10);

        num[31-j] = s;

        i /= 10;
        j++;
    } while ((i != 0) && (j < 31));

    if(neg) {
        num[31-j] = '-';
        j++;
    }
    num[32] = 0;

    (void)io_write (io, num+(32 -j), j+1);
}

static void sx_write_dispatch (struct sexpr_io *io, struct sexpr *sexpr)
{
    switch (sexpr->type) {
        case sxt_symbol:
        case sxt_string:
            sx_write_string_or_symbol (io->out, (struct sexpr_string_or_symbol *)sexpr);
            break;

        case sxt_cons:
            sx_write_cons (io, (struct sexpr_cons *)sexpr);
            break;

        case sxt_integer:
            sx_write_integer (io->out, (struct sexpr_integer *)sexpr);
            break;

        case sxt_nil:
            (void)io_write (io->out, "#nil", 4);
            break;
        case sxt_false:
            (void)io_write (io->out, "#f", 2);
            break;
        case sxt_true:
            (void)io_write (io->out, "#t", 2);
            break;
        case sxt_empty_list:
            (void)io_write (io->out, "()", 2);
            break;
        case sxt_end_of_list:
            (void)io_write (io->out, "#eol", 4);
            break;
        case sxt_end_of_file:
            (void)io_write (io->out, "#eof", 4);
            break;
        case sxt_not_a_number:
            (void)io_write (io->out, "#nan", 4);
            break;
        case sxt_nonexistent:
            (void)io_write (io->out, "#ne", 3);
            break;
    }
}

void sx_write(struct sexpr_io *io, struct sexpr *sexpr) {
    sx_write_dispatch(io, sexpr);

    (void)io_write (io->out, "\n", 1);
}
