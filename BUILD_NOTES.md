# 153Client Cross-Compilation Build Notes

## Overview

This document tracks the changes and fixes needed to cross-compile the Perfect World International (153) game client from Linux using MinGW-w64.

## Build System

- **Toolchain**: i686-w64-mingw32-g++-posix (MinGW-w64)
- **Build System**: CMake
- **Target**: Windows 32-bit (i686)

## Build Command

```bash
cd /home/doug/153Client
cmake -B build
cmake --build build
```

## Known Issues and Fixes

### 1. Case Sensitivity Issues (Linux vs Windows)

Linux filesystems are case-sensitive while Windows is not. Many includes use different case than the actual files.

**Solution**: Create symbolic links for alternate case versions.

Symlinks created:
- `AUInterface/AUIlabel.h` -> `AUILabel.h`
- `AUInterface/AUIImagepicture.h` -> `AUIImagePicture.h`
- `AUInterface/AUIRADIOBUTTON.h` -> `AUIRadioButton.h`
- `AUInterface/AUITextarea.h` -> `AUITextArea.h`
- `AngelicaCommon/Header/Alist2.h` -> `AList2.h`
- `Angelica3D/header/A3DGdi.h` -> `a3dgdi.h`
- `ElementClient/Ec_Game.h` -> `EC_Game.h`
- `ElementClient/NetWork` -> `Network`

### 2. Windows Backslash Paths in Includes

Many source files use Windows-style backslashes in #include paths.

**Solution**: Use sed to batch convert:
```bash
find . -name "*.cpp" -o -name "*.h" | xargs sed -i 's|#include "\([^"]*\)\\\\\([^"]*\)"|#include "\1/\2"|g'
```

Or fix individual files manually when found.

### 3. MSVC For-Loop Variable Scope Extension

MSVC allows using loop variables outside the loop scope, which is non-standard.

**Example problem**:
```cpp
for(int i=0; i<n; i++) { ... }
// Later use of 'i' outside loop (MSVC allows, GCC doesn't)
for(i=0; i<m; i++) { ... }
```

**Solution**: Add `int` declaration or use different variable:
```cpp
for(int i=0; i<m; i++) { ... }
```

Batch fix:
```bash
sed -i 's/for( i=/for( int i=/g' *.cpp
sed -i 's/for (i=/for (int i=/g' *.cpp
```

### 4. Type Mismatches in min/max Calls

GCC is stricter about template type deduction than MSVC.

**Example problem**:
```cpp
unsigned long x = min(unsignedValue, signedValue);  // Type mismatch
```

**Solution**: Cast operands to consistent types:
```cpp
unsigned long x = min(unsignedValue, (unsigned long)signedValue);
```

### 5. Pure Virtual Destructor Syntax

MSVC accepts `=0{}` for pure virtual destructors, GCC doesn't.

**Problem**:
```cpp
virtual ~MyClass()=0{}
```

**Solution**: Separate declaration from definition:
```cpp
// In header:
virtual ~MyClass() = 0;

// In cpp:
MyClass::~MyClass() {}
```

### 6. Ternary Operator Type Mismatches

GCC requires both branches of ternary to have compatible types.

**Problem**:
```cpp
pObj->SetText(condition ? GetString(123) : L"default");  // May have incompatible types
```

**Solution**: Wrap with constructor to ensure same type:
```cpp
pObj->SetText(condition ? ACString(GetString(123)) : L"default");
```

### 7. swap() with Rvalue References

GCC doesn't allow calling swap() with a temporary.

**Problem**:
```cpp
myVector.swap(vector<int>());  // Rvalue not allowed
```

**Solution**: Use a named variable:
```cpp
vector<int> emptyVec;
myVector.swap(emptyVec);
```

### 8. ATL (Active Template Library) Not Available

ATL is Microsoft-specific and not available with MinGW. Used for IE browser embedding.

**Affected files**:
- `PWBrowser.cpp`
- `PWAtl.h`
- `DlgExplorer.cpp`
- `DlgLoginOther.cpp`

**Solution**: Exclude these files from build and provide stub implementations.

```cmake
# In CMakeLists.txt:
list(FILTER SOURCES EXCLUDE REGEX ".*PWBrowser\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*DlgExplorer\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*DlgLoginOther\\.cpp$")
```

Create `PWBrowser_stub.cpp`:
```cpp
#include <windows.h>
BOOL InitBrowser() { return TRUE; }
BOOL FreeBrowser() { return TRUE; }
```

### 9. Character Encoding (GBK vs UTF-8)

Some source files use GBK encoding for Chinese comments.

**Solution**: Convert to UTF-8:
```bash
iconv -f GBK -t UTF-8 original.cpp > converted.cpp
mv converted.cpp original.cpp
```

**Note**: Do NOT use `-finput-charset=GBK` compiler flag as it breaks angle bracket includes.

### 10. Forward Declaration Issues

Some headers need forward declarations added.

**Examples**:
```cpp
// EC_NPC.h needs:
class CECNPCMan;

// DlgComboSkillShortCut.h needs:
class CDlgComboShortCutBar;
```

### 11. __int64 Cast Syntax

MSVC-specific cast syntax doesn't work with GCC.

**Problem**:
```cpp
__int64 x = value * __int64(count);
```

**Solution**: Use C-style cast:
```cpp
__int64 x = value * ((__int64)(count));
```

## Excluded Features

Due to MinGW limitations, the following features are disabled:

1. **In-game Browser** (ATL dependency) - `PWBrowser`, `DlgExplorer`
2. **Image Cache Library** (MFC dependency) - `ImageCacheLib`
3. **Audio Assist** (MFC dependency) - Editor tool only

## Build Phases

1. Third-party libraries (zlib, jpeg, png, lua, freetype, etc.)
2. Angelica Engine (AngelicaCommon, AngelicaFile, AngelicaMedia, Angelica3D)
3. Audio System (AudioEngine, ogg, vorbis, mpg123)
4. Support Libraries (GfxCommon, FWEditor, AUInterface)
5. Game Libraries (ElementSkill)
6. Main Executable (ElementClient)

## Current Status

- Phases 1-5: Complete
- Phase 6 (ElementClient): In progress, fixing compilation errors

## File Modifications

All modifications are tracked in git. Use `git diff` to see changes.
