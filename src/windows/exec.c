/*
 * This file is part of the kyuba.org Curie project.
 * See the appropriate repository at http://git.kyuba.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2010, Kyuba Project Members
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

    context->in = (struct io *)0;
    context->out = (struct io *)0;

    if ((options & EXEC_CALL_NO_IO) == 0) {
        net_open_loop(&proc_stdout_in, &proc_stdout_out);
        net_open_loop(&proc_stdin_in, &proc_stdin_out);
    }

    context->status = ps_running;
    
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
        char *lastbs;
        char *nq;
        int i, j, argvsize, envvsize, nqsize;
        STARTUPINFO s;
        PROCESS_INFORMATION p;

        for (i = 0; command[i]; i++); i++;
        argvsize = sizeof (char *) * i;
        argv = aalloc (argvsize);
        nqsize = i;
        nq = aalloc (nqsize);

        for (i = 0; command[i]; i++)
        {
            argv[i] = command[i];
        }
        argv[i] = (char *)0;

        for (i = 0, lastbs = argv[0]; argv[0][i]; i++)
        {
            if (argv[0][i] == '\\')
            {
                lastbs = argv[0] + i + 1;
            }
        }
        argv[0] = lastbs;        

        for (i = 0; argv[i]; i++)
        {
            nq[i] = 0;

            for (j = 0; argv[i][j]; j++)
            {
                if ((argv[i][j] == ' ') || (argv[i][j] == '\\'))
                {
                    nq[i] = 1;
                }

                if (argv[i][j] == '"')
                {
                    alength++;
                }

                alength++;
            }

            if (nq[i])
            {
                 /* one for the trailing space, two for the quotes */
                alength += 3;
            }
            else
            {
                alength++;
            }
        }
    
        av = aalloc (alength);
        alength = 0;

        for (i = 0; argv[i]; i++)
        {
            if (alength > 0) av[alength-1] = ' ';

            if (nq[i])
            {
                av[alength] = '"';
                alength++;
            }

            for (j = 0; argv[i][j]; j++)
            {
                if (argv[i][j] == '"')
                {
                    av[alength] = '\\';
                    alength++;
                }

                av[alength] = argv[i][j];
                alength++;
            }

            if (nq[i])
            {
                av[alength] = '"';
                alength++;
            }
            av[alength] = 0;
            alength++;
        }

        if (environment != (char **)0)
        {
            for (i = 0; environment[i]; i++)
            {
                for (j = 0; environment[i][j]; j++)
                {
                    elength++;
                }
                elength++;
            }

            envvsize = elength + 1;
            envx = aalloc (envvsize);

            elength = 0;
            for (i = 0; environment[i]; i++)
            {
                for (j = 0; environment[i][j]; j++)
                {
                    envx[elength] = environment[i][j];
                    elength++;
                }
                envx[elength] = 0;
                elength++;
            }
            envx[elength] = 0;
        }
       
        memset (&s, 0, sizeof (s));
        s.cb = sizeof (s);
        memset (&p, 0, sizeof (p));

        if (((options & EXEC_CALL_NO_IO) == 0) &&
            (proc_stdin_out != (void *)0) && (proc_stdout_in != (void *)0))
        {
            s.dwFlags    |= STARTF_USESTDHANDLES;
            s.hStdInput   = proc_stdin_out->handle;
            s.hStdOutput  = proc_stdout_in->handle;
            s.hStdError   = proc_stdout_in->handle;
        }

        if (CreateProcessA(command[0], av, (void *)0, (void *)0, FALSE,
                           /* CREATE_NEW_CONSOLE */ 0,
                           envx, (void *)0, &s, &p))
        {
            context->pid    = p.dwProcessId;
            context->handle = p.hProcess;
/*            CloseHandle (p.hProcess);*/
            CloseHandle (p.hThread);
            
            if ((options & EXEC_CALL_NO_IO) == 0) {
                io_close (proc_stdout_in);
                io_close (proc_stdin_out);

                context->in = proc_stdin_in;
                context->out = proc_stdout_out;
            }
        }
        else
        {
            if ((options & EXEC_CALL_NO_IO) == 0) {
                io_close (proc_stdout_in);
                io_close (proc_stdout_out);
                io_close (proc_stdin_in);
                io_close (proc_stdin_out);
            }

            context->in = (struct io *)0;
            context->out = (struct io *)0;
            context->status = ps_terminated;
            context->handle = (void *)0;
            context->pid = -1;
        }

        afree (argvsize, argv);
        afree (nqsize,   nq);
        if (environment != (char **)0)
        {
            afree (envvsize, envx);
        }
    }
    
    return context;
}

void free_exec_context (struct exec_context *context) {
    if (context->handle != (void *)0)
    {
        CloseHandle (context->handle);
    }

    free_pool_mem ((void *)context);
}

void check_exec_context (struct exec_context *context) {
    switch (context->pid) {
        case 0:
        case -1:
            return;
        default:
            if (context->handle != (void *)0)
            {
                GetExitCodeProcess (context->handle, &(context->exitstatus));
                if (context->exitstatus != 259 /* STILL_ALIVE */)
                {
                    context->status = ps_terminated;
                    CloseHandle (context->handle);
                    context->handle = (void *)0;
                }
            }
            return;
    }
}
