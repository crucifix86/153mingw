# async - Asynchronous file reader filter
add_library(async STATIC
    asyncflt.cpp
    asyncio.cpp
    asyncrdr.cpp
    memfile.cpp
)

target_include_directories(async PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(async PUBLIC strmbase)
target_compile_definitions(async PRIVATE WIN32 UNICODE _UNICODE)
