cmake_minimum_required(VERSION 3.16)
project(Tappy CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
include(cmake/FetchLagom.cmake)

add_compile_options(-Werror=implicit-fallthrough)
add_compile_options(-Wno-literal-suffix)
add_subdirectory(Serializer)
add_subdirectory(LibTerraria)
add_subdirectory(Server)
