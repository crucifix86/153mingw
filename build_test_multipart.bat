@echo off
REM Build script for testing multi-part PCK support

echo Building test_multipart_pck.exe...

REM Set up Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Copy necessary DLLs
copy /Y WinPCK\pckdll_x86.dll .
copy /Y zlibwapi.dll .

REM Compile test program
cl /nologo /EHsc /MD ^
    /I"AngelicaFile\Header" ^
    /I"AngelicaCommon\Header" ^
    /I"WinPCK_Dependencies\include" ^
    test_multipart_pck.cpp ^
    /link ^
    WinPCK\pckdll_x86.lib ^
    zlibwapi.lib ^
    /out:test_multipart_pck.exe

if %ERRORLEVEL% == 0 (
    echo Build successful!
    echo.
    echo To test litmodels.pck, run:
    echo test_multipart_pck.exe litmodels.pck
) else (
    echo Build failed!
)