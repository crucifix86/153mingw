# ImageCacheLib - Image caching library
file(GLOB IMAGECACHELIB_SOURCES "*.cpp")

add_library(ImageCacheLib STATIC ${IMAGECACHELIB_SOURCES})

target_include_directories(ImageCacheLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ImageCacheLib PUBLIC
    AngelicaCommon AngelicaFile Angelica3D
    GfxCommon ImmWrapper cximage LuaWrapper lua51
)
target_compile_definitions(ImageCacheLib PRIVATE WIN32 DX9)
