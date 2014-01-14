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

#include <syscall/syscall.h>
#include <curie/attributes.h>
#include <curie/main.h>
#include <curie/stack.h>

char **curie_argv        = 0;
char **curie_environment = 0;

void __start ( void ) __attribute__((used,noreturn));
void __asm_cexit ( int ) __attribute__((used,alias("cexit")));
int  jump_cmain ( void ) __attribute__((flatten));

int __asm_cmain ( void ) __attribute__((used,alias("jump_cmain")));

int jump_cmain ( void )
{
    return cmain ();
}

void cexit (int status)
{
    sys_exit (status);
    UNREACHABLE
}

void __start ( void )
{
    __asm__ volatile
        (".globl _start\n\t"
         ".type _start, @function\n"
         "_start:\n\t"
         "xorq  %%rbp, %%rbp\n\t"
         "movq  %%rsp, %2\n\t"
         "movq  %%rsp, %%r11\n\t"
         "addq  $0x8, %%r11\n\t"
         "movq  %%r11, %0\n\t"
         "movq  (%%rsp), %%rbx\n\t"
         "incq  %%rbx\n\t"
         "imulq $0x8, %%rbx, %%rbx\n\t"
         "addq  %%rbx, %%r11\n\t"
         "movq  %%r11, %1\n\t"
         "call __asm_cmain\n\t"
         "movq %%rax, %%rdi\n\t"
         "call __asm_cexit"
         : "=m"(curie_argv), "=m"(curie_environment), "=m"(stack_start_address)
         : "p"(cexit), "p"(cmain)
         : "memory" );
    UNREACHABLE
}
