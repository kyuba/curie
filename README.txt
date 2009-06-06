###- QUICK AND DIRTY: COMPILATION AND INSTALLATION -############################
  You need to have either mingw's, bcc32's or msvc++'s compiler binaries in your
  PATH, then do this on a cmd.exe console:

    build.cmd -dif c:\

################################################################################


NOTE:
  Library documentation has gotten somewhat better, but this new build system
  could have made things a tiny bit trickier. This has been tested on
  linux/x86-64, linux/powerpc, darwin/powerpc and windows/x86 though.

NOTE.2:
  Please take note of the library licence (the COPYING file), as well as the
  credits (the CREDITS file).

NOTE.3:
  This is the description for Windows systems. For details on how to get this
  running on POSIX-ish systems, see README.

###- DETAILS -##################################################################

Getting icemake:
  On windows systems it might be advantageous to use a precompiled icemake
  programme instead of the default build script which would bootstrap icemake.
  You may obtain a recent version of this programme (compiled with MSVC++) at:

    http://kyuba.org/icemake/

Compilation:
  If you have icemake installed already, for whatever reason that may be, just
  call:

    icemake

  otherwise (which should be the most common case), use:

    build.cmd

  This will compile the library itself. Either command would need to be issued
  from a shell window (cmd.exe, DOS prompt, whichever terminology you prefer),
  and you need to be in the directory where the curie sources had been extracted
  to (use the "cd" command to do this).

  A number of compilation options are available if you run into issues. To see
  them, use one of these:

    icemake --help
  -- or --
    build.cmd --help

  The compiler will try to determine the compiler to use automatically,
  depending on what's available in your PATH. The search order is MSVC++, then
  Borland/Turbo C++, then GCC/MinGW. Do note that only the Borland compiler will
  put itself into the computer's PATH automatically; for MSVC++ you may invoke
  icemake from the MSVC command line (there should be a link to this in your
  start menu).

Installation:
  use this command to install regularly (for example into /usr, in the example):

    icemake -dif c:\
  -- or --
    build.cmd -dif c:\

Compiling/Installing icemake:
  Installing icemake separately is fairly simple:

    icemake -dif c:\ icemake
  -- or --
    build.cmd -dif c:\ icemake

  Icemake will heed the environment variables CHOST, CFLAGS, CXXFLAGS and
  LDFLAGS. Other flags, such as CC, are ignored, because you /should/ set the
  proper CHOST instead.

Selecing a Toolchain:
  Use the -t command to set a target CHOST, the fourth part of it is used to
  determine the toolchain to search for and use:

    icemake -t i386-microsoft-windows-gnu
  -- or --
    build.cmd -t i386-microsoft-windows-gnu

  You may find this useful in situations where you have more than one compiler
  suite in your PATH and the default choice icemake makes doesn't suit your
  taste.

  Valid toolchains are:
    * i386-microsoft-windows-gnu     -- GCC/MinGW
    * i386-microsoft-windows-borland -- Borland/Turbo C++
    * i386-microsoft-windows-msvc    -- Microsoft Visual C++

Testing:
  Please help by giving this libraries' test suites a shot:

    icemake -r
  -- or --
    build.cmd -r

  When running this suite, please take note of the memory use with a tool like
  top. Afterwards, just send the output of the command to
  <magnus.deininger@student.uni-tuebingen.de>, or drop by on IRC

CONTACT:
  Best bet is IRC: freenode #kyuba
