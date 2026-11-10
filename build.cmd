@echo off
setlocal
cd /d "%~dp0"
set VSLANG=1033
set CFG=%1
if "%CFG%"=="" set CFG=Release
set B=%2
if "%B%"=="" set B=out\build

set CMAKE=cmake
where cmake >nul 2>&1 && goto :have_cmake
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist %VSWHERE% goto :no_cmake
for /f "usebackq delims=" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set VSDIR=%%i
set CMAKE="%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not exist %CMAKE% goto :no_cmake
:have_cmake

%CMAKE% -S . -B "%B%" -A Win32 || exit /b 1
%CMAKE% --build "%B%" --config %CFG% -- /m /v:minimal || exit /b 1
echo.
echo Built: "%B%\%CFG%\shStudio 10.exe"
exit /b 0

:no_cmake
echo CMake not found. Install Visual Studio 2022+ with "Desktop development with C++", or put cmake on PATH.
exit /b 1
