@echo off
REM Run this script to start the game and inject the Evitania cheat on Windows

set "SCRIPT_DIR=%~dp0"
set "DLL_PATH="
set "INJECTOR_EXE="
set "GAME_EXE=EvitaniaOnline.exe"
set "APP_ID=4119420"

for %%F in ("%SCRIPT_DIR%*.dll") do set "DLL_PATH=%%F"
if defined DLL_PATH if exist "%SCRIPT_DIR%injector.exe" (
    set "INJECTOR_EXE=%SCRIPT_DIR%injector.exe"
    goto found_dll
)

for %%F in ("%SCRIPT_DIR%..\build\release\*.dll") do set "DLL_PATH=%%F"
if defined DLL_PATH if exist "%SCRIPT_DIR%..\build\release\injector.exe" (
    set "INJECTOR_EXE=%SCRIPT_DIR%..\build\release\injector.exe"
    goto found_dll
)

echo Error: Could not find any .dll and injector.exe
pause
exit /b 1

:found_dll

echo Starting Evitania Online...
start steam://rungameid/%APP_ID%

echo Waiting for game to load...
timeout /t 5 /nobreak

echo Injecting Evitania.dll...
"%INJECTOR_EXE%" "%GAME_EXE%" "%DLL_PATH%"

echo Injection complete!
pause
