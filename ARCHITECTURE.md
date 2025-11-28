# PWI v153 Client - Technical Architecture Reference

**Version:** 1.5.3 Build 2457
**Platform:** Windows (cross-compiled from Linux via MinGW-w64)
**Last Updated:** November 27, 2025

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Build System](#2-build-system)
3. [Application Lifecycle](#3-application-lifecycle)
4. [Engine Architecture (Angelica3D)](#4-engine-architecture-angelica3d)
5. [UI System (AUInterface)](#5-ui-system-auinterface)
6. [Game Logic (ElementClient)](#6-game-logic-elementclient)
7. [File & Resource Systems](#7-file--resource-systems)
8. [Audio System](#8-audio-system)
9. [Skill System](#9-skill-system)
10. [Network Layer](#10-network-layer)
11. [Current Issues & Debug Info](#11-current-issues--debug-info)

---

## 1. Project Overview

### Directory Structure

```
/home/doug/153Client/
├── Angelica3D/          # 3D rendering engine (DirectX 8/9)
├── AngelicaCommon/      # Common utilities, logging, memory
├── AngelicaFile/        # PCK archive system, file I/O
├── AngelicaMedia/       # Media loading (sounds, textures)
├── AUInterface/         # UI widget system (80+ controls)
├── AudioEngine/         # FMOD-based sound system
├── ElementClient/       # Main game client code
├── ElementSkill/        # Skill definitions (3300+ skills)
├── GfxCommon/           # Graphics effects
├── FTDriver/            # FreeType font rendering
├── LuaWrapper/          # Lua scripting integration
├── build/               # Build output
├── cmake/               # CMake toolchain files
├── include_fix/         # MinGW compatibility headers
└── logs/                # Build logs
```

### Key Files

| File | Purpose |
|------|---------|
| `ElementClient/ElementClient.cpp` | Main entry point (wWinMain) |
| `ElementClient/EC_Game.cpp` | CECGame class - engine initialization |
| `ElementClient/EC_GameRun.cpp` | Game state machine |
| `ElementClient/BolaDebug.h/cpp` | Custom debug logging system |
| `CMakeLists.txt` | Root build configuration |
| `build.sh` | Build automation script |

---

## 2. Build System

### Quick Build Commands

```bash
# Full release build
cd /home/doug/153Client
cmake --build build -j8 2>&1 | tee /tmp/build.log

# Clean rebuild
rm -rf build && cmake -B build && cmake --build build -j8

# Single target
cmake --build build --target ElementClient
```

### Cross-Compilation Setup

**Toolchain:** MinGW-w64 (i686-w64-mingw32-g++-posix)

**Key Compiler Flags:**
```cmake
-w                      # Suppress warnings
-fpermissive            # Allow non-standard code
-msse2                  # Enable SSE2
-static-libgcc          # Static linking for Wine
-static-libstdc++
```

**Critical Defines:**
```cmake
_WIN32_WINNT=0x0501     # Windows XP target
_A_FORBID_NEWDELETE     # Use standard CRT allocator
NDEBUG                  # Disable asserts (Wine stability)
BOLA_DEVBUILD           # Disable custom ASSERT macros
_NOMINIDUMP             # Disable SEH (GCC incompatible)
DX9                     # DirectX 9 mode
```

### Build Phases (Dependency Order)

1. **Phase 4:** Third-party libs (zlib, jpeg, png, ogg, vorbis, lua, freetype)
2. **Phase 5:** Angelica engine core
3. **Phase 6:** DirectShow & Audio
4. **Phase 7:** Support libraries (UI, graphics, fonts)
5. **Phase 8:** ElementSkill
6. **Phase 9:** ElementClient executable

### Excluded Components (Wine/MinGW Incompatible)

- `defence/` - Anti-cheat (MSVC SEH, inline assembly)
- `PWBrowser` - ATL-dependent
- `discord/` - C++ type incompatibilities
- `EL_Tree, EL_Forest` - SpeedTree proprietary library
- MFC-based tools (PropDlg, AFileDialog, ImageCacheLib)

---

## 3. Application Lifecycle

### Startup Flow

```
wWinMain() [ElementClient.cpp:733]
    │
    ├─ BolaDebug_Init("Logs")           # Initialize debug logging
    ├─ InitializeWindowsAPI()
    ├─ InitThreadPool()                  # cxxpool thread pools
    │
    ├─ prepare_file_structures()         # Create cache/, logs/, screenshots/
    ├─ _ParseCommandLine()
    │
    ├─ _InitGameApp() [line 1271]
    │   ├─ af_Initialize()               # File system init
    │   ├─ glb_InitLogSystem("EC.log")
    │   ├─ _InitNetwork()                # WSAStartup, GNET::PollIO
    │   └─ g_GameCfgs.Init()             # Load configs
    │
    ├─ CheckHardwareCaps()               # D3D device caps
    ├─ _RegisterWndClass()
    ├─ _CreateMainWnd()
    │
    ├─ l_theGame.Init(GameInit)          # CECGame::Init [EC_Game.cpp:353]
    │   ├─ InitA3DEngine()               # Angelica3D setup
    │   ├─ CreateFonts()
    │   ├─ A3DGFXExMan, ECModelMan       # Effect managers
    │   ├─ LoadCursors()
    │   ├─ ShadowRender, PortraitRender
    │   ├─ LoadElementData()             # Game data files
    │   └─ CECGameRun::Init()
    │
    ├─ StartLogin()                      # Enter login state
    │
    └─ Main Loop [line 1042]
        ├─ Check g_dwFatalErrorFlag
        ├─ SuspendRenderThread()
        ├─ AUIEditBox::ToggleIME()
        └─ RunInMainThread() / RenderThread
```

### Game States (CECGameRun)

```cpp
enum {
    GS_NONE  = 0,    // Initial state
    GS_LOGIN = 1,    // Login interface
    GS_GAME  = 2     // In-game
};
```

### Threading Model

| Thread | Purpose |
|--------|---------|
| Main Thread | Game logic, Windows messages |
| Render Thread | D3D rendering (optional multi-thread mode) |
| Defence Thread | Anti-cheat checks (disabled) |
| Load Thread Pool | Asset streaming |
| Tick Thread Pool | Entity updates |

---

## 4. Engine Architecture (Angelica3D)

### Core Classes

```
A3DEngine (Central orchestration)
    ├─ A3DDevice (DirectX abstraction)
    │   ├─ IDirect3DDevice9
    │   ├─ Render state caching
    │   └─ Matrix management
    │
    ├─ A3DViewport + A3DCamera
    │   └─ A3DFrustum (culling)
    │
    ├─ A3DTextureMan (Texture resources)
    ├─ A3DModelMan (Model resources)
    ├─ A3DShaderMan (Shader compilation)
    ├─ A3DLightMan (Lighting)
    ├─ A3DGFXExMan (Particle effects)
    │
    ├─ A3DMeshCollector (Mesh batching)
    ├─ A3DVertexCollector (Vertex batching)
    │
    └─ A3DSkinMan → A3DSkinRender (Skeletal animation)
```

### Rendering Pipeline

```
A3DEngine::RenderAllViewport()
    └─ For each viewport:
        ├─ viewport->Active()
        ├─ Clear(color, Z, stencil)
        ├─ BeginRender()
        │
        ├─ RenderWorld()
        │   ├─ Frustum culling
        │   ├─ Collect visible meshes
        │   ├─ Sort by state/material
        │   └─ Batch render
        │
        ├─ RenderGFX()              # Particle effects
        ├─ FlushCachedAlphaMesh()   # Transparent geometry
        ├─ FlushMeshCollector()
        ├─ FlushVertexCollector()
        │
        └─ EndRender()

    Present() → Display
```

### Key Files

| File | Lines | Purpose |
|------|-------|---------|
| `A3DEngine.h` | ~500 | Main engine class |
| `A3DDevice.h` | ~800 | DirectX device wrapper |
| `A3DCamera.h` | ~300 | Camera and frustum |
| `A3DVertex.h` | ~200 | Vertex format definitions |

---

## 5. UI System (AUInterface)

### Architecture

```
AUIManager (Central controller)
    ├─ Dialog z-order management
    ├─ Hit testing
    ├─ Font management (A3DFTFontMan)
    ├─ Event routing
    │
    └─ AUIDialog[] (Dialog instances)
        └─ AUIObject[] (Controls)
            ├─ AUIButton
            ├─ AUIEditBox
            ├─ AUIListBox
            ├─ AUIComboBox
            ├─ AUILabel
            ├─ AUIProgress
            ├─ AUISlider
            ├─ AUITreeView
            └─ ... (80+ widget types)
```

### Event System

**Declaration:**
```cpp
class MyDialog : public AUIDialog {
    AUI_DECLARE_EVENT_MAP();
    AUI_DECLARE_COMMAND_MAP();
};
```

**Implementation:**
```cpp
AUI_BEGIN_EVENT_MAP(MyDialog, AUIDialog)
    AUI_ON_EVENT("ButtonOK", WM_LBUTTONUP, OnButtonClick)
AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(MyDialog, AUIDialog)
    AUI_ON_COMMAND("Exit", OnCommand_Exit)
AUI_END_COMMAND_MAP()
```

### Dialog Loading

- **XML Format:** Modern format with full property support
- **Script Format (.dcf/.dtf):** Legacy text-based format
- **Template System:** Runtime theme switching

### Key Files

| File | Lines | Purpose |
|------|-------|---------|
| `AUIManager.h` | ~270 | UI manager |
| `AUIDialog.h` | ~830 | Dialog class |
| `AUIObject.h` | ~360 | Base widget |
| `AUIEditBox.h` | ~400 | Text input with IME |

---

## 6. Game Logic (ElementClient)

### Entity Class Hierarchy

```
CECObject (Base entity)
    ├─ CECPlayer (Generic player)
    │   ├─ CECHostPlayer (Local player - controlled)
    │   ├─ CECElsePlayer (Other players)
    │   └─ CECLoginPlayer (Character select)
    │
    ├─ CECNPC (NPCs and monsters)
    │   ├─ NPC_MONSTER
    │   ├─ NPC_SERVER (Quest NPCs)
    │   └─ NPC_PET
    │
    └─ CECMatter (Gatherable objects)
```

### Object Class IDs

```cpp
OCID_OBJECT      = 0    OCID_HOSTPLAYER  = 2
OCID_PLAYER      = 1    OCID_ELSEPLAYER  = 3
OCID_NPC         = 5    OCID_MONSTER     = 6
OCID_MATTER      = 8    OCID_PET         = 9
```

### Manager System

```
CECWorld (World context)
    ├─ CECPlayerMan    # Other players (cache: 80 max, 5-min age)
    ├─ CECNPCMan       # NPCs and monsters
    ├─ CECMatterMan    # Gatherable items
    ├─ CECOrnamentMan  # Decorations
    ├─ CECDecalMan     # Ground effects
    ├─ CECSkillGfxMan  # Skill visual effects
    └─ CECAttacksMan   # Attack effects
```

### Host Player Systems

```
CECHostPlayer
    ├─ CECInventory      # Main bag
    ├─ CECDealInventory  # Trade window
    ├─ CECNPCInventory   # Bank/warehouse
    ├─ CECHPWorkMan      # Action state machine
    ├─ CECFriendMan      # Friend list
    ├─ CECPetCorral      # Pet storage
    └─ CECForceMgr       # Faction system
```

### Player Actions

```cpp
// Movement
MD_FORWARD = 0x01, MD_BACK = 0x04
MD_RIGHT   = 0x02, MD_LEFT = 0x08

// States
ACT_STAND, ACT_WALK, ACT_RUN, ACT_JUMP_*
ACT_FLY, ACT_SWIM, ACT_SITDOWN
ACT_GROUNDDIE, ACT_WATERDIE
```

### Key Files

| File | Purpose |
|------|---------|
| `EC_GameRun.h/cpp` | Game state machine |
| `EC_World.h/cpp` | World management |
| `EC_Player.h/cpp` | Player base class |
| `EC_HostPlayer.h/cpp` | Local player |
| `EC_NPC.h/cpp` | NPC system |
| `EC_ManPlayer.h/cpp` | Player manager |

---

## 7. File & Resource Systems

### PCK Archive System

```
AFilePackMan (Global manager)
    └─ AFilePackBase (Abstract interface)
        ├─ AFilePackage (Standard PCK)
        └─ AFilePackageMultiPart (.pck + .pkx)
```

### File Loading Priority

```
AFileImage::Open(filename)
    │
    ├─ 1. Check PCK packages
    │      └─ g_AFilePackMan.GetFilePck(relativePath)
    │          └─ OpenSharedFile() → Cached buffer
    │
    └─ 2. Fall back to disk
           └─ fopen() → Read to memory
```

### PCK File Format

```cpp
// Header (276 bytes per entry)
struct FILEENTRY {
    char szFileName[256];
    DWORD dwOffset;
    DWORD dwLength;           // Uncompressed
    DWORD dwCompressedLength;
};

// Encryption keys
AFPCK_GUARDBYTE0 = 0xfdfdfeee
AFPCK_GUARDBYTE1 = 0xf00dbeef
AFPCK_MASKDWORD  = 0xa8937462
```

### Resource Caching

```cpp
// Shared file cache (reference-counted)
struct SHAREDFILE {
    DWORD dwFileID;
    int iRefCnt;        // Reference counter
    BYTE* pFileData;
    DWORD dwFileLen;
};
```

### Current Configuration

**From ElementClient.ini:**
```ini
PackageFile=   # Empty = read from disk (PCK disabled)
```

All PCK files are **pre-extracted** to `/home/doug/Desktop/PWI_EN/element/`

---

## 8. Audio System

### Architecture (FMOD-based)

```
EventSystem (Singleton)
    └─ EventProject
        ├─ EventManager → Event → EventInstance
        ├─ AudioManager → AudioBank → Audio
        └─ SoundDefManager → SoundDef → SoundInstance
```

### Sound Playback Flow

```
Event::CreateInstance()
    └─ EventInstance::Start()
        └─ EventLayerInstance
            └─ SoundInstance
                └─ FMOD Channel
```

### Effect Types

```cpp
EFFECT_TYPE_VOLUME       EFFECT_TYPE_ECHO
EFFECT_TYPE_PAN          EFFECT_TYPE_PITCH
EFFECT_TYPE_SURROUND_PAN EFFECT_TYPE_OCCLUSION
EFFECT_TYPE_PARAMEQ      EFFECT_TYPE_COMPRESSOR
```

### Sound File Cache

```cpp
// AMSoundBufferMan manages:
- 2D sounds: m_aAuto2DSounds[]
- 3D sounds: m_aAuto3DGroups[]
- Cached sounds: ASCACHEITEM[]
```

---

## 9. Skill System

### Skill Data Structure

```cpp
class SkillStub {
    ID id;              // Unique identifier
    int cls;            // Required profession
    const wchar_t* name;
    const char* icon;
    int max_level;      // 1-10
    char type;          // Attack/Bless/Curse/etc.

    // Execution properties
    bool allow_ride;
    short attr;         // Element 1-6
    int apcost;
};
```

### Skill Types

```cpp
TYPE_ATTACK = 1      TYPE_PASSIVE = 5
TYPE_BLESS  = 2      TYPE_ENABLED = 6
TYPE_CURSE  = 3      TYPE_LIVE    = 7
TYPE_SUMMON = 4      TYPE_PRODUCE = 9
```

### Condition Checking

```cpp
int ElementSkill::Condition(ID id, UseRequirement& info, int level)
    // Returns: 0=success, 1-13=failure codes
    // Checks: MP, AP, form, weapon, location, combo, HP
```

### File Organization

- **3,324+ skill header files** (`skill1.h` - `skill1599.h`)
- Each contains SkillStub implementation
- Loaded at compile time

---

## 10. Network Layer

### Session Management

```cpp
class CECGameSession {
    // Protocol handling
    int AddNewProtocol(GNET::Protocol* p);
    bool ProcessNewProtocols();

    // Data transmission
    bool SendNetData(const GNET::Protocol& p, bool bUrg);
    bool SendGameData(void* pData, int iSize);
    bool SendChatData(char channel, const ACHAR* msg);
};
```

### Link States

```cpp
enum LinkBrokenReason {
    LBR_NONE,
    LBR_CHALLENGE_OVERTIME,
    LBR_ENTERWORLD_OVERTIME,
    LBR_ANTI_WALLOW,
    LBR_MAP_INVALID,
    LBR_PASSIVE_ONLINE,
    // ...
};
```

### Common Commands (C2S)

```cpp
c2s_CmdPlayerMove()      c2s_CmdCastSkill()
c2s_CmdStopMove()        c2s_CmdUseItem()
c2s_CmdSelectTarget()    c2s_CmdPickupItem()
c2s_CmdNormalAttack()    c2s_CmdLogout()
```

---

## 11. Current Issues & Debug Info

### Active Issue: Crash at Building Load

**Symptoms:**
- Client starts and initializes successfully
- Login world loads
- Main loop begins
- **Crash on first iteration** when `g_dwFatalErrorFlag == 1`

**Last Trace (BolaTrace.log):**
```
[21:31:21.531] Main loop iteration 1 starting
[21:31:21.531] Main loop 1: checking g_dwFatalErrorFlag=1
[CRASH at 0x00008002]
```

**Analysis:**
- `g_dwFatalErrorFlag = 1` = `FATAL_ERROR_LOAD_BUILDING`
- Crash address 0x00008002 = NULL pointer + offset

### Files Setting FATAL_ERROR_LOAD_BUILDING

| File | Line |
|------|------|
| `EC_HomeOrnament.cpp` | 72 |
| `EC_Ornament.cpp` | 185 |
| `EC_Matter.cpp` | 183, 191 |
| `EC_GameRun.cpp` | 2844 |
| `EC_RandomMapPreProcessor.cpp` | 1265 |

### Debug System (BOLA)

**Log Location:** `/home/doug/Desktop/PWI_EN/element/logs/BolaTrace.log`

**Macros:**
```cpp
BOLA_FUNC()              // Function entry/exit
BOLA_INFO(fmt, ...)      // Info message (printf-style)
BOLA_CHECKPOINT(msg)     // Single string checkpoint
BOLA_ERROR(fmt, ...)     // Error message
```

**Output Format:**
```
[HH:MM:SS.mmm] [LEVEL] File.cpp:line (Function): Message
```

### Debug Next Steps

1. Add BOLA_INFO before `a_LogOutput` in error handler
2. Check if `a_LogOutput` callback is crashing
3. Investigate which file sets the fatal error flag
4. Check building asset loading paths

### Deploy & Test

```bash
# Copy executable
cp /home/doug/153Client/build/ElementClient/elementclient.exe \
   /home/doug/Desktop/PWI_EN/element/

# Clear logs
rm -f /home/doug/Desktop/PWI_EN/element/logs/*.log

# Run
cd /home/doug/Desktop/PWI_EN/element
DISPLAY=:0 wine elementclient.exe startbypatcher 2>&1
```

---

## Appendix: Important Constants

### Fatal Error Flags

```cpp
FATAL_ERROR_LOAD_BUILDING    = 1
FATAL_ERROR_WRONG_CONFIGDATA = 2
```

### Move Environments

```cpp
MOVEENV_GROUND = 0
MOVEENV_WATER  = 1
MOVEENV_AIR    = 2
```

### Render Flags

```cpp
RD_NORMAL  = 0
RD_REFRACT = 1
RD_REFLECT = 2
```

---

*Document generated from codebase analysis - November 27, 2025*
