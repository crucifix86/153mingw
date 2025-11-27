# GfxCommon - Graphics common library
file(GLOB GFXCOMMON_SOURCES "*.cpp" "*.CPP")

add_library(GfxCommon STATIC ${GFXCOMMON_SOURCES})

target_include_directories(GfxCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GfxCommon PUBLIC
    LuaWrapper lua51 ImmWrapper CHBasedCD
    AngelicaCommon AngelicaFile Angelica3D AngelicaMedia
)
target_compile_definitions(GfxCommon PRIVATE WIN32 DX9 UNICODE _UNICODE)
