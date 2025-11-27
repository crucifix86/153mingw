# mpg123 library
add_library(mpg123 STATIC
    common.cpp
    dct64_i386.cpp
    decode_2to1.cpp
    decode_4to1.cpp
    decode_i386.cpp
    decode_ntom.cpp
    equalizer.cpp
    getbits.cpp
    layer1.cpp
    layer2.cpp
    layer3.cpp
    mpg123.cpp
    readers.cpp
    tabinit.cpp
)

target_include_directories(mpg123 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mpg123 PRIVATE AngelicaCommon AngelicaFile)
target_compile_definitions(mpg123 PRIVATE WIN32)
