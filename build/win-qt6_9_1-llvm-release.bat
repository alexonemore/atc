@echo off
setlocal

rem === Vars ===
set "QT_PATH=C:\Qt\6.9.1\llvm-mingw_64"
set "LLVM_PATH=C:\Qt\Tools\llvm-mingw1706_64"
set "BUILD_DIR=build-win-qt6_9_1-llvm-release"
set "STATIC_BUILD=true"

rem === Code ===
set "CMAKE_STATIC_FLAG=OFF"

if /i "%STATIC_BUILD%"=="true" (
	set "CMAKE_STATIC_FLAG=ON"
    set "BUILD_DIR=%BUILD_DIR%-static"
)

set "PATH=%QT_PATH%\bin;%LLVM_PATH%\bin;%PATH%"

if not exist "%BUILD_DIR%" (
    mkdir %BUILD_DIR%
    if errorlevel 1 exit /b %ERRORLEVEL%
)

cd %BUILD_DIR%
if errorlevel 1 exit /b %ERRORLEVEL%

cmake ../.. ^
  -G "MinGW Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_PREFIX_PATH="%QT_PATH%" ^
  -DSTATIC_BUILD=%CMAKE_STATIC_FLAG%
if errorlevel 1 exit /b %ERRORLEVEL%

cmake --build . --config Release --parallel --target install
if errorlevel 1 exit /b %ERRORLEVEL%

cd install/bin
if errorlevel 1 exit /b %ERRORLEVEL%

windeployqt atc.exe
if errorlevel 1 exit /b %ERRORLEVEL%

if /i not "%STATIC_BUILD%"=="true" (
    windeployqt libqcustomplot.dll
    if errorlevel 1 exit /b %ERRORLEVEL%
)
copy "%QT_PATH%\bin\libc++.dll" .
if errorlevel 1 exit /b %ERRORLEVEL%
    
copy "%QT_PATH%\bin\libunwind.dll" .
if errorlevel 1 exit /b %ERRORLEVEL%

atc.exe
if errorlevel 1 exit /b %ERRORLEVEL%

cd ../../../
if errorlevel 1 exit /b %ERRORLEVEL%

endlocal
