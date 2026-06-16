@echo off
:: Simple launcher — calls the PowerShell script as Admin
echo Starting ESP32 Cyber Academy Setup...
echo.
powershell -Command "Start-Process powershell -ArgumentList '-ExecutionPolicy Bypass -File ""%~dp0SETUP.ps1""' -Verb RunAs"
