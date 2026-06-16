@echo off
echo Starting ESP32 Cyber Academy Setup...
echo.

:: Check if already running as Administrator
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo Requesting administrator privileges...
    powershell -NoProfile -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

:: Already admin — run the PowerShell setup script
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0SETUP.ps1"
pause
