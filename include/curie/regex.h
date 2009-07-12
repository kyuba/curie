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

/*! \defgroup regex Regular Expressions
 *  \brief Curie's regular expressions under the hood
 *
 *  Curie's regular expression syntax is a bit different from the POSIX or Perl
 *  syntaxes. The supported operators are mostly the same, that is . | ( ) [ ] ?
 *  + and *; escaping (literal character inclusion) is supported using the
 *  \-character. POSIX character classes are not supported.
 *
 *  Unlike the POSIX or Perl variants, Curie's regexen always match the whole
 *  string, not any part of it. That is, the Curie regular expression "a"
 *  matches the string "a", but not the string "blah", whereas a POSIX or Perl
 *  regular expression would match both strings. To get the latter behaviour
 *  with Curie, simply add ".*" before and after the string, i.e. ".*a.*".
 *
 *  Also, Curie's regular expressions are actually "regular", that is they don't
 *  support any context-free grammar features, such as backreferences and the
 *  like.
 *
 *  \section rxImplementation Implementation
 *
 *  The regular expressions are matched using a purely NFA-based approach. No
 *  backtracking algorithm is provided, thus the regular expressions may "only"
 *  be used to match strings, but they're quite fast.
 *
 *  Internally the NFA is represented using a graph.h digraph. Each node
 *  represents an NFA state, the entry point is a node with an sx_nil label,
 *  nodes with an sx_true label are matching states. The graph edges represent
 *  state changes, that is, an edge's label contains a character as its integer
 *  representation, which needs to match the current character for the state to
 *  change from the current node to the edge's target node. sx_false edges are
 *  always followed without advancing the string position. sx_true edges are
 *  also followed unconditionally but the string position is advanced (this is
 *  to implement the '.' operator).
 *
 *  \section rxSxntax Syntax Reference
 *
 *  This applies to the string representation of a regular expression. In its
 *  simplest form, the regular expression is just the string you want to match.
 *  For example, the regular expression "whacky" will match the string "whacky"
 *  and only that string.
 *
 *  The following characters have a special meaning:
 *
 *     - The wildcard, ".", will match any single character. Example: "pix.."
 *       will match the strings "pixie" as well as "pixel" or any other
 *       five-letter word that starts with "pix".
 *     - The alternation operator, "|", will match either the string before or
 *       the one after the operator. Example: "leprechaun|pixie" will either
 *       match the string "leprechaun", or the string "pixie".
 *     - Brackets, "[" and "]", represent character classes. That means that
 *       any character within the "[" and the next "]" matches against the
 *       character in the target string. Example: "pix[ei][el]" will match
 *       "pixie", "pixel", "pixil" and "pixee". Character ranges are also
 *       supported using the "-", for example "[0-9]" will match any string that
 *       is a single digit. Both ranges and literal characters may be mixed
 *       freely.
 *     - Parentheses, "(" and ")", are used to group subexpressions together.
 *       This is most useful for applying the repetition operators to a longer
 *       string, or when using the alternation operator.
 *     - Repetition operators, "+", "*" and "?", specify how often the last
 *       character or subexpression may be repeated. "+" means once-or-more, "?"
 *       means once-or-never. "*" allows the previous character or subexpression
 *       to appear never, once or any number of times. Example: ".+" will match
 *       any string that has at least one character, or in other words any
 *       string but "". "(mad )?hatter" will match the strings "mad hatter" as
 *       well as "hatter". "a+ha!" will match "aha", as well as "aaaaaha!", etc.
 *       ".*" matches any string, including "". You get the idea.
 *     - Literal inclusion of a character is achieved by means of the backslash,
 *       "\". Example: "file.c" will match "file.c" but also "filepc", whereas
 *       "file\.c" will only match "file.c".
 *
 *  \section rxGlobbing Differences to shell patterns/globbing
 *
 *  There's really not that much of a difference between regular expressions and
 *  shell globbing, however for some reason people seem to think the latter to
 *  be very hard to understand and the former to be very easy to use.
 *
 *  The basic differences are the operators. Escaping using the backspace
 *  character and character classes work quite the same as in shell globbing,
 *  but the symbols "*" and "?" work distinctly different. In shell patterns,
 *  "?" is used to denote any single character, whereas "*" is used to denote a
 *  string of arbitrary characters. In regular expressions, you'd write "." and
 *  ".*" instead.
 *
 *  Example: the shell pattern "*.c" would instead be written ".*\.c" as a
 *  regular expression, and the pattern "file??.c" would be written as
 *  "file..\.c".
 *
 *  @{
 */

/*! \file
 *  \brief Regular Expression Library
 *
 *  This header file describes the functions that implement curie's regular
 *  expressions.
 *
 *  \see regex
 */

#ifndef LIBCURIE_REGEX_H
#define LIBCURIE_REGEX_H

#include <curie/graph.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Compile a Regular Expression
 *  \param[in] rx The regular expression in string form, or an NFA graph in
 *                s-expression form.
 *  \return Generated NFA graph.
 *
 *  This function is used to turn a regular expression into a finite-state
 *  machine for the rx_match() and rx_match_rx() functions. Syntax errors aren't
 *  really checked for and the usually result in an NFA that won't match
 *  anything.
 */
sexpr rx_compile_sx (sexpr rx);

/*! \brief Compile a Regular Expression
 *  \param[in] rx The regular expression as a C string.
 *  \return Generated NFA graph.
 *
 *  Same as rx_compile_sx, but its argument is a C string instead of an
 *  s-expression.
 */
sexpr rx_compile    (const char *rx);

/*! \brief Match String
 *  \param[in] rx The NFA graph to use for the matching.
 *  \param[in] s  The string to match against.
 *  \return sx_true for a match, sx_false otherwise.
 *
 *  This function simulates the NFA graph and applies it to the given string.
 *  If the NFA matches the string, sx_true is returned, otherwise it's sx_false.
 */
sexpr rx_match_sx           (sexpr rx, sexpr s);

/*! \brief Match String
 *  \param[in] rx The NFA graph to use for the matching.
 *  \param[in] s  The C string to match against.
 *  \return sx_true for a match, sx_false otherwise.
 *
 *  Same as rx_match_sx, but the string is a C-style string instead of an
 *  s-expression.
 */
sexpr rx_match              (sexpr rx, const char *s);

#ifdef __cplusplus
}
#endif

#endif

/*! @} */
