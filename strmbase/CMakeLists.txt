# strmbase - DirectShow base classes
add_library(strmbase STATIC
    amextra.cpp
    amfilter.cpp
    amvideo.cpp
    arithutil.cpp
    combase.cpp
    cprop.cpp
    ctlutil.cpp
    ddmm.cpp
    dllentry.cpp
    dllsetup.cpp
    mtype.cpp
    outputq.cpp
    perflog.cpp
    pstream.cpp
    pullpin.cpp
    refclock.cpp
    renbase.cpp
    schedule.cpp
    seekpt.cpp
    source.cpp
    strmctl.cpp
    sysclock.cpp
    transfrm.cpp
    transip.cpp
    videoctl.cpp
    vtrans.cpp
    winctrl.cpp
    winutil.cpp
    wxdebug.cpp
    wxlist.cpp
    wxutil.cpp
)

target_include_directories(strmbase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include_fix
)
target_compile_definitions(strmbase PRIVATE WIN32 UNICODE _UNICODE)
# Force-include SAL compatibility header for MinGW
target_compile_options(strmbase PRIVATE -include ${CMAKE_SOURCE_DIR}/include_fix/sal_fix.h)
target_link_libraries(strmbase PRIVATE ole32 oleaut32 strmiids)
