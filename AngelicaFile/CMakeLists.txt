# AngelicaFile - File system and packaging library
add_library(AngelicaFile STATIC
    Source/ADir.cpp
    Source/ADirImage.cpp
    Source/AFI.cpp
    Source/AFile.cpp
    Source/AFileImage.cpp
    Source/AFilePackBase.cpp
    Source/AFilePackGame.cpp
    Source/AFilePackMan.cpp
    Source/AFilePackageMultiPart.cpp
    Source/AFileUnicode.cpp
    Source/AIniFile.cpp
    Source/AMemFile.cpp
    Source/AScriptFile.cpp
    Source/ATextTabFile.cpp
    Source/AWIniFile.cpp
    Source/AWScriptFile.cpp
    Source/afilepackage.cpp
)

target_include_directories(AngelicaFile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/header
)

target_link_libraries(AngelicaFile PUBLIC AngelicaCommon zlib)
target_compile_definitions(AngelicaFile PRIVATE WIN32 UNICODE _UNICODE)
