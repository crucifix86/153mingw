# PropDlg - Property dialog library
file(GLOB PROPDLG_SOURCES "*.cpp")

add_library(PropDlg STATIC ${PROPDLG_SOURCES})

target_include_directories(PropDlg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PropDlg PUBLIC AngelicaCommon)
target_compile_definitions(PropDlg PRIVATE WIN32)
