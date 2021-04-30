add_executable(Serializer main.cpp)
target_include_directories(Serializer SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}/serenity/
        ${CMAKE_SOURCE_DIR}/serenity/Userland/Libraries
        ${CMAKE_SOURCE_DIR}/serenity/BuildLagom
        )
target_link_directories(Serializer PRIVATE ${CMAKE_SOURCE_DIR}/serenity/BuildLagom)
target_link_libraries(Serializer PRIVATE LagomCore)
