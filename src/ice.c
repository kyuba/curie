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

#include <icemake/icemake.h>
#include <curie/main.h>

#if !defined(NOVERSION)
#include <curie/version.h>
#include <sievert/version.h>
#include <icemake/version.h>
#include <ice/version.h>
#endif

#define ICEMAKE_OPTION_VIS_ICE         (1 << 0x10)
#define ICEMAKE_OPTION_VIS_RAW         (1 << 0x11)

struct icemake_meta
{
    sexpr buildtargets;
    enum fs_layout filesystem_layout;
    unsigned long options;
    unsigned int max_processes;
    sexpr alternatives;
};

static void print_version_wo (struct io *out)
{
#if defined(NOVERSION)
    io_write (out, "icemake (version info not available\n",
              sizeof ("icemake (version info not available"));
#else
    io_collect (out, ice_version_long "\n"
                     icemake_version_long "\n"
                     curie_version_long "\n"
                     sievert_version_long "\n",
                sizeof (ice_version_long) +
                sizeof (icemake_version_long) +
                sizeof (curie_version_long) +
                sizeof (sievert_version_long));
#endif
}

static void print_help ()
{
    struct io *out = io_open_stdout();

    print_version_wo (out);

#define HELP "\nUsage: ice [options] [targets]\n"\
        "\n"\
        "Options:\n"\
        " -h           Print help (this text) and exit.\n"\
        " -v           Print version and exit.\n\n"\
        " -t <chost>   Specify target CHOST\n"\
        " -d <destdir> Specify the directory to install to.\n"\
        " -i           Install resulting binaries\n"\
        " -r           Execute runtime tests\n"\
        " -f           Use the FHS layout for installation\n"\
        " -l <libdir>  Use <libdir> instead of 'lib' when installing\n"\
        " -s           Use the default FS layout for installation\n"\
        " -L           Optimise linking.\n"\
        " -c           Use gcc's -combine option for C source files.\n"\
        " -o           Don't link dynamic libraries.\n"\
        " -O           Do link dynamic libraries.\n"\
        " -S           Enforce a static link (default).\n"\
        " -R           Enforce a dynamic link.\n"\
        " -j <num>     Spawn <num> processes simultaneously.\n"\
        " -a <1> <2>   Use implementation <2> for code part <1>.\n"\
        " -x           Build documentation (if possible).\n"\
        " -m           Use raw output.\n"\
        " -M           Use nicer output (default).\n"\
        "\n"\
        "The [targets] specify a list of things to build, according to the\n"\
        "icemake.sx file located in the current working directory.\n\n"

    io_collect (out, HELP,
                sizeof (HELP) - 1);

    io_close (out);
}

static void print_version ()
{
    struct io *out = io_open_stdout();

    print_version_wo (out);

    io_close (out);
}

static int with_icemake (struct icemake *im, void *aux)
{
    struct icemake_meta *imc = (struct icemake_meta *)aux;

    im->filesystem_layout  = imc->filesystem_layout;
    im->buildtargets       = imc->buildtargets;

    im->max_processes      = imc->max_processes;

    if (im->options & ICEMAKE_OPTION_VIS_ICE)
    {
        icemake_prepare_visualiser_ice (im);
        im->options &= ~ICEMAKE_OPTION_VIS_ICE;
    }

    if (im->options & ICEMAKE_OPTION_VIS_RAW)
    {
        icemake_prepare_visualiser_raw (im);
        im->options &= ~ICEMAKE_OPTION_VIS_RAW;
    }

    return icemake (im);
}

static int with_toolchain (struct toolchain_descriptor *td, void *aux)
{
    struct icemake_meta *im = (struct icemake_meta *)aux;

    return icemake_prepare
        ((struct icemake *)0, ".", td, im->options, im->alternatives,
         with_icemake, (void *)im);
}

static int with_architecture (const char *arch, void *aux)
{
    return icemake_prepare_toolchain
        (arch, with_toolchain, aux);
}

