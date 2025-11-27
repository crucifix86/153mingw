# PWI 153 Client Wine Debugging Notes

## CRITICAL: DO NOT DELETE THE BUILD DIRECTORY
The build takes a very long time. Always do INCREMENTAL builds only.
- Only touch files that need changing
- Run `cmake --build build -j8` to rebuild (incremental)
- NEVER run `rm -rf build`

## How to Run the Client

```bash
# 1. Build (incremental - only rebuilds changed files)
cd /home/doug/153Client
cmake --build build -j8

# 2. Copy exe to game directory
cp build/ElementClient/elementclient.exe /home/doug/Desktop/PWI_EN/element/

# 3. Run with Wine
cd /home/doug/Desktop/PWI_EN/element
export DISPLAY=:0
wine elementclient.exe startbypatcher
```

## Current Issues Being Fixed

### Issue: ASSERT crashes under Wine
Wine's threading model differs from Windows, causing various ASSERT failures.

**Solution**: Disable ASSERT globally when BOLA_DEVBUILD is defined.

### Files Modified:

1. **`/home/doug/153Client/AngelicaCommon/Header/ABaseDef.h`**
   - Changed ASSERT macro to no-op under BOLA_DEVBUILD
   - This should catch most ASSERT calls

2. **`/home/doug/153Client/AUInterface/ImeUi.cpp`**
   - Removed `assert(false)` from TSF thread checks (5 locations)
   - IME won't work under Wine but won't crash

3. **`/home/doug/153Client/AUInterface/IMEWrapper.cpp`**
   - Removed `assert(false)` from TSF thread checks (2 locations)

4. **`/home/doug/153Client/AUInterface/AUITag.h`**
   - Removed ASSERT from IMPLEMENT_AUITEXT_TAG macro

### FIXED - Global Assert Disable (CMakeLists.txt)
Added to CMakeLists.txt:
```cmake
# Wine compatibility: disable all asserts to prevent crashes
add_compile_definitions(NDEBUG)  # Disables standard assert()
add_compile_definitions(BOLA_DEVBUILD)  # Disables custom ASSERT macros
```

This defines:
- `NDEBUG` - Standard way to disable C `assert()` from `<assert.h>`
- `BOLA_DEVBUILD` - Custom flag checked in ABaseDef.h to disable `ASSERT` macro

## Build Commands Reference

```bash
# Incremental build (ALWAYS use this)
cd /home/doug/153Client && cmake --build build -j8

# Rebuild single target only (faster for testing)
cd /home/doug/153Client && cmake --build build --target AUInterface

# Check build log
tail -f /tmp/build.log
```

## Game Directory Structure
- Game files: `/home/doug/Desktop/PWI_EN/element/`
- Client exe: `/home/doug/Desktop/PWI_EN/element/elementclient.exe`
- Logs: `/home/doug/Desktop/PWI_EN/element/logs/`
