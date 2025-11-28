# PWI v153 Client Linux/Wine Port - Progress Notes

## Project Overview
Porting Perfect World International v153 game client to run under Wine on Linux using MinGW-w64 cross-compilation.

## Build System

### Directory Structure
- **Source**: `/home/doug/153Client/`
- **Build output**: `/home/doug/153Client/build/`
- **Game files**: `/home/doug/Desktop/PWI_EN/element/`
- **Logs**: `/home/doug/Desktop/PWI_EN/element/logs/`

### How to Build

**Incremental build (recommended - only recompiles changed files):**
```bash
cd /home/doug/153Client
cmake --build build -j8 2>&1 | tee /tmp/build.log
```

**Full rebuild from scratch:**
```bash
cd /home/doug/153Client
rm -rf build
cmake -B build
cmake --build build -j8
```

**Build single target:**
```bash
cmake --build build --target ElementClient
```

### Deploy and Test
```bash
# Copy executable to game directory
cp /home/doug/153Client/build/ElementClient/elementclient.exe /home/doug/Desktop/PWI_EN/element/

# Clear old logs
rm -f /home/doug/Desktop/PWI_EN/element/logs/*.log

# Run with Wine (needs display)
cd /home/doug/Desktop/PWI_EN/element
DISPLAY=:0 wine elementclient.exe startbypatcher 2>&1
```

---

## BOLA Trace Logging System

### Overview
Custom debug logging system added to trace execution flow. Outputs to `logs/BolaTrace.log`.

### Log File Location
```
/home/doug/Desktop/PWI_EN/element/logs/BolaTrace.log
```

### Macros (defined in BolaDebug.h)

| Macro | Arguments | Purpose |
|-------|-----------|---------|
| `BOLA_FUNC()` | None | Log function entry/exit (use at function start) |
| `BOLA_INFO(fmt, ...)` | printf-style | Log info message with formatting |
| `BOLA_CHECKPOINT(msg)` | Single string only | Log a checkpoint (NO printf formatting!) |
| `BOLA_ERROR(fmt, ...)` | printf-style | Log error message |

### Usage Examples
```cpp
void MyFunction() {
    BOLA_FUNC();  // Logs ENTER --> MyFunction

    BOLA_INFO("Processing item %d", itemId);  // OK - supports formatting
    BOLA_CHECKPOINT("About to call Init");     // OK - single string
    // BOLA_CHECKPOINT("Item %d", id);         // WRONG - only takes 1 arg!

}  // Automatically logs EXIT <-- MyFunction
```

### Log Format
```
[HH:MM:SS.mmm] [LEVEL] File.cpp:line (Function): Message
[21:31:15.342] [INFO ] ElementClient.cpp:737 (wWinMain): === PWI Client Starting ===
[21:31:15.394] [ENTER] --> _InitGameApp (ElementClient.cpp:1273)
[21:31:15.779] [EXIT ] <-- _InitGameApp (ElementClient.cpp:1273)
```

### Files with BOLA Logging Added
- `ElementClient/ElementClient.cpp` - Main entry, init, main loop
- `ElementClient/EC_Game.cpp` - Game init, RunInMainThread, OnceRun
- `ElementClient/EC_GameRun.cpp` - StartLogin, CreateLoginWorld
- `ElementClient/EC_LoginUIMan.cpp` - Login UI initialization

---

## Current Issue: Crash at 0x00008002

### Symptoms
- Client starts, initializes successfully
- Login world loads
- Main loop begins
- Crash on first iteration when `g_dwFatalErrorFlag == 1`

### Last Log Output (BolaTrace.log)
```
[21:31:21.531] [INFO ] ElementClient.cpp:1040 (wWinMain): Entering main loop
[21:31:21.531] [INFO ] ElementClient.cpp:1046 (wWinMain): Main loop iteration 1 starting
[21:31:21.531] [INFO ] ElementClient.cpp:1049 (wWinMain): Main loop 1: checking g_dwFatalErrorFlag=1
```
Then crash - no more entries.

### Analysis
- `g_dwFatalErrorFlag = 1` means `FATAL_ERROR_LOAD_BUILDING`
- Something during world loading sets this flag
- Crash occurs in the error handling block (lines 1050-1057)
- Address 0x00008002 = NULL pointer + large offset access

### Error Handling Code (ElementClient.cpp ~1050)
```cpp
if (g_dwFatalErrorFlag == FATAL_ERROR_LOAD_BUILDING)
{
    a_LogOutput(1, "exit process because failed to load building");
    HWND hWnd = g_pGame ? g_pGame->GetGameInit().hWnd : NULL;
    MessageBoxA(hWnd, "Failed to load building...", "Fatal Error", MB_OK);
    g_bHookExitProcess = FALSE;
    ::ExitProcess(-2);
}
```

### Places that set FATAL_ERROR_LOAD_BUILDING
- `EC_HomeOrnament.cpp:72`
- `EC_Ornament.cpp:185`
- `EC_Matter.cpp:183, 191`
- `EC_GameRun.cpp:2844`
- `EC_RandomMapPreProcessor.cpp:1265`

### Next Steps to Debug
1. Add BOLA_INFO before `a_LogOutput` call to confirm it reaches there
2. Check if `a_LogOutput` callback is crashing
3. Investigate why building loading is failing (the root cause)

---

## Completed Fixes

### 1. NULL Pointer Checks in EC_LoginUIMan.cpp
- Added NULL check for `GetTerrainWater()` at line 1318
- Added NULL check for `GetTerrainWater()` in `RefreshPlayerList()` at line 1327

### 2. Player Model Paths
- Changed from `Evolved/` to `Models/` directory
- Files: Various model loading code

### 3. Skin Suffix Encoding
- Changed `层` (layer) to `级` (tier) for skin file naming
- GBK encoding issue with Chinese characters

### 4. Folder Name Encoding
- Fixed UTF-8/Latin-1 to GBK byte sequences for folder names
- Required for Chinese path names in assets

### 5. MessageBox NULL Check
- Added NULL check for `g_pGame` before accessing `GetGameInit().hWnd`
- Prevents crash when showing error dialogs

---

## Important Files

| File | Purpose |
|------|---------|
| `ElementClient/ElementClient.cpp` | Main entry point, wWinMain, main loop |
| `ElementClient/EC_Game.cpp` | Game class, initialization, tick/render |
| `ElementClient/EC_GameRun.cpp` | Game state management, login, world loading |
| `ElementClient/EC_LoginUIMan.cpp` | Login screen UI manager |
| `ElementClient/BolaDebug.h` | Debug logging macros |
| `ElementClient/BolaDebug.cpp` | Debug logging implementation |
| `AngelicaCommon/Source/ALog.cpp` | Original logging system |

---

## PCK Files Status
**ALL PCK FILES ARE EXTRACTED** - The client is configured to read from extracted directories, not PCK archives. Set in `ElementClient.ini`:
```ini
PackageFile=
```
Game data is at `/home/doug/Desktop/PWI_EN/element/` with extracted folders.

---

## Wine Testing Notes
- Use `DISPLAY=:0` for graphical output
- `timeout 30` can be used to auto-kill hung processes
- Check `logs/BolaTrace.log` FIRST for debugging (not EC.log)
- Wine may show warnings about stdcall fixups - these are normal

---

## Date: November 27, 2025
