add_executable(Serializer
        main.cpp
        ModelGenerator.cpp
        )
target_link_libraries(Serializer PRIVATE Lagom::Core Lagom::Main)