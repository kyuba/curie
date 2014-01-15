# COMPILING CURIE #############################################################

Curie uses a good, old-fashioned makefile to compile and install the library.
In order to compile the library, simply change to the root directory of the
Curie source directory and issue the 'make' command:

    $ make

This will create a directory '.build' and some support files in that directory.
Building the library will then commence directly and should - hopefully -
complete without any errors.

NOTE: Make sure you have a version of 'make' that supports the GNU make
      extensions and that your shell is a proper Bourne shell.

## INSTALLING CURIE ###########################################################

Simply use the 'install' target, like so:

    $ make install

The makefiles support the PREFIX and DESTDIR parameters to determine where to
install to. DESTDIR can be used to choose a different root directory to copy
files to, which is useful when creating packages. PREFIX is used to supply an
FHS prefix - the default is /usr/local. Since this is a system library, you
would probably want to use:

    $ make PREFIX=/ install

## TEST CASES #################################################################

Curie comes with a fairly large number of test cases, which you can run by
supplying the 'test' target to make:

    $ make test

## COMPILING DEPENDANT PROJECTS ###############################################

Since Curie is a very basic, system-level library, it is desirable to use that
library in some way. Curie-based libraries and programmes may be compiled in
one of two ways: either you need to install Curie properly, or you need to copy
the dependant project's sources into Curie's source directory. The former is
fairly straightforward, but the latter may be more convenient, especially when
cross-compiling.

To compile Curie projects without first installing Curie, simply copy or check
out the other project in your Curie directory, then compile normally:

    $ cd curie
    $ git clone https://github.com/kyuba/duat.git
    $ make

You could also check out all of the Curie-related repositories side-by-side and
create a minimal Curie makefile in that top level directory:

    $ cd projects
    $ git clone https://github.com/kyuba/curie.git
    $ git clone https://github.com/kyuba/duat.git
    $ echo 'include curie/include/curie.mk' > makefile
    $ make

The 'include/curie.mk' file is also installed when installing Curie with the
'install' target. That way a typical Curie-baed project only needs to create a
minimal makefile that contains a single 'include curie.mk' command.
