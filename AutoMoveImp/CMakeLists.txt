# AutoMoveImp - Auto movement implementation
file(GLOB AUTOMOVEIMP_SOURCES "*.cpp")

add_library(AutoMoveImp STATIC ${AUTOMOVEIMP_SOURCES})

target_include_directories(AutoMoveImp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AutoMoveImp PUBLIC AngelicaCommon AngelicaFile Angelica3D)
target_compile_definitions(AutoMoveImp PRIVATE WIN32)
