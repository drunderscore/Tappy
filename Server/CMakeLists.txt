add_executable(Server
        main.cpp
        Client.cpp
        )

target_serenity(Server)
target_include_directories(Server SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        )
target_link_libraries(Server PRIVATE Terraria)