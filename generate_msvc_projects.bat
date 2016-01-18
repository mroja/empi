@echo off

set CMAKE="C:\Program Files (x86)\CMake\bin\cmake.exe"
set FFTW_PATH_32="C:\Users\Alex\Source\Repos\fftw-3.3.4\32"
set FFTW_PATH_64="C:\Users\Alex\Source\Repos\fftw-3.3.4\64"

mkdir build32 & pushd build32
%CMAKE% -G "Visual Studio 14 2015" -DCMAKE_LIBRARY_PATH="%FFTW_PATH_32%" -DCMAKE_INCLUDE_PATH="%FFTW_PATH_32%" ..\
popd

mkdir build64 & pushd build64
%CMAKE% -G "Visual Studio 14 2015 Win64" -DCMAKE_LIBRARY_PATH="%FFTW_PATH_64%" -DCMAKE_INCLUDE_PATH="%FFTW_PATH_64%" ..\
popd

pause

rem %CMAKE% --build build32 --config Release
rem %CMAKE% --build build64 --config Release
