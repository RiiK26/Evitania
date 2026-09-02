@echo off
REM scripts\build.bat
REM Run this script to build the Evitania.dll and injector.exe on Windows

REM Change to project root directory
cd /d "%~dp0.."

echo Building project with CMake...
if not exist build mkdir build
cd build

REM Configure the project using the default installed generator (e.g. Visual Studio)
cmake ..

REM Build the project in Release mode
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%

echo Build complete.
pause
