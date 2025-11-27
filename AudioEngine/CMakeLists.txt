# AudioEngine - FMOD-based audio system
add_library(AudioEngine STATIC
    FAudio.cpp
    FAudioBank.cpp
    FAudioBankArchive.cpp
    FAudioContainer.cpp
    FAudioEngine.cpp
    FAudioGroup.cpp
    FAudioInstance.cpp
    FAudioInstanceManager.cpp
    FAudioManager.cpp
    FAudioTemplate.cpp
    FCriticalSectionWrapper.cpp
    FCurve.cpp
    FEffect.cpp
    FEffectFactory.cpp
    FEvent.cpp
    FEventGroup.cpp
    FEventInstance.cpp
    FEventLayer.cpp
    FEventLayerInstance.cpp
    FEventManager.cpp
    FEventParameter.cpp
    FEventProject.cpp
    FEventSystem.cpp
    FGuid.cpp
    FIntervalManager.cpp
    FLog.cpp
    FLogManager.cpp
    FPropCurves.cpp
    FReverb.cpp
    FReverbDataManager.cpp
    FSoundDef.cpp
    FSoundDefArchive.cpp
    FSoundDefManager.cpp
    FSoundInstance.cpp
    FSoundInstanceTemplate.cpp
    FSoundPlayList.cpp
    XML/tinystr.cpp
    XML/tinyxml.cpp
    XML/tinyxmlerror.cpp
    XML/tinyxmlparser.cpp
)

target_include_directories(AudioEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/fmod/include
)

target_link_libraries(AudioEngine PUBLIC
    AngelicaCommon
    AngelicaFile
    AngelicaMedia
    Angelica3D
)

target_compile_definitions(AudioEngine PRIVATE WIN32)
