@ECHO OFF
REM Build System Bootstrap
REM Win32\cmd Variant

SET INCLUDES=-Iinclude\windows -Iinclude
SET INCLUDESMSVC=/Iinclude\windows /Iinclude
SET ICEMAKE_FILES=icemake icemake-os-generic icemake-stubs sexpr memory memory-pool memory-allocator io string io-system sexpr-read-write sexpr-library tree-basic immutable multiplex multiplex-signal multiplex-process multiplex-io multiplex-system signal-system exec exec-system network network-system multiplex-sexpr filesystem sexpr-stdio stdio utf-8 sexpr-custom hash tree-string tree-library gcd variables libc-compat io-pool sexpr-set bootstrap shell regex graph sexpr-sort metadata-path metadata-unix time-unix icemake-vis-stub icemake-vis-raw icemake-vis-ice ice sievert-filesystem sexpr-list icemake-archive cpio directory directory-common sexpr-alist
SET OBJECTS=

SET TOOLCHAINTYPE=

IF NOT EXIST build (
    mkdir build
)

GOTO MAIN

:MSVC_BUILD
SET OBJECTS=%2bj %OBJECTS%
IF EXIST %2bj GOTO :EOF

cl %CFLAGS% %CCFLAGS% /DNOVERSION /c /TC %INCLUDESMSVC% /nologo %1 /Fo%2bj
GOTO :EOF

:MSVC_LINK
IF EXIST build\b-icemake.exe GOTO :EOF

cl %LDFLAGS% /nologo /Febuild\b-icemake.exe %*
GOTO :EOF

:GCC_BUILD
SET OBJECTS=%2 %OBJECTS%
IF EXIST %2 GOTO :EOF

@ECHO ON
gcc %CFLAGS% %CCFLAGS% -DNOVERSION -std=c99 %INCLUDES% -c %1 -o %2
@ECHO OFF
GOTO :EOF

:GCC_LINK
IF EXIST build\b-icemake.exe GOTO :EOF

@ECHO ON
gcc %LDFLAGS% %* -o build\b-icemake.exe
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

cl
IF "%ERRORLEVEL%"=="0" (
    SET TOOLCHAINTYPE=msvc
)

IF NOT "%TOOLCHAINTYPE%"=="" GOTO :EOF

gcc --version
IF "%ERRORLEVEL%"=="0" (
    SET TOOLCHAINTYPE=gcc
)

GOTO :EOF

:MAIN

CALL :GETTOOLCHAIN
CALL :BUILDALL %ICEMAKE_FILES%
CALL :LINKALL %ICEMAKE_FILES%

build\b-icemake.exe -D data\toolchains.ice %* curie-bootstrap syscall curie sievert icemake ice
