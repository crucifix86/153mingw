# PWI v153 Client - Building Load Crash Debug Plan

**Issue:** Client crashes on first main loop iteration with `g_dwFatalErrorFlag=1` (FATAL_ERROR_LOAD_BUILDING)
**Crash Address:** 0x00008002 (NULL pointer + offset)
**Last Known Good Point:** Main loop iteration 1, checking g_dwFatalErrorFlag

**STATUS: RESOLVED** - See findings below.

---

## Phase 1: Identify Which File Sets the Fatal Error Flag

### Files to Instrument with BOLA Tracing

- [x] **1.1** `EC_HomeOrnament.cpp:72` - Add BOLA_ERROR before flag set
- [x] **1.2** `EC_Ornament.cpp:185` - Add BOLA_ERROR before flag set (**THIS WAS THE SOURCE**)
- [x] **1.3** `EC_Matter.cpp:183` - Add BOLA_ERROR before flag set
- [x] **1.4** `EC_Matter.cpp:191` - Add BOLA_ERROR before flag set
- [x] **1.5** `EC_GameRun.cpp:2844` - Add BOLA_ERROR before flag set
- [x] **1.6** `EC_RandomMapPreProcessor.cpp:1265` - Add BOLA_ERROR before flag set

### Outcome
- BOLA trace showed `EC_Ornament.cpp:185` was setting the flag
- The file paths being loaded were corrupted (e.g., `LitModels\Login\�ba` instead of `LitModels\Login\1\litmodel_268.bmd`)

---

## Phase 2: Trace the Error Handler Crash

### The crash occurs in ElementClient.cpp ~1050-1057

- [x] **2.1** Add BOLA_INFO immediately before `a_LogOutput(1, "exit process...")`
- [x] **2.2** Add BOLA_INFO after `a_LogOutput` call
- [x] **2.3** Add BOLA_INFO before `MessageBoxA` call
- [x] **2.4** Verify `g_pGame` is not NULL before accessing hWnd
- [x] **2.5** Disabled MessageBoxA to avoid potential crash (skip directly to exit)

### Outcome
- The crash was secondary - the real issue was corrupted paths causing building load failures

---

## Phase 3: Investigate Root Cause (Building Load Failure)

### Investigation Steps:

- [x] **3.1** Trace the specific Load() function that fails - `EC_Ornament::Load()`
- [x] **3.2** Check file path being loaded - paths were corrupted!
- [x] **3.3** Verify file exists on disk - files existed but paths were wrong
- [x] **3.4** Check if path encoding issue - **ROOT CAUSE FOUND**
- [x] **3.5** Check if PCK vs disk loading issue - not the issue

### Root Cause Identified:
The issue was in `EC_Ornament.cpp` when building the file path using `AString::Format()`:

```cpp
m_strFileToLoad.Format("%s\\%s", szMapPath, strModelFile);  // BUG!
```

When passing an `AString` object to a variadic function like `Format()`, the implicit conversion to `const char*` doesn't work correctly. The fix is to use an explicit cast:

```cpp
m_strFileToLoad.Format("%s\\%s", szMapPath, (const char*)strModelFile);  // FIXED!
```

---

## Phase 4: Fix Implementation

### Fix Applied:

- [x] **4.1** Added explicit `(const char*)` cast when passing AString to Format()
- [x] **4.2** Applied fix to all Format() calls in `EC_Ornament::Load()`
- [x] **4.3** Added comment explaining the issue for future reference
- [x] **4.4** Cleaned up verbose debug logging
- [x] **4.5** Rebuild and test - **SUCCESS!**

### Code Change in EC_Ornament.cpp:
```cpp
//  Build building file name
//  NOTE: Must use explicit (const char*) cast when passing AString to variadic Format()
//  to ensure proper conversion - without the cast, the object may be passed incorrectly.
if (pScene->GetSceneFileFlags() == ECWFFLAG_EDITOREXP)
{
    if (pScene->GetBSDFileVersion() < 12)
    {
        const char* pTemp = strrchr(strModelFile, '\\');
        pTemp = pTemp ? pTemp+1 : (const char*)strModelFile;
        m_strFileToLoad.Format("%s\\%s", szMapPath, pTemp);
    }
    else
        m_strFileToLoad.Format("%s\\%s", szMapPath, (const char*)strModelFile);
}
else
{
    m_strFileToLoad.Format("%s\\%s", szMapPath, (const char*)strModelFile);
}
```

---

## Phase 5: Verification

- [x] **5.1** Clean build - SUCCESS
- [x] **5.2** Deploy to game directory - DONE
- [x] **5.3** Clear old logs - DONE
- [x] **5.4** Run client - **SUCCESS!**
- [x] **5.5** Verify no crash at building load - **PASSED!**
- [x] **5.6** Verify login screen appears - **USER CONFIRMED SERVER SELECT SCREEN VISIBLE!**
- [ ] **5.7** Update PROGRESS_NOTES.md

---

## Debug Commands Quick Reference

```bash
# Build
cd /home/doug/153Client
cmake --build build -j8 2>&1 | tee /tmp/build.log

# Deploy
cp build/ElementClient/elementclient.exe /home/doug/Desktop/PWI_EN/element/

# Clear logs
rm -f /home/doug/Desktop/PWI_EN/element/logs/*.log

# Run
cd /home/doug/Desktop/PWI_EN/element
DISPLAY=:0 timeout 30 wine elementclient.exe startbypatcher 2>&1

# Check trace log
cat /home/doug/Desktop/PWI_EN/element/logs/BolaTrace.log
```

---

## Log Locations

| Log | Purpose |
|-----|---------|
| `/home/doug/Desktop/PWI_EN/element/logs/BolaTrace.log` | Custom debug trace (PRIMARY) |
| `/home/doug/Desktop/PWI_EN/element/logs/EC.log` | Game engine log |
| `/tmp/build.log` | Build output |

---

## Status

**Current Phase:** COMPLETE - Issue resolved!
**Last Updated:** November 27, 2025
**Resolution:** Fixed AString implicit conversion bug in EC_Ornament.cpp

---

## Notes

```
[2025-11-27] [1.2] EC_Ornament.cpp:185 was setting FATAL_ERROR_LOAD_BUILDING
[2025-11-27] [3.2] Path corruption observed: "LitModels\Login\�ba" instead of proper paths
[2025-11-27] [3.4] ROOT CAUSE: AString passed to variadic Format() without explicit (const char*) cast
[2025-11-27] [4.5] FIX VERIFIED: Client now reaches server select screen without crash!
```

---

## Future Considerations

The same AString-to-variadic-function bug pattern may exist elsewhere in the codebase. Consider:
1. Searching for other `.Format(` calls that pass AString objects
2. Searching for `a_LogOutput` calls that pass AString objects
3. Adding a helper macro or function to ensure safe AString-to-string conversion
