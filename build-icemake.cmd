@ECHO OFF
REM Build System Bootstrap
REM Win32\cmd Variant

SET INCLUDES=-Iinclude\windows -Iinclude\internal\windows -Iinclude -Iinclude\internal
SET ICEMAKE_FILES=icemake icemake-build icemake-install icemake-link icemake-crosslink icemake-stubs sexpr memory memory-pool memory-allocator io string io-system sexpr-read-write sexpr-library tree immutable multiplex multiplex-signal multiplex-process multiplex-io multiplex-system signal-system exec exec-system network network-system multiplex-sexpr filesystem sexpr-stdio stdio
SET OBJECTS=

SET TOOLCHAINTYPE=

IF NOT EXIST build (
    mkdir build
)

GOTO MAIN

:BORLAND_BUILD
SET OBJECTS=%2bj %OBJECTS%
IF EXIST %2bj GOTO :EOF

bcc32 -AT -q -w %INCLUDES% -o %2bj -c %1
GOTO :EOF

:BORLAND_LINK
IF EXIST build\b-icemake.exe GOTO :EOF

bcc32 -q -o build\b-icemake.exe %*
GOTO :EOF

:GCC_BUILD
SET OBJECTS=%2 %OBJECTS%
IF EXIST %2 GOTO :EOF

@ECHO ON
gcc -std=c99 -Wall -pedantic %INCLUDES% -c %1 -o %2
@ECHO OFF
GOTO :EOF

:GCC_LINK
IF EXIST build\b-icemake.exe GOTO :EOF

@ECHO ON
gcc %* -o build\b-icemake.exe
@ECHO OFF
GOTO :EOF

:BUILD
IF EXIST src\windows\%TOOLCHAINTYPE%\%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src\windows\%TOOLCHAINTYPE%\%1.c build\%1.o
) ELSE IF EXIST src\windows\%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src\windows\%1.c build\%1.o
) ELSE IF EXIST src\ansi\%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src\ansi\%1.c build\%1.o
) ELSE IF EXIST src\posix\%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src\posix\%1.c build\%1.o
) ELSE IF EXIST src\%1.c (
    CALL :%TOOLCHAINTYPE%_BUILD src\%1.c build\%1.o
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

:GETTOOLCHAIN
bcc32 --version
IF %ERRORLEVEL% == 0 (
    SET TOOLCHAINTYPE=borland
) ELSE (
    gcc --version
    IF %ERRORLEVEL% == 0 (
        SET TOOLCHAINTYPE=gcc
    )
)

GOTO :EOF

:MAIN

CALL :GETTOOLCHAIN
CALL :BUILDALL %ICEMAKE_FILES%
CALL :LINKALL %ICEMAKE_FILES%

build\b-icemake.exe %* curie syscall icemake
