# AUInterface - Audio/UI interface library
file(GLOB AUINTERFACE_SOURCES "*.cpp")

add_library(AUInterface STATIC ${AUINTERFACE_SOURCES})

target_include_directories(AUInterface PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ElementClient/CCommon
)
target_link_libraries(AUInterface PUBLIC
    AngelicaCommon AngelicaFile Angelica3D AngelicaMedia
    LuaWrapper lua51 GfxCommon AudioEngine ImmWrapper
)
target_compile_definitions(AUInterface PRIVATE WIN32 DX9 UNICODE _UNICODE _ANGELICA2 FTDRIVERDLL_EXPORTS IMMWRAPPER_EXPORTS)
