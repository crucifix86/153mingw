# Cross-compilation toolchain for 32-bit Windows using MinGW-w64
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw32.cmake ..

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR i686)

# Specify the cross-compiler (use posix variant for C++11 threading support)
set(CMAKE_C_COMPILER i686-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++-posix)
set(CMAKE_RC_COMPILER i686-w64-mingw32-windres)

# Where to look for target environment (leave empty to use compiler defaults)
set(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32)

# Adjust behavior of find_* commands
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Default to static linking for cleaner Wine compatibility
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# Windows-specific defines
add_definitions(-DWIN32 -D_WINDOWS -D_WIN32_WINNT=0x0501)

# Add include_fix directory for case-insensitive header symlinks
include_directories(BEFORE SYSTEM "${CMAKE_SOURCE_DIR}/include_fix")
