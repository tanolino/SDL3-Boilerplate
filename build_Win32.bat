@echo off
cmake -G "Visual Studio 17 2022" --fresh -S . -B build_Win32 
cmake --build build_Win32 --config Release
rem Release
cmake --install build_Win32 --prefix install_Win32 --config Release

