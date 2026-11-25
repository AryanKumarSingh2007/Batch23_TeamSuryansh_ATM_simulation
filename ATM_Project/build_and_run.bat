@echo off
REM Builds and runs the ATM Simulation. Double-click this file.
cd /d "%~dp0"

where gcc >nul 2>nul
if errorlevel 1 (
    echo GCC was not found. Install MinGW-w64 or MSYS2 and add gcc to your PATH.
    pause
    exit /b 1
)

echo Compiling...
gcc main.c atm.c auth.c storage.c -o atm.exe
if errorlevel 1 (
    echo.
    echo Build FAILED. Read the error messages above.
    pause
    exit /b 1
)

echo Build successful. Starting the ATM...
echo.
atm.exe

echo.
pause
