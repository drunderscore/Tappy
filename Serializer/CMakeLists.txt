add_executable(Serializer
        main.cpp
        ModelGenerator.cpp
        )
target_lagom(Serializer)
target_link_libraries(Serializer PRIVATE LagomMain)