/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
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

/*! \file
 *  \brief Stack Information
 *
 *  Some generic information to help with certain stack majick thingamajiggies.
 *  The GC needs this.
 */

#ifndef LIBCURIE_STACK_H
#define LIBCURIE_STACK_H

/*!\brief Stack Growth Direction (Date Type)
 *
 * Stacks in computer programmes may grow either upwards or downwards, depending
 * on the computer's architecture. Which direction the stack grows towards needs
 * to be known to programmes that manipulate the stack.
 */
enum stack_growth
{
    sg_down = 0, /*!< The stack grows downwards (from high addresses to low) */
    sg_up   = 1  /*!< The stack grows upwards (from low addresses to high) */
};

/*!\brief Stack Growth Direction
 *
 * This is the actual stack growth direction, as identified by
 * initialise_stack().
 */
extern enum stack_growth stack_growth;

/*!\brief Stack Start Address
 *
 * This is the start address of the stack, or a value near enough, as identified
 * by initialise_stack().
 */
extern void *stack_start_address;

/*! \brief Initialise Stack Boundaries
 *
 *  This function will try to identify the start address of the stack and
 *  whether it grows up or downwards.
 */
void initialise_stack();

#endif
