@echo off
echo Building test_pck for x86...
cl /EHsc test_pck.cpp /Fe:test_pck_x86.exe
echo.
echo Building test_pck for x64...
cl /EHsc test_pck.cpp /Fe:test_pck_x64.exe
echo.
echo Build complete. Run with:
echo   test_pck_x86.exe litmodels.pck
echo   test_pck_x64.exe litmodels.pck