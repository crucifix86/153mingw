# jpeg library
add_library(jpeg STATIC
    Jcapimin.c
    Jcapistd.c
    Jccoefct.c
    Jccolor.c
    Jcdctmgr.c
    Jchuff.c
    Jcinit.c
    Jcmainct.c
    Jcmarker.c
    Jcmaster.c
    Jcomapi.c
    Jcparam.c
    Jcphuff.c
    Jcprepct.c
    Jcsample.c
    Jctrans.c
    Jdapimin.c
    Jdapistd.c
    Jdatadst.c
    Jdatasrc.c
    Jdcoefct.c
    Jdcolor.c
    Jddctmgr.c
    Jdhuff.c
    Jdinput.c
    Jdmainct.c
    Jdmarker.c
    Jdmaster.c
    Jdmerge.c
    Jdphuff.c
    Jdpostct.c
    Jdsample.c
    Jdtrans.c
    Jerror.c
    Jfdctflt.c
    Jfdctfst.c
    Jfdctint.c
    Jidctflt.c
    Jidctfst.c
    Jidctint.c
    Jidctred.c
    Jmemmgr.c
    Jmemnobs.c
    Jquant1.c
    Jquant2.c
    Jutils.c
)

target_include_directories(jpeg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(jpeg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(jpeg PRIVATE WIN32)
