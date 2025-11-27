# ElementSkill - Skill system library
file(GLOB ELEMENTSKILL_SOURCES "*.cpp")

add_library(ElementSkill STATIC ${ELEMENTSKILL_SOURCES})

target_include_directories(ElementSkill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ElementSkill PUBLIC AngelicaCommon AngelicaFile)
target_compile_definitions(ElementSkill PUBLIC WIN32 ELEMENTSKILL_EXPORTS)

# Source files are in GBK/GB2312 encoding (Chinese)
target_compile_options(ElementSkill PRIVATE -finput-charset=GBK -fexec-charset=GBK)
