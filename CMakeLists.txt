cmake_minimum_required(VERSION 3.17)
project(Tappy CXX)

set(CMAKE_CXX_STANDARD 20)

function(target_serenity target)
    target_include_directories(${target} SYSTEM PRIVATE
            # This is pretty much solely for AK
            ${CMAKE_SOURCE_DIR}/serenity/
            ${CMAKE_SOURCE_DIR}/serenity/Userland/Libraries
            # We need this for the generated Debug.h
            ${CMAKE_SOURCE_DIR}/serenity/BuildLagom
            )
    target_link_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/serenity/BuildLagom)
    target_link_libraries(${target} PRIVATE LagomCore)

endfunction()

add_subdirectory(Serializer)
add_subdirectory(LibTerraria)
add_subdirectory(Server)
