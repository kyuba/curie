/*
 *  regex.c
 *  libcurie
 *
 *  Created by Magnus Deininger on 20/12/2008.
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

#include <curie/regex.h>
#include <curie/memory.h>

static void rx_compile_add_nodes (struct graph *g, const char *s)
{
    unsigned int p = 0;
    char quote = 0, cclass = 0;

    while (s[p] != 0)
    {
        if (quote)
        {
            quote = 0;
            goto add;
        }

        if (cclass)
        {
            if (s[p] == ']')
            {
                cclass = 0;
            }
            p++;
            continue;
        }

        switch (s[p])
        {
            case '\\':
                quote = 1;
                break;
            case '?':
            case '*':
            case '+':
            case '(':
            case ')':
            case '|':
                break;
            case '[':
                cclass = 1;
            default:
                add:
                graph_add_node (g, make_integer(p));
                break;
        }

        p++;
    }
}

static struct graph_node *rx_compile_recurse
    (struct graph *g, struct graph_node *n, struct graph_node *e, const char *s,
     unsigned int *pp)
{
    unsigned int p = *pp;
    char quote = 0;
    struct graph_node *c = n, *last = (struct graph_node *)0;
    sexpr el = truep(e->label) ? make_integer(0) : sx_false;

    while (s[p] != 0)
    {
        if (quote)
        {
            quote = 0;
            goto add;
        }

        switch (s[p])
        {
            case '\\':
                quote = 1;
                break;
            case '[':
                {
                    struct graph_node *t =
                            graph_search_node (g, make_integer(p));
                    char lastchar = 0, range = 0;

                    last = c;
                    c = t;

                    while (s[p] != 0)
                    {
                        char cc = s[p];

                        if (cc == ']') break;

                        if (range == 1)
                        {
                            for (char n = lastchar; n < cc; n++)
                            {
                                graph_node_add_edge (last, t, make_integer(n));
                            }

                            lastchar = cc;
                            range = 0;
                            p++;
                            continue;
                        } else if (cc == '-') {
                            lastchar++;
                            range = 1;
                            p++;
                            continue;
                        }

                        graph_node_add_edge (last, t, make_integer(cc));

                        lastchar = cc;
                        p++;
                    }

                    if (s[p] == 0)
                    {
                        goto quit;
                    }
                }
                break;
            case '?':
                if (last != (struct graph_node *)0);
                {
                    struct graph_node *ne = graph_add_node (g, sx_false);

                    graph_node_add_edge (c, ne, sx_false);
                    graph_node_add_edge (last, ne, sx_false);

                    last = c;
                    c = ne;
                }
                break;
            case '+':
                if (last != (struct graph_node *)0);
                {
                    struct graph_node *ne = graph_add_node (g, sx_false);

                    graph_node_add_edge (c, last, sx_false);
                    graph_node_add_edge (c, ne, sx_false);

                    last = c;
                    c = ne;
                }
                break;
            case '*':
                if (last != (struct graph_node *)0);
                {
                    struct graph_node *ne = graph_add_node (g, sx_false);

                    graph_node_add_edge (c, ne, sx_false);
                    graph_node_add_edge (last, ne, sx_false);
                    graph_node_add_edge (c, last, sx_false);

                    last = c;
                    c = ne;
                }
                break;
            case '(':
                {
                    struct graph_node *ne = graph_add_node (g, sx_false);

                    p++;

                    last = c;
                    c = rx_compile_recurse (g, c, ne, s, &p);
                }
                goto next;
            case ')':
                p++;
                el = sx_false;
                goto quit;
            case '|':
                {
                    struct graph_node *ne = graph_add_node (g, sx_false);

                    p++;

                    graph_node_add_edge (c, ne, sx_false);

                    last = c;
                    c = ne;

                    rx_compile_recurse (g, n, ne, s, &p);
                    goto quit;
                }
                break;
            case '.':
                {
                    struct graph_node *t =
                            graph_search_node (g, make_integer(p));

                    graph_node_add_edge (c, t, sx_true);

                    last = c;
                    c = t;
                }
                break;
            default:
                add:
                {
                    struct graph_node *t =
                            graph_search_node (g, make_integer(p));

                    graph_node_add_edge (c, t, make_integer(s[p]));

                    last = c;
                    c = t;
                }
                break;
        }

        p++;

        next:;
    }

    quit:
    *pp = p;
    graph_node_add_edge (c, e, el);
    return c;
}

struct graph *rx_compile (sexpr sx)
{
    struct graph *g;

    if (consp(sx))
    {
        g = sexpr_to_graph(sx);
    }
    else
    {
        g = graph_create();

        if (stringp(sx)) {
            const char *s = sx_string(sx);
            unsigned int p = 0;
            struct graph_node *n = graph_add_node (g, sx_nil);
            struct graph_node *e = graph_add_node (g, sx_true);

            rx_compile_add_nodes(g, s);
            (void)rx_compile_recurse(g, n, e, s, &p);
        }
    }

    return g;
}

struct nfa_state
{
    struct graph_node *n;
    unsigned int p;
    struct nfa_state *next;
};

static struct memory_pool nfa_pool =
        MEMORY_POOL_INITIALISER (sizeof(struct nfa_state));

static void rx_match_add_nfa_state
    (struct nfa_state **s, struct graph_node *n, unsigned int p)
{
    /* don't add nfa states that are already in the list */
    for (struct nfa_state *c = *s; c != (struct nfa_state *)0; c = c->next)
    {
        if ((c->n == n) && (c->p == p)) return;
    }

    struct nfa_state *ns = get_pool_mem (&nfa_pool);

    ns->n = n;
    ns->p = p;

    ns->next = (*s);
    *s = ns;
}

