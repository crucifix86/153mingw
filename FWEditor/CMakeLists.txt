# FWEditor - Framework editor library
file(GLOB FWEDITOR_SOURCES "*.cpp")

add_library(FWEditor STATIC ${FWEDITOR_SOURCES})

target_include_directories(FWEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(FWEditor PUBLIC
    AngelicaCommon AngelicaFile Angelica3D AngelicaMedia
    AUInterface GfxCommon
)
target_compile_definitions(FWEditor PRIVATE WIN32 DX9 UNICODE _UNICODE _ANGELICA2 _FW_EDITOR)
