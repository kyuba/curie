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

#include <curie/io-system.h>
#include <curie/memory.h>
#include <curie/io.h>
#include <curie/exec.h>
#include <curie/exec-system.h>
#include <curie/network.h>
#include <windows.h>

struct exec_context *execute(unsigned int options,
                             char **command,
                             char **environment)
{
    static struct memory_pool
            pool = MEMORY_POOL_INITIALISER(sizeof(struct exec_context));
    struct exec_context *context =
            (struct exec_context *)get_pool_mem(&pool);
    struct io *proc_stdout_in, *proc_stdout_out,
              *proc_stdin_in,  *proc_stdin_out;

    if (context == (struct exec_context *)0)
    {
        return (struct exec_context *)0;
    }
    
    if (command == (char **)0)
    {
        context->pid = -1;
    }
    else
    {
        char *av;
        int alength = 0, elength = 0;
        char **argv = command;
        char *envx = (char *)0;
        for (int i = 0; argv[i]; i++)
        {
            for (int j = 0; argv[i][j]; j++)
            {
                alength++;
            }
            alength++;
        }
    
        av = aalloc (alength);
        alength = 0;

        for (int i = 0; argv[i]; i++)
        {
            if (alength > 0) av[alength-1] = ' ';
            for (int j = 0; argv[i][j]; j++)
            {
                av[alength] = argv[i][j];
                alength++;
            }
            av[alength] = 0;
            alength++;
        }
        
        if (environment != (char **)0)
        {
            for (int i = 0; environment[i]; i++)
            {
                for (int j = 0; environment[i][j]; j++)
                {
                    elength++;
                }
                elength++;
            }
            
            envx = aalloc (elength + 1);

            elength = 0;
            for (int i = 0; environment[i]; i++)
            {
                for (int j = 0; environment[i][j]; j++)
                {
                    envx[elength] = environment[i][j];
                    elength++;
                }
                envx[elength] = 0;
                elength++;
            }
            envx[elength] = 0;
            elength++;
        }
       
        STARTUPINFO s;
        PROCESS_INFORMATION p;
        memset (&s, 0, sizeof (s));
        s.cb = sizeof (s);
        memset (&p, 0, sizeof (p));
        
        if (CreateProcessA(command[0], av, (void *)0, (void *)0, FALSE,
                           /* CREATE_NEW_CONSOLE */ 0,
                           envx, (void *)0, &s, &p))
        {
            context->pid = p.dwProcessId;
            CloseHandle (p.hProcess);
            CloseHandle (p.hThread);
        }
        else
        {
            context->pid = -1;
        }
    }
    
    return context;
}

void free_exec_context (struct exec_context *context) {
    free_pool_mem ((void *)context);
}

void check_exec_context (struct exec_context *context) {
/*    int i;

    switch (context->pid) {
        case 0:
        case -1:
            return;
        default:
            if ((context->status == ps_running) &&
                (a_wait (context->pid, &i) != wr_running)) {
                context->exitstatus = i;
                context->status = ps_terminated;
            }
            return;
    }*/
}
