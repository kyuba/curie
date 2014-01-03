/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2014, Kyuba Project Members
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

#include <sievert/sexpr.h>

sexpr sx_list1
    (sexpr alpha)
{
    return cons (alpha, sx_end_of_list);
}

sexpr sx_list2
    (sexpr alpha, sexpr beta)
{
    return cons (alpha,
                 sx_list1 (beta));
}

sexpr sx_list3
    (sexpr alpha, sexpr beta, sexpr gamma)
{
    return cons (alpha,
                 sx_list2 (beta, gamma));
}

sexpr sx_list4
    (sexpr alpha, sexpr beta, sexpr gamma, sexpr delta)
{
    return cons (alpha,
                 sx_list3 (beta, gamma, delta));
}

sexpr sx_list5
    (sexpr alpha, sexpr beta, sexpr gamma, sexpr delta, sexpr epsilon)
{
    return cons (alpha,
                 sx_list4 (beta, gamma, delta, epsilon));
}

sexpr sx_list6
    (sexpr alpha, sexpr beta, sexpr gamma, sexpr delta, sexpr epsilon,
     sexpr zeta)
{
    return cons (alpha,
                 sx_list5 (beta, gamma, delta, epsilon, zeta));
}
