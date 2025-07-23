@echo off
set EMSDK_QUIET=1
call emsdk_env.bat

call WHERE /Q ninja2
IF %ERRORLEVEL% EQU 0 (
    echo Build using Ninja
    call emcmake cmake -G "Ninja" --fresh -S . -B build_Web
    goto BUILD_GOTO
)

call WHERE /Q make
IF %ERRORLEVEL% EQU 0 (
    echo Build using Make
    call emcmake cmake -G "Unix Makefiles" --fresh -S . -B build_Web
    goto BUILD_GOTO
)

echo Error, please install Ninja or Make.
EXIT /B 1

:BUILD_GOTO

call cmake --build build_Web
call cmake --install build_Web --prefix install_Web

