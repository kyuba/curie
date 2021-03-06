/**\file
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

#include <curie/memory.h>
#include <curie/sexpr.h>
#include <curie/sexpr-internal.h>
#include <curie/string.h>
#include <curie/gc.h>
#include <curie/tree.h>
#include <curie/hash.h>
#include <curie/math.h>

static struct tree sx_cons_tree     = TREE_INITIALISER;
static struct tree sx_string_tree   = TREE_INITIALISER;
static struct tree sx_symbol_tree   = TREE_INITIALISER;
static struct tree sx_rational_tree = TREE_INITIALISER;
unsigned long gc_base_items         = 0;

sexpr cons(sexpr sx_car, sexpr sx_cdr)
{
    static struct memory_pool pool =
            MEMORY_POOL_INITIALISER(sizeof (struct sexpr_cons));
    struct sexpr_cons *rv;
    struct tree_node *n;
    sexpr t[2];
    int_pointer hash;

    t[0] = sx_car;
    t[1] = sx_cdr;

    hash = hash_murmur2_pt (t, sizeof(t), 0);

    if ((n = tree_get_node (&sx_cons_tree, (int_pointer)hash)))
    {
        return (sexpr)node_get_value (n);
    }

    rv = get_pool_mem (&pool);

    rv->type = sxt_cons;
    rv->car  = sx_car;
    rv->cdr  = sx_cdr;

    tree_add_node_value (&sx_cons_tree, hash, (void *)rv);

    gc_base_items++;

    return (sexpr)rv;
}

sexpr make_rational(int_pointer p, int_pointer_s q)
{
    static struct memory_pool pool =
            MEMORY_POOL_INITIALISER(sizeof (struct sexpr_rational));
    struct sexpr_rational *rv;
    struct tree_node *n;
    int_64 g = gcd (p, q >= 0 ? q : (q * -1));
    int_pointer t[2], hash;

    p /= g;
    q /= g;

    if (q == 1)
    {
        return make_integer (p);
    }
    else if (q == -1)
    {
        return make_integer (p * -1);
    }

    t[0] = p;
    t[1] = q;
    hash = hash_murmur2_pt (t, sizeof(t), 0);

    if ((n = tree_get_node (&sx_rational_tree, (int_pointer)hash)))
    {
        return (sexpr)node_get_value (n);
    }

    rv = get_pool_mem (&pool);

    rv->type        = sxt_rational;
    rv->numerator   = p;
    rv->denominator = q;

    tree_add_node_value (&sx_rational_tree, hash, (void *)rv);

    gc_base_items++;

    return (sexpr)rv;
}

static sexpr make_string_or_symbol_lh
    (const char *string, char symbol, int_pointer hash, unsigned long len)
{
    struct sexpr_string_or_symbol *s;
    unsigned int i;
    struct tree_node *n;

    if ((n = tree_get_node ((symbol == (char)1) ? &sx_symbol_tree
                                                : &sx_string_tree,
                            (int_pointer)hash)))
    {
        return (sexpr)node_get_value (n);
    }

    s = aalloc (sizeof (struct sexpr_string_or_symbol) + len + 1);

    tree_add_node_value ((symbol == (char)1) ? &sx_symbol_tree
                                             : &sx_string_tree,
                         (int_pointer)hash, s);

    for (i = 0; i < len; i++)
    {
        s->character_data[i] = string[i];
    }
    s->character_data[i] = (char)0;

    s->type = (symbol == (char)1) ? sxt_symbol : sxt_string;

    gc_base_items++;

    return (sexpr)s;
}

static sexpr make_string_or_symbol
    (const char *string, char symbol)
{
    unsigned long len;
    int_pointer hash = str_hash (string, &len);

    return make_string_or_symbol_lh (string, symbol, hash, len);
}

static sexpr make_string_or_symbol_l
    (const char *string, char symbol, unsigned long len)
{
    int_pointer hash = hash_murmur2_pt (string, len, 0);

    return make_string_or_symbol_lh (string, symbol, hash, len);
}

sexpr make_string
    (const char *string)
{
    return make_string_or_symbol (string, (char)0);
}

sexpr make_symbol
    (const char *symbol)
{
    return make_string_or_symbol (symbol, (char)1);
}

sexpr make_string_l
        (const char *string, unsigned long length)
{
    return make_string_or_symbol_l (string, (char)0, length);
}

sexpr make_symbol_l
        (const char *symbol, unsigned long length)
{
    return make_string_or_symbol_l (symbol, (char)1, length);
}

void sx_destroy(sexpr sxx)
{
    if (!pointerp(sxx)) return;

    if (stringp(sxx) || symbolp(sxx))
    {
        struct sexpr_string_or_symbol *sx
                = (struct sexpr_string_or_symbol *)sx_pointer(sxx);

        unsigned long length = 0;
        int_32 hash;
        struct tree_node *n;

        hash = str_hash (((struct sexpr_string_or_symbol *)sx)->character_data,
                         &length);

        if ((n = tree_get_node ((sx->type == sxt_string) ? &sx_string_tree
                                                         : &sx_symbol_tree,
                                (int_pointer)hash)))
        {
            tree_remove_node ((sx->type == sxt_string) ? &sx_string_tree
                                                       : &sx_symbol_tree,
                              (int_pointer)hash);

            afree ((sizeof (struct sexpr_string_or_symbol) + length + 1), sx);
            gc_base_items--;
        }
    }
    else if (consp(sxx))
    {
        struct sexpr_cons *sx = (struct sexpr_cons *)sx_pointer(sxx);
        sexpr t[2];
        int_pointer hash;

        t[0] = sx->car;
        t[1] = sx->cdr;
        
        hash = hash_murmur2_pt (t, sizeof(t), 0);
        
        tree_remove_node (&sx_cons_tree, hash);

        free_pool_mem (sx);
        gc_base_items--;
    }
    else if (rationalp(sxx))
    {
        struct sexpr_rational *sx = (struct sexpr_rational *)sx_pointer(sxx);
        int_pointer t[2];
        int_pointer hash;

        t[0] = sx->numerator;
        t[1] = sx->denominator;

        hash = hash_murmur2_pt (t, sizeof(t), 0);

        tree_remove_node (&sx_rational_tree, hash);

        free_pool_mem (sx);
        gc_base_items--;
    }
    else if (customp(sxx))
    {
        int type = sx_type (sxx);
        struct sexpr_type_descriptor *d = sx_get_descriptor (type);

        if ((d->destroy != (void *)0))
        {
            d->destroy (sxx);
        }
    }
}

static void sx_map_call (struct tree_node *node, void *u)
{
    sexpr sx = (sexpr)node_get_value(node);

    gc_call (sx);
}

void sx_call_all ( void )
{
    tree_map (&sx_cons_tree,     sx_map_call, (void *)0);
    tree_map (&sx_string_tree,   sx_map_call, (void *)0);
    tree_map (&sx_symbol_tree,   sx_map_call, (void *)0);
    tree_map (&sx_rational_tree, sx_map_call, (void *)0);
}

static sexpr sx_integer_to_string (int_pointer_s i)
{
    char num [SX_MAX_NUMBER_LENGTH];

    int neg = 0;
    unsigned int j = 1;

    num[(SX_MAX_NUMBER_LENGTH-2)] = 0;

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

    return make_string (num + ((SX_MAX_NUMBER_LENGTH - 1) - j));
}

sexpr sx_to_string (sexpr a)
{
    if (integerp (a))
    {
        return sx_integer_to_string (sx_integer (a));
    }

    return a;
}
