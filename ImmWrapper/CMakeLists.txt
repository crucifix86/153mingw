# ImmWrapper - IME wrapper
file(GLOB IMMWRAPPER_SOURCES "Source/*.cpp")

add_library(ImmWrapper STATIC ${IMMWRAPPER_SOURCES})

target_include_directories(ImmWrapper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/header
)

target_link_libraries(ImmWrapper PUBLIC AngelicaCommon AngelicaMedia imm32)
target_compile_definitions(ImmWrapper PUBLIC WIN32 IMMWRAPPER_EXPORTS)