int cmain ()
{
    int i = 1;
    const char *target_architecture = (const char *)0;
    struct icemake_meta im =
        { sx_end_of_list, fs_afsl,
          ICEMAKE_OPTION_STATIC | ICEMAKE_OPTION_DYNAMIC_LINKING |
          ICEMAKE_OPTION_VIS_ICE,
          1, sx_end_of_list };

    initialise_icemake ();

    mkdir ("build", 0755);

    i_destlibdir = str_lib;

    while (curie_argv[i])
    {
        if (curie_argv[i][0] == '-')
        {
            int y = 1;
            int xn = i + 1;
            while (curie_argv[i][y] != 0)
            {
                switch (curie_argv[i][y])
                {
                    case 't':
                        if (curie_argv[xn])
                        {
                            target_architecture = curie_argv[xn];
                            xn++;
                        }
                        break;
                    case 'd':
                        if (curie_argv[xn])
                        {
                            i_destdir = make_string(curie_argv[xn]);
                            xn++;
                        }
                        break;
                    case 'i':
                        im.options |=  ICEMAKE_OPTION_INSTALL;
                        break;
                    case 'L':
                        im.options |=  ICEMAKE_OPTION_OPTIMISE_LINKING;
                        break;
                    case 'c':
                        im.options |=  ICEMAKE_OPTION_COMBINE;
                        break;
                    case 'S':
                        im.options |=  ICEMAKE_OPTION_STATIC;
                        break;
                    case 'R':
                        im.options &= ~ICEMAKE_OPTION_STATIC;
                        break;
                    case 'x':
                        do_build_documentation = sx_true;
                        break;
                    case 'f':
                        im.filesystem_layout = fs_fhs;
                        break;
                    case 'o':
                        im.options &= ~ICEMAKE_OPTION_DYNAMIC_LINKING;
                        break;
                    case 'O':
                        im.options |=  ICEMAKE_OPTION_DYNAMIC_LINKING;
                        break;
                    case 's':
                        im.filesystem_layout = fs_afsl;
                        break;
                    case 'l':
                        if (curie_argv[xn])
                        {
                            i_destlibdir = make_string(curie_argv[xn]);
                            xn++;
                        }
                        break;
                    case 'a':
                        if (curie_argv[xn] && curie_argv[(xn + 1)])
                        {
                            im.alternatives
                                = cons (cons(make_symbol(curie_argv[xn]),
                                             make_string(curie_argv[(xn + 1)])),
                                        im.alternatives);

                            xn += 2;
                        }
                        break;
                    case 'j':
                        if (curie_argv[xn])
                        {
                            char *s = curie_argv[xn];
                            unsigned int j;

                            im.max_processes = 0;

                            for (j = 0; s[j]; j++)
                            {
                                im.max_processes =
                                    10 * im.max_processes + (s[j]-'0');
                            }

                            xn++;
                        }

                        if (im.max_processes == 0)
                        {
                            im.max_processes = 1;
                        }
                        break;
                    case 'r':
                        im.options |=  ICEMAKE_OPTION_TESTS;
                        break;
                    case 'm':
                        im.options &= ~ICEMAKE_OPTION_VIS_RAW;
                        im.options |=  ICEMAKE_OPTION_VIS_ICE;
                        break;
                    case 'M':
                        im.options &= ~ICEMAKE_OPTION_VIS_ICE;
                        im.options |=  ICEMAKE_OPTION_VIS_RAW;
                        break;
                    case 'v':
                        print_version ();
                        return 0;
                    case 'h':
                    case '-':
                        print_help (curie_argv[0]);
                        return 0;
                }

                y++;
            }

            i = xn;
        }
        else
        {
            im.buildtargets =
                sx_set_add (im.buildtargets, make_string (curie_argv[i]));
            i++;
        }
    }

    if (target_architecture != (const char *)0)
    {
        return with_architecture
            (target_architecture, (void *)&im);
    }
    else
    {
        return icemake_default_architecture
            (with_architecture,   (void *)&im);
    }

}
