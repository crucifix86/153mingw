# CHBasedCD - Collision detection library
file(GLOB CHBASEDCD_SOURCES "*.cpp")

add_library(CHBasedCD STATIC ${CHBASEDCD_SOURCES})

target_include_directories(CHBasedCD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CHBasedCD PUBLIC AngelicaCommon AngelicaFile Angelica3D)
target_compile_definitions(CHBasedCD PRIVATE WIN32 DX9)
