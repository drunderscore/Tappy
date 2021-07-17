add_executable(Server
        main.cpp
        Client.cpp
        Server.cpp
        Scripting/Engine.cpp
        Scripting/Types.cpp
        Scripting/Format.cpp
        )

target_lagom(Server)
target_include_directories(Server SYSTEM PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        )

target_link_libraries(Server PRIVATE Terraria lua5.3)
