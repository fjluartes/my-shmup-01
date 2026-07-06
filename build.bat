@echo off
setlocal

cmake --preset windows-msvc
cmake --build --preset windows-msvc --config Debug
build\msvc\Debug\my-shmup-01.exe

rem cmake --preset windows-mingw
rem cmake --build --preset windows-mingw
rem build\mingw\my-shmup-01.exe