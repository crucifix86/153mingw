# libpng library
add_library(png STATIC
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngvcrd.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

target_include_directories(png PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(png PRIVATE zlib)
target_compile_definitions(png PRIVATE WIN32)
