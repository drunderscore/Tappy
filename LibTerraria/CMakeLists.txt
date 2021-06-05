file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Net/Packets/Modules)

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

file(GLOB packet_definitions ${CMAKE_CURRENT_SOURCE_DIR}/Net/Packets/Modules/*.json)
foreach(file ${packet_definitions})
    get_filename_component(file_name ${file} NAME_WE)
    compile_packet_definition(Modules/${file_name}.json Modules/${file_name}.h)
endforeach()

add_library(Terraria
        Item.cpp
        Color.cpp
        Net/Packet.cpp
        Tile.h
        PlayerInventory.cpp
        ConnectRequest.h
        SpawnPlayerSelf.h
        Net/Packets/SyncProjectile.cpp
        Net/Packets/SyncNPC.cpp
        TogglePvp.h
        ReleaseNPC.h
        KillProjectile.h
        Net/Packets/SyncPlayer.cpp
        TileFrameSection.h
        ConnectFinished.h
        PlayerHurt.h
        Disconnect.h
        Net/Packets/TeleportEntity.cpp
        Net/Packets/TileSection.cpp
        Net/Packets/SyncTileRect.cpp
        SetUserSlot.h
        PlayerInfo.h
        DamageNPC.h
        WorldData.h
        PlayerDeathReason.cpp
        Net/NetworkText.cpp
        Character.cpp
        TileMap.cpp
        PlayerDeath.h
        SpawnData.h
        ModifyTile.h
        SyncTilePicking.h
        SpawnPlayer.h
        PlayerBuffs.h
        TileModification.cpp
        PlayerItemAnimation.h
        ClientUUID.h
        PlayerHP.h
        Net/Packets/SyncInventorySlot.cpp
        PlayerMana.h
        Modules/Text.h
        PlayerActive.h
        PlayerDead.h
        World.cpp
        FileMetadata.cpp
        )

target_include_directories(Terraria SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        )

target_lagom(Terraria)