static sexpr rx_match_nfa_state_progress
    (struct nfa_state *ns, struct nfa_state **r, const char *s)
{
    unsigned int p = ns->p;
    struct graph_node *n = ns->n;
    char haveedge = 0;

    if (truep(n->label)) return sx_true;

    sexpr sx = make_integer(s[p]);

    for (unsigned int i = 0; i < n->edge_count; i++)
    {
        struct graph_edge *e = n->edges[i];
        sexpr l = e->label;

        if (haveedge)
        {
            if (falsep(l))
            {
                rx_match_add_nfa_state (r, e->target, p);
            }
            else if (((s[p] != 0) && truep(l)) || truep(equalp(l, sx)))
            {
                struct graph_node *nt = e->target;

                rx_match_add_nfa_state (r, nt, p + ((s[p] == 0) ? 0 : 1));
            }
        }
        else
        {
            if (falsep(l))
            {
                ns->n = e->target;

                haveedge = 1;
            }
            else if (((s[p] != 0) && truep(l)) || truep(equalp(l, sx)))
            {
                ns->n = e->target;

                if (s[p] != 0) ns->p = p + 1;

                haveedge = 1;
            }
        }
    }

    if (!haveedge)
    {
        return sx_nonexistent;
    }

    return sx_false;
}

static sexpr rx_match_recurse
    (struct nfa_state **ns, const char *s)
{
    while ((*ns) != (struct nfa_state *)0)
    {
        struct nfa_state *c = (*ns);

        while (c != (struct nfa_state *)0)
        {
            sexpr sx = rx_match_nfa_state_progress (c, ns, s);

            if (truep (sx))
            {
                return sx_true;
            }
            else if (nexp (sx))
            {
                struct nfa_state *f = c;

                c = c->next;

                free_pool_mem (f);

                if ((*ns) == f)
                {
                    *ns = c;
                }
                else for (struct nfa_state *y = *ns;
                          y != (struct nfa_state *)0;
                          y = y->next)
                {
                    if (y->next == f)
                    {
                        y->next = c;
                        break;
                    }
                }
            }
            else
            {
                c = c->next;
            }
        }
    }

    return sx_false;
}

sexpr rx_match (struct graph *g, sexpr sx)
{
    sexpr rv = sx_false;

    if ((g != (struct graph *)0) && stringp(sx))
    {
        const char *s = sx_string(sx);
        struct graph_node *n = graph_search_node (g, sx_nil);
        struct nfa_state *ns = get_pool_mem (&nfa_pool);

        if (n == (struct graph_node *)0) return sx_false;

        if (truep(n->label)) return sx_true;

        ns->next = (struct nfa_state *)0;
        ns->n = n;
        ns->p = 0;

        rv = rx_match_recurse (&ns, s);

        while (ns != (struct nfa_state *)0)
        {
            struct nfa_state *f = ns;
            ns = ns->next;

            free_pool_mem (f);
        }
    }

    return rv;
}

void rx_free (struct graph *g)
{
    graph_destroy (g);
}
