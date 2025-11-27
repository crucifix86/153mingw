#!/bin/bash
# Fix case sensitivity issues for Linux builds

cd /home/doug/153Client

# Create a local include directory with case-fixed symlinks for Windows headers
echo "Creating case-insensitive Windows header symlinks..."
mkdir -p include_fix
cd include_fix

# Create symlinks for common Windows headers with wrong case
MINGW_INC="/usr/i686-w64-mingw32/include"

# Windows.h variants
ln -sf "$MINGW_INC/windows.h" Windows.h 2>/dev/null
ln -sf "$MINGW_INC/stdio.h" StdIO.h 2>/dev/null
ln -sf "$MINGW_INC/stdlib.h" StdLib.h 2>/dev/null
ln -sf "$MINGW_INC/winuser.h" WinUser.h 2>/dev/null
ln -sf "$MINGW_INC/wingdi.h" WinGdi.h 2>/dev/null
ln -sf "$MINGW_INC/winbase.h" WinBase.h 2>/dev/null
ln -sf "$MINGW_INC/winsock2.h" WinSock2.h 2>/dev/null
ln -sf "$MINGW_INC/winsock.h" WinSock.h 2>/dev/null
ln -sf "$MINGW_INC/mmsystem.h" MMSystem.h 2>/dev/null
ln -sf "$MINGW_INC/shlwapi.h" Shlwapi.h 2>/dev/null
ln -sf "$MINGW_INC/shellapi.h" ShellAPI.h 2>/dev/null
ln -sf "$MINGW_INC/commctrl.h" CommCtrl.h 2>/dev/null
ln -sf "$MINGW_INC/commdlg.h" CommDlg.h 2>/dev/null
ln -sf "$MINGW_INC/richedit.h" RichEdit.h 2>/dev/null
ln -sf "$MINGW_INC/windowsx.h" WindowsX.h 2>/dev/null
ln -sf "$MINGW_INC/ole2.h" Ole2.h 2>/dev/null
ln -sf "$MINGW_INC/objbase.h" ObjBase.h 2>/dev/null
ln -sf "$MINGW_INC/tchar.h" Tchar.h 2>/dev/null
ln -sf "$MINGW_INC/tchar.h" TChar.h 2>/dev/null
ln -sf "$MINGW_INC/imm.h" Imm.h 2>/dev/null
ln -sf "$MINGW_INC/process.h" Process.h 2>/dev/null
ln -sf "$MINGW_INC/dbghelp.h" DbgHelp.h 2>/dev/null
echo "Windows header symlinks created in include_fix/"

cd /home/doug/153Client

# jpeg library - create lowercase symlinks
cd jpeg
for f in J*.h; do
    lower=$(echo "$f" | tr 'A-Z' 'a-z')
    if [ "$f" != "$lower" ] && [ ! -e "$lower" ]; then
        ln -sf "$f" "$lower"
        echo "Created symlink: $lower -> $f"
    fi
done
cd ..

# AngelicaCommon - fix header -> Header
cd AngelicaCommon
if [ -d "Header" ] && [ ! -e "header" ]; then
    ln -sf Header header
    echo "Created symlink: header -> Header"
fi
cd ..

# AngelicaFile
cd AngelicaFile
if [ -d "Header" ] && [ ! -e "header" ]; then
    ln -sf Header header
    echo "Created symlink: header -> Header"
fi
cd ..

# AngelicaMedia
cd AngelicaMedia
if [ -d "Header" ] && [ ! -e "header" ]; then
    ln -sf Header header
    echo "Created symlink: header -> Header"
fi
cd ..

# Angelica3D
cd Angelica3D
if [ -d "Header" ] && [ ! -e "header" ]; then
    ln -sf Header header
    echo "Created symlink: header -> Header"
fi
cd ..

# ImmWrapper
cd ImmWrapper
if [ -d "Header" ] && [ ! -e "header" ]; then
    ln -sf Header header
    echo "Created symlink: header -> Header"
fi
cd ..

# strmbase - Schedule.h -> schedule.h
cd strmbase
if [ -f "schedule.h" ] && [ ! -e "Schedule.h" ]; then
    ln -sf schedule.h Schedule.h
    echo "Created symlink: Schedule.h -> schedule.h"
fi
cd ..

# FTDriver - StdAfx.h -> stdafx.h
cd FTDriver
if [ -f "StdAfx.h" ] && [ ! -e "stdafx.h" ]; then
    ln -sf StdAfx.h stdafx.h
    echo "Created symlink: stdafx.h -> StdAfx.h"
fi
cd ..

echo "Case sensitivity fixes applied!"
