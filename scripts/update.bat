@echo off
REM Run this script on Windows to automatically update and rebuild Evitania
cd /d "%~dp0.."

if exist "%~dp0..\resources\Tools\updater.py" (
    python "%~dp0..\resources\Tools\updater.py" %*
) else (
    echo [-] Error: updater.py not found in resources\Tools\
    pause
    exit /b 1
)

if %ERRORLEVEL% NEQ 0 (
    echo [!] Update failed with error code %ERRORLEVEL%
    pause
    exit /b %ERRORLEVEL%
)
pause
