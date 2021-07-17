cmake_minimum_required(VERSION 3.17)
project(Tappy CXX)

set(CMAKE_CXX_STANDARD 20)

function(target_lagom target)
    target_include_directories(${target} SYSTEM PRIVATE
            # This is pretty much solely for AK
            ${CMAKE_SOURCE_DIR}/serenity/
            ${CMAKE_SOURCE_DIR}/serenity/Userland/Libraries
            )
    target_link_libraries(${target} PRIVATE Lagom)
endfunction()

set(BUILD_LAGOM ON)
include_directories(BEFORE SYSTEM ${CMAKE_BINARY_DIR}/serenity/Meta/Lagom/)
add_subdirectory(serenity/Meta/Lagom)
target_compile_options(Lagom PUBLIC -fno-gnu-keywords)
add_subdirectory(Serializer)
add_subdirectory(LibTerraria)
add_subdirectory(Server)
