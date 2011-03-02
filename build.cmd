@ECHO OFF
REM Build System Wrapper
REM Win32/cmd Variant

IF NOT EXIST build\bin\icemake.exe (
    CALL build-icemake.cmd -fLoid build\ 
)

IF ERRORLEVEL 0 (
    build\bin\icemake %*
)
