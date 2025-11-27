# ElementClient - Main game executable

# Collect all source files recursively
file(GLOB_RECURSE ELEMENTCLIENT_SOURCES
    "*.cpp"
    "*.cxx"
)

# Exclude some utility/test files if they exist
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*test.*\\.cpp$")

# Exclude ATL-dependent browser files (ATL not available with MinGW)
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*PWBrowser\\.cpp$")
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*DlgExplorer\\.cpp$")
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*DlgLoginOther\\.cpp$")

# Exclude BBS files - disabled feature with MSVC SEH and other incompatibilities
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*EC_BBSPage\\.cpp$")
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*EC_BBSPageFile\\.cpp$")
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*EC_BBSFileDownloader\\.cpp$")
# Also exclude dialogs that depend heavily on BBS functionality
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*DlgBBS\\.cpp$")
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*DlgLoginPage\\.cpp$")

# Exclude defence (anti-cheat) directory - Uses MSVC SEH, inline asm, and Windows-specific APIs
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*/defence/.*\\.cpp$")

# Exclude Discord SDK files - C++ type issues with MinGW
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*/discord/.*\\.cpp$")

# Exclude GT overlay files - ATL-dependent
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*/GT/.*\\.cpp$")

# Exclude HwLoader - Windows WMI-dependent
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*HwLoader\\.cpp$")

# Exclude Arc Asia SDK files - Publisher-specific SDK not available
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*EC_ArcAsia\\.cpp$")

# Exclude SpeedTree-dependent files - Proprietary library not available
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*EL_Tree\\.cpp$")
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*EL_Forest\\.cpp$")

# Exclude EC_GameUICommand.cpp - These methods are duplicated in EC_GameUIMan.cpp
# (MSVC project probably had special handling for this)
list(FILTER ELEMENTCLIENT_SOURCES EXCLUDE REGEX ".*EC_GameUICommand\\.cpp$")

add_executable(ElementClient WIN32 ${ELEMENTCLIENT_SOURCES})

target_include_directories(ElementClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CCommon
    ${CMAKE_CURRENT_SOURCE_DIR}/CCommon/AUTOCA
    ${CMAKE_CURRENT_SOURCE_DIR}/CCommon/Face
    ${CMAKE_CURRENT_SOURCE_DIR}/CCommon/Gfx
    ${CMAKE_CURRENT_SOURCE_DIR}/Network
    ${CMAKE_CURRENT_SOURCE_DIR}/Network/inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Network/IOLib
    ${CMAKE_CURRENT_SOURCE_DIR}/Task
    ${CMAKE_CURRENT_SOURCE_DIR}/Network/rpcdata
    ${CMAKE_CURRENT_SOURCE_DIR}/CrossServer
    ${CMAKE_CURRENT_SOURCE_DIR}/Home
    ${CMAKE_CURRENT_SOURCE_DIR}/defence
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(ElementClient PRIVATE
    # Engine libraries
    Angelica3D
    AngelicaMedia
    AngelicaFile
    AngelicaCommon

    # Support libraries
    GfxCommon
    FWEditor
    AUInterface
    AudioEngine
    # AudioAssist - Requires MFC, used for editor tools only
    LuaWrapper
    lua51
    CHBasedCD
    AutoMoveImp
    # ImageCacheLib  # MFC-based, requires afx.h
    ImmWrapper
    FTDriver
    freetype
    ElementSkill

    # Image libraries
    cximage
    png
    jpeg
    tiff
    zlib

    # Audio libraries
    ogg
    vorbis
    mpg123

    # DirectShow
    async
    strmbase

    # DirectX libraries
    d3d9
    d3dx9_43
    ddraw
    dsound
    dinput8
    dxguid

    # Windows libraries
    ws2_32
    winmm
    imm32
    version
    comctl32
    comdlg32
    ole32
    oleaut32
    uuid
    gdi32
    user32
    kernel32
    advapi32
    shell32
    shlwapi
    strmiids
    dbghelp
    opengl32
    glu32
    psapi
)

target_compile_definitions(ElementClient PRIVATE
    WIN32
    _WINDOWS
    UNICODE
    _UNICODE
    DX9
    _ELEMENTCLIENT
    ENABLE_SKILL
    RESOURCE_BATTLE
    LOG_PROTOCOL
    _NOMINIDUMP  # Disable SEH __try/__except (not supported by GCC)
)

# Set subsystem to Windows GUI
set_target_properties(ElementClient PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "elementclient"
)
