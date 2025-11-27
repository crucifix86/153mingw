# FreeType 2.1.10 library
# Simplified build for the modules typically used

set(FREETYPE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(freetype STATIC
    # Base
    ${FREETYPE_SRC_DIR}/base/ftsystem.c
    ${FREETYPE_SRC_DIR}/base/ftdebug.c
    ${FREETYPE_SRC_DIR}/base/ftinit.c
    ${FREETYPE_SRC_DIR}/base/ftbase.c
    ${FREETYPE_SRC_DIR}/base/ftbbox.c
    ${FREETYPE_SRC_DIR}/base/ftbdf.c
    ${FREETYPE_SRC_DIR}/base/ftbitmap.c
    ${FREETYPE_SRC_DIR}/base/ftglyph.c
    ${FREETYPE_SRC_DIR}/base/ftmm.c
    ${FREETYPE_SRC_DIR}/base/ftpfr.c
    ${FREETYPE_SRC_DIR}/base/ftstroke.c
    ${FREETYPE_SRC_DIR}/base/ftsynth.c
    ${FREETYPE_SRC_DIR}/base/fttype1.c
    ${FREETYPE_SRC_DIR}/base/ftwinfnt.c
    ${FREETYPE_SRC_DIR}/base/ftxf86.c

    # Rasterizers
    ${FREETYPE_SRC_DIR}/raster/raster.c
    ${FREETYPE_SRC_DIR}/smooth/smooth.c

    # Font drivers
    ${FREETYPE_SRC_DIR}/truetype/truetype.c
    ${FREETYPE_SRC_DIR}/type1/type1.c
    ${FREETYPE_SRC_DIR}/cff/cff.c
    ${FREETYPE_SRC_DIR}/cid/type1cid.c
    ${FREETYPE_SRC_DIR}/pfr/pfr.c
    ${FREETYPE_SRC_DIR}/type42/type42.c
    ${FREETYPE_SRC_DIR}/winfonts/winfnt.c
    ${FREETYPE_SRC_DIR}/pcf/pcf.c
    ${FREETYPE_SRC_DIR}/bdf/bdf.c

    # Auxiliary
    ${FREETYPE_SRC_DIR}/sfnt/sfnt.c
    ${FREETYPE_SRC_DIR}/autofit/autofit.c
    ${FREETYPE_SRC_DIR}/pshinter/pshinter.c
    ${FREETYPE_SRC_DIR}/psaux/psaux.c
    ${FREETYPE_SRC_DIR}/psnames/psnames.c
    ${FREETYPE_SRC_DIR}/cache/ftcache.c

    # Compression
    ${FREETYPE_SRC_DIR}/gzip/ftgzip.c
    ${FREETYPE_SRC_DIR}/lzw/ftlzw.c
)

target_include_directories(freetype PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(freetype PRIVATE
    WIN32
    FT2_BUILD_LIBRARY
)
