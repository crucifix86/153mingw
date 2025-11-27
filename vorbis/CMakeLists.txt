# libvorbis library
add_library(vorbis STATIC
    analysis.c
    bitrate.c
    block.c
    codebook.c
    envelope.c
    floor0.c
    floor1.c
    info.c
    lookup.c
    lpc.c
    lsp.c
    mapping0.c
    mdct.c
    psy.c
    registry.c
    res0.c
    sharedbook.c
    smallft.c
    synthesis.c
    vorbisfile.c
    window.c
)

target_include_directories(vorbis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vorbis PUBLIC ogg)
target_compile_definitions(vorbis PRIVATE WIN32)
