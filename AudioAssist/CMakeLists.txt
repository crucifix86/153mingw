# AudioAssist - Audio helper library
file(GLOB AUDIOASSIST_SOURCES "*.cpp")

add_library(AudioAssist STATIC ${AUDIOASSIST_SOURCES})

target_include_directories(AudioAssist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AudioAssist PUBLIC AudioEngine AngelicaCommon AngelicaFile)
target_compile_definitions(AudioAssist PRIVATE WIN32)
