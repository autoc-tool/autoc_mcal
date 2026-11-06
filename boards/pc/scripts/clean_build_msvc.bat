@echo off
setlocal
set ROOT=%~dp0..\..\..\..\..
rem ROOT = vcos (boards/pc/scripts -> 4 levels up is vendor? wait)
rem scripts is at vendor/autoc/boards/pc/scripts
rem .. = pc, ../.. = boards, ../../.. = autoc, ../../../.. = vendor, ../../../../.. = vcos
set VCOS=%~dp0..\..\..\..\..
for %%I in ("%VCOS%") do set VCOS=%%~fI
set OUT=%VCOS%\..\output\minimal_system5_pc_gcc
echo VCOS=%VCOS%
echo OUT=%OUT%
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%" 2>nul
cd /d "%VCOS%\build"
call "%VCOS%\.venv\Scripts\activate.bat" 2>nul
python vcos_build.py -app_name minimal_system5 -board_name pc -compiler gcc -maketool ninja -all -update
if errorlevel 1 exit /b 1
python "%~dp0msvc_sanitize_ninja.py" "%OUT%"
cd /d "%OUT%"
ninja
exit /b %ERRORLEVEL%