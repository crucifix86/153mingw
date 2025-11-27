#!/bin/bash
# 153Client Linux Cross-Compilation Build Script
# Builds Windows executable using MinGW-w64

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
LOG_DIR="${SCRIPT_DIR}/logs"
BUILD_TYPE="${1:-Release}"
TARGET="${2:-all}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

timestamp() {
    date "+%Y-%m-%d %H:%M:%S"
}

log() {
    echo -e "[$(timestamp)] $1"
}

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  153Client Cross-Compilation Build${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Create directories
mkdir -p "${BUILD_DIR}"
mkdir -p "${LOG_DIR}"

# Check for MinGW
if ! command -v i686-w64-mingw32-g++ &> /dev/null; then
    echo -e "${RED}ERROR: MinGW-w64 (32-bit) not found!${NC}"
    echo "Install with: sudo apt install mingw-w64"
    exit 1
fi
log "${GREEN}[✓]${NC} MinGW-w64 found"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}ERROR: CMake not found!${NC}"
    echo "Install with: sudo apt install cmake"
    exit 1
fi
log "${GREEN}[✓]${NC} CMake found"

cd "${BUILD_DIR}"

# Configure if needed
if [ ! -f "Makefile" ] || [ "$TARGET" == "configure" ]; then
    log "${YELLOW}[*]${NC} Running CMake configuration..."
    cmake \
        -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/toolchain-mingw32.cmake" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -G "Unix Makefiles" \
        "${SCRIPT_DIR}" 2>&1 | tee "${LOG_DIR}/cmake_config.log"

    if [ ${PIPESTATUS[0]} -ne 0 ]; then
        echo -e "${RED}CMake configuration failed! Check ${LOG_DIR}/cmake_config.log${NC}"
        exit 1
    fi
    log "${GREEN}[✓]${NC} CMake configuration complete"
fi

if [ "$TARGET" == "configure" ]; then
    exit 0
fi

# Number of parallel jobs
NPROC=$(nproc)
log "${YELLOW}[*]${NC} Building with ${NPROC} parallel jobs..."

# Build targets in order for better error isolation
PHASE4_LIBS="zlib zliblib jpeg png tiff ogg vorbis lua51 mpg123 cximage freetype"
PHASE5_LIBS="AngelicaCommon AngelicaFile AngelicaMedia Angelica3D"
PHASE6_LIBS="strmbase async AudioEngine AudioAssist"
PHASE7_LIBS="FTDriver CHBasedCD ImmWrapper LuaWrapper AutoMoveImp PropDlg AFileDialog GfxCommon AUInterface FWEditor ImageCacheLib"
PHASE8_LIBS="ElementSkill"
PHASE9="ElementClient"

build_phase() {
    local phase_name="$1"
    local targets="$2"
    local log_file="${LOG_DIR}/build_${phase_name}.log"

    log "${YELLOW}[*]${NC} Building ${phase_name}..."

    if make -j${NPROC} ${targets} 2>&1 | tee "${log_file}"; then
        log "${GREEN}[✓]${NC} ${phase_name} complete"
        return 0
    else
        log "${RED}[✗]${NC} ${phase_name} FAILED - check ${log_file}"
        return 1
    fi
}

case "$TARGET" in
    all)
        build_phase "Phase4-ThirdParty" "$PHASE4_LIBS" || exit 1
        build_phase "Phase5-AngelicaCore" "$PHASE5_LIBS" || exit 1
        build_phase "Phase6-Audio" "$PHASE6_LIBS" || exit 1
        build_phase "Phase7-Support" "$PHASE7_LIBS" || exit 1
        build_phase "Phase8-GameLibs" "$PHASE8_LIBS" || exit 1
        build_phase "Phase9-ElementClient" "$PHASE9" || exit 1
        ;;
    phase4)
        build_phase "Phase4-ThirdParty" "$PHASE4_LIBS" || exit 1
        ;;
    phase5)
        build_phase "Phase5-AngelicaCore" "$PHASE5_LIBS" || exit 1
        ;;
    phase6)
        build_phase "Phase6-Audio" "$PHASE6_LIBS" || exit 1
        ;;
    phase7)
        build_phase "Phase7-Support" "$PHASE7_LIBS" || exit 1
        ;;
    phase8)
        build_phase "Phase8-GameLibs" "$PHASE8_LIBS" || exit 1
        ;;
    phase9)
        build_phase "Phase9-ElementClient" "$PHASE9" || exit 1
        ;;
    clean)
        log "${YELLOW}[*]${NC} Cleaning build directory..."
        rm -rf "${BUILD_DIR}"/*
        log "${GREEN}[✓]${NC} Clean complete"
        exit 0
        ;;
    *)
        # Build specific target
        log "${YELLOW}[*]${NC} Building target: ${TARGET}"
        make -j${NPROC} ${TARGET} 2>&1 | tee "${LOG_DIR}/build_${TARGET}.log"
        ;;
esac

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  BUILD COMPLETE${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Logs are in: ${LOG_DIR}/"
echo ""
if [ -f "${BUILD_DIR}/ElementClient/elementclient.exe" ]; then
    echo "Output: ${BUILD_DIR}/ElementClient/elementclient.exe"
    echo ""
    echo "To run with Wine:"
    echo "  wine ${BUILD_DIR}/ElementClient/elementclient.exe"
fi
