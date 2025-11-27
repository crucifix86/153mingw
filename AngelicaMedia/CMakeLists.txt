# AngelicaMedia - Audio/Video playback library
add_library(AngelicaMedia STATIC
    Source/AM3DSoundBuffer.cpp
    Source/AM3DSoundDevice.cpp
    Source/AMConfig.cpp
    Source/AMEngine.cpp
    Source/AMMp3File.cpp
    Source/AMOggFile.cpp
    Source/AMPI.cpp
    Source/AMSoundBuffer.cpp
    Source/AMSoundBufferMan.cpp
    Source/AMSoundDevice.cpp
    Source/AMSoundEngine.cpp
    Source/AMSoundStream.cpp
    Source/AMSurfaceRenderer.cpp
    Source/AMVideoClip.cpp
    Source/AMVideoEngine.cpp
    Source/AMWaveFile.cpp
    Source/FFT.cpp
)

target_include_directories(AngelicaMedia PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/header
    ${CMAKE_SOURCE_DIR}/Angelica3D/Header
    ${CMAKE_SOURCE_DIR}/mpg123
    ${CMAKE_SOURCE_DIR}/ImmWrapper/Header
    ${CMAKE_SOURCE_DIR}/strmbase
    ${CMAKE_SOURCE_DIR}/async
)

target_link_libraries(AngelicaMedia PUBLIC
    AngelicaCommon
    AngelicaFile
    ogg
    vorbis
    mpg123
)

target_compile_definitions(AngelicaMedia PRIVATE WIN32 UNICODE _UNICODE DX9 IMMWRAPPER_EXPORTS)
