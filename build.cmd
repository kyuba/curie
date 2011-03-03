@ECHO OFF
REM Build System Wrapper
REM Win32/cmd Variant

IF NOT EXIST build\bin\ice.exe (
    CALL build-icemake.cmd -fLoid build\ 
)

build\bin\ice %*
