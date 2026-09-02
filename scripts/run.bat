@echo off
REM Run this script to start the game and inject the Evitania cheat on Windows

cd /d "%~dp0.."

echo Starting Evitania Online...
start "" "C:\Program Files (x86)\Steam\steamapps\common\Evitania Online\Evitania.exe"

echo Waiting for game to load...
timeout /t 5 /nobreak

echo Injecting Evitania.dll...
cd build
injector.exe

echo Injection complete!
pause
