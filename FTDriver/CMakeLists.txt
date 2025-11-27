# FTDriver - FreeType font driver
file(GLOB FTDRIVER_SOURCES "*.cpp")

add_library(FTDriver STATIC ${FTDRIVER_SOURCES})

target_include_directories(FTDriver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/FREETYPE-2.1.10/include
    ${CMAKE_SOURCE_DIR}/FREETYPE-2.1.10/include/freetype
)

target_link_libraries(FTDriver PUBLIC freetype)
target_compile_definitions(FTDriver PUBLIC WIN32 FTDRIVERDLL_EXPORTS)
