# libogg library
add_library(ogg STATIC
    bitwise.c
    framing.c
)

target_include_directories(ogg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ogg PRIVATE WIN32)
