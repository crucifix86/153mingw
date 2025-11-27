# 153Client Linux Cross-Compilation Plan

Goal: Compile the Windows game client on Linux using MinGW-w64 cross-compiler, producing a Windows .exe that runs under Wine.

---

## Phase 1: Environment Setup
- [ ] Install MinGW-w64 cross-compiler (x86_64-w64-mingw32-g++ and i686-w64-mingw32-g++)
- [ ] Install CMake 3.20+
- [ ] Install Wine for testing compiled binaries
- [ ] Create toolchain file for CMake cross-compilation
- [ ] Verify toolchain works with simple test program

---

## Phase 2: DirectX SDK Setup
- [ ] Download DirectX SDK headers (can use Wine's or Microsoft's)
- [ ] Set up include paths for d3d9.h, d3dx9.h, ddraw.h
- [ ] Obtain or create import libraries for d3d9.dll, d3dx9_43.dll, ddraw.dll
- [ ] Set up Windows SDK headers (windows.h, winsock2.h, etc.)

---

## Phase 3: Parse Visual Studio Projects
- [ ] Write script to extract source files from each .vcxproj
- [ ] Extract preprocessor definitions per project
- [ ] Extract include directories per project
- [ ] Extract library dependencies per project
- [ ] Map project dependencies from .sln file

---

## Phase 4: Third-Party Libraries (Static libs - no Windows deps)
- [ ] zlib - CMakeLists.txt
- [ ] zliblib - CMakeLists.txt
- [ ] jpeg - CMakeLists.txt
- [ ] png - CMakeLists.txt
- [ ] tiff - CMakeLists.txt
- [ ] ogg - CMakeLists.txt
- [ ] vorbis - CMakeLists.txt
- [ ] vorbisfile - CMakeLists.txt
- [ ] lua5.1 - CMakeLists.txt
- [ ] mpg123 - CMakeLists.txt
- [ ] cximage - CMakeLists.txt
- [ ] FREETYPE-2.1.10 - CMakeLists.txt

---

## Phase 5: Angelica Engine Core Libraries
- [ ] AngelicaCommon - CMakeLists.txt
- [ ] AngelicaFile - CMakeLists.txt
- [ ] AngelicaMedia - CMakeLists.txt
- [ ] Angelica3D - CMakeLists.txt (D3D9 dependent)

---

## Phase 6: Audio System
- [ ] AudioEngine - CMakeLists.txt (DirectSound + FMOD)
- [ ] AudioAssist - CMakeLists.txt
- [ ] Set up FMOD library linking

---

## Phase 7: Support Libraries
- [ ] async - CMakeLists.txt
- [ ] strmbase - CMakeLists.txt (DirectShow)
- [ ] FTDriver - CMakeLists.txt
- [ ] CHBasedCD - CMakeLists.txt
- [ ] ImmWrapper - CMakeLists.txt
- [ ] LuaWrapper - CMakeLists.txt
- [ ] AutoMoveImp - CMakeLists.txt
- [ ] ImageCacheLib - CMakeLists.txt
- [ ] GfxCommon - CMakeLists.txt
- [ ] FWEditor - CMakeLists.txt
- [ ] PropDlg - CMakeLists.txt
- [ ] AFileDialog - CMakeLists.txt
- [ ] AUInterface - CMakeLists.txt

---

## Phase 8: Game-Specific Libraries
- [ ] ElementSkill - CMakeLists.txt
- [ ] IFC (if needed) - CMakeLists.txt

---

## Phase 9: Main Executable
- [ ] ElementClient - CMakeLists.txt
- [ ] Link all libraries together
- [ ] Handle resource compilation (.rc files) with windres
- [ ] Set up proper linking order

---

## Phase 10: Launcher & Tools
- [ ] Launcher - CMakeLists.txt
- [ ] CNewSkin components (optional)
- [ ] Patcher components (optional)

---

## Phase 11: Testing & Debugging
- [ ] Compile in Debug mode
- [ ] Test binary under Wine
- [ ] Fix linking errors
- [ ] Fix runtime errors
- [ ] Compare behavior with VS-compiled version

---

## Phase 12: Build System Polish
- [ ] Create master CMakeLists.txt with all targets
- [ ] Add Release/Debug configurations
- [ ] Add build scripts (build.sh)
- [ ] Document build process
- [ ] Set up CI if desired

---

## File Structure (Target)

```
153Client/
├── CMakeLists.txt              # Master build file
├── cmake/
│   ├── toolchain-mingw64.cmake # Cross-compile toolchain
│   └── FindDirectX.cmake       # DirectX detection
├── build.sh                    # Simple build script
├── zlib/CMakeLists.txt
├── jpeg/CMakeLists.txt
├── png/CMakeLists.txt
├── ... (each component)
└── ElementClient/CMakeLists.txt
```

---

## Dependencies Graph (Simplified)

```
ElementClient.exe
├── ElementSkill
├── GfxCommon
│   ├── LuaWrapper
│   │   ├── lua5.1
│   │   ├── AngelicaFile
│   │   ├── AngelicaMedia
│   │   └── Angelica3D
│   └── CHBasedCD
├── ImageCacheLib
│   ├── cximage
│   │   ├── jpeg, png, tiff, zlib
│   └── ImmWrapper
├── AudioEngine
│   └── AngelicaFile
├── FTDriver
│   └── FREETYPE
├── async
│   └── strmbase
└── (Windows libs: d3d9, ddraw, dsound, ws2_32, winmm, etc.)
```

---

## Notes

- Target: 32-bit Windows executable (i686-w64-mingw32) - game may require 32-bit
- Alternative: 64-bit (x86_64-w64-mingw32) if 64-bit build exists
- DirectX headers from: Wine development headers or DXSDK
- Some .lib files may need to be converted or recreated as .a files
