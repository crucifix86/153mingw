# LuaWrapper - Lua scripting wrapper
file(GLOB LUAWRAPPER_SOURCES "*.cpp")

add_library(LuaWrapper STATIC ${LUAWRAPPER_SOURCES})

target_include_directories(LuaWrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(LuaWrapper PUBLIC lua51 AngelicaCommon AngelicaFile Angelica3D)
target_compile_definitions(LuaWrapper PRIVATE WIN32)
