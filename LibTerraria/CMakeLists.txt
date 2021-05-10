file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Net/Packets)

function(compile_packet_definition source output)
    add_custom_command(
            OUTPUT ${output}
            COMMAND Serializer/Serializer -a packet_definition ${CMAKE_CURRENT_SOURCE_DIR}/Net/Packets/${source} > ${CMAKE_CURRENT_BINARY_DIR}/Net/Packets/${output}
            VERBATIM
            DEPENDS Serializer
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endfunction()

file(GLOB packet_definitions ${CMAKE_CURRENT_SOURCE_DIR}/Net/Packets/*.json)
foreach(file ${packet_definitions})
    get_filename_component(file_name ${file} NAME_WE)
    compile_packet_definition(${file_name}.json ${file_name}.h)
endforeach()

add_library(Terraria
        Item.cpp
        Color.cpp
        Net/Packet.cpp
        Tile.h
        PlayerInventory.cpp
        ConnectRequest.h
        SpawnPlayerSelf.h
        Point.cpp
        Net/Packets/SyncProjectile.cpp
        Net/Packets/SyncPlayer.cpp
        TileFrameSection.h
        ConnectFinished.h
        Disconnect.h
        Net/Packets/TileSection.cpp
        SetUserSlot.h
        PlayerInfo.h
        WorldData.h
        SpawnData.h
        SpawnPlayer.h
        PlayerBuffs.h
        ClientUUID.h
        PlayerHP.h
        PlayerMana.h
        SyncInventorySlot.h
        )

target_include_directories(Terraria SYSTEM PRIVATE ${CMAKE_SOURCE_DIR})

target_lagom(Terraria)