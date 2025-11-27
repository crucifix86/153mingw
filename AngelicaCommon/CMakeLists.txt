# AngelicaCommon - Core utility library
add_library(AngelicaCommon STATIC
    Source/AAssist.cpp
    Source/ACounter.cpp
    Source/AList.cpp
    Source/ALog.cpp
    Source/AMemory.cpp
    Source/AMemoryDbg.cpp
    Source/AMiniDump.cpp
    Source/APerlinNoise1D.cpp
    Source/APerlinNoise2D.cpp
    Source/APerlinNoise3D.cpp
    Source/APerlinNoiseBase.cpp
    Source/ARandomGen.cpp
    Source/AString.cpp
    Source/ATempMemMan.cpp
    Source/ATime.cpp
    Source/AWString.cpp
    Source/jexception.cpp
)

target_include_directories(AngelicaCommon PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/header
)

target_compile_definitions(AngelicaCommon PRIVATE WIN32 UNICODE _UNICODE)
