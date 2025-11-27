# 153Client - Perfect World International Client
# Cross-compilation build system for MinGW-w64
cmake_minimum_required(VERSION 3.16)
project(153Client VERSION 1.53 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)  # C++14 to avoid std::byte conflicts with Windows byte
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Include our custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(FindDirectX)

# Global compiler flags for MinGW
if(MINGW)
    add_compile_options(-w)  # Suppress warnings initially
    add_compile_options(-fpermissive)  # Allow some non-standard code
    add_compile_options(-msse2)  # Enable SSE2 for FastMemcpy.h
    add_compile_definitions(_WIN32_WINNT=0x0501)  # Windows XP+
endif()

# ============================================================================
# Third-Party Libraries (Phase 4)
# ============================================================================
add_subdirectory(zlib)
add_subdirectory(zliblib)
add_subdirectory(jpeg)
add_subdirectory(png)
add_subdirectory(tiff)
add_subdirectory(ogg)
add_subdirectory(vorbis)
add_subdirectory(lua5.1)
add_subdirectory(mpg123)
add_subdirectory(cximage)
add_subdirectory(FREETYPE-2.1.10)

# ============================================================================
# Angelica Engine Core (Phase 5)
# ============================================================================
add_subdirectory(AngelicaCommon)
add_subdirectory(AngelicaFile)
add_subdirectory(AngelicaMedia)
add_subdirectory(Angelica3D)

# ============================================================================
# DirectShow Support (Phase 6/7)
# ============================================================================
add_subdirectory(strmbase)
add_subdirectory(async)

# ============================================================================
# Audio System (Phase 6)
# ============================================================================
add_subdirectory(AudioEngine)
# add_subdirectory(AudioAssist)  # MFC-based, editor tool only

# ============================================================================
# Support Libraries (Phase 7)
# ============================================================================
add_subdirectory(FTDriver)
add_subdirectory(CHBasedCD)
add_subdirectory(ImmWrapper)
add_subdirectory(LuaWrapper)
add_subdirectory(AutoMoveImp)
# add_subdirectory(PropDlg)  # MFC-based, editor tool only
# add_subdirectory(AFileDialog)  # MFC-based, editor tool only
add_subdirectory(GfxCommon)
add_subdirectory(AUInterface)
add_subdirectory(FWEditor)
# add_subdirectory(ImageCacheLib)  # MFC-based, requires afx.h

# ============================================================================
# Game Libraries (Phase 8)
# ============================================================================
add_subdirectory(ElementSkill)

# ============================================================================
# Main Executable (Phase 9)
# ============================================================================
add_subdirectory(ElementClient)

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "=== 153Client Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Target: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "")
