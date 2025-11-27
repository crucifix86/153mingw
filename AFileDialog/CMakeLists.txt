# AFileDialog - File dialog library
file(GLOB AFILEDIALOG_SOURCES "*.cpp")

add_library(AFileDialog STATIC ${AFILEDIALOG_SOURCES})

target_include_directories(AFileDialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AFileDialog PUBLIC AngelicaCommon comdlg32)
target_compile_definitions(AFileDialog PRIVATE WIN32)
