@ECHO OFF
REM Build System Bootstrap
REM Win32/cmd Variant

SET INCLUDES=-Iinclude/windows/internal -Iinclude/windows -Iinclude/internal -Iinclude

SET ICEMAKE_FILES=icemake icemake-build icemake-install icemake-link icemake-crosslink icemake-stubs sexpr memory memory-pool memory-allocator io string io-system sexpr-read-write sexpr-library tree immutable multiplex multiplex-signal multiplex-process multiplex-io multiplex-system signal-system exec exec-system network network-system multiplex-sexpr filesystem
SET OBJECTS=

SET TOOLCHAINTYPE=gcc

IF NOT EXIST build (
    mkdir build
)

GOTO MAIN

:GCC_BUILD
@ECHO ON
gcc -std=c99 -Wall -pedantic -g %INCLUDES% -c %1 -o %2
@ECHO OFF
SET OBJECTS=%2 %OBJECTS%
GOTO :EOF

:GCC_LINK
@ECHO ON
gcc %* -lws2_32 -o build/b-icemake.exe
@ECHO OFF
GOTO :EOF

:BUILD
IF EXIST src/windows/%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src/windows/%1.c build/%1.o
) ELSE IF EXIST src/ansi/%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src/ansi/%1.c build/%1.o
) ELSE IF EXIST src/posix/%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src/posix/%1.c build/%1.o
) ELSE IF EXIST src/%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src/%1.c build/%1.o
)
GOTO :EOF

:LINK
CALL :%TOOLCHAINTYPE%_LINK %*
GOTO :EOF

:BUILDALL
FOR %%f IN (%*) DO (
    CALL :BUILD %%f
)
GOTO :EOF

:LINKALL
CALL :LINK %OBJECTS%

SET OBJECTS=
GOTO :EOF

:MAIN

CALL :BUILDALL %ICEMAKE_FILES%
CALL :LINKALL %ICEMAKE_FILES%
