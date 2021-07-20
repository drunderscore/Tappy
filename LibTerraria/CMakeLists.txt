file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Net/Packets/Modules)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Models)

function(compile_packet_definition source output)
    add_custom_command(
            OUTPUT ${output}
            COMMAND Serializer/Serializer -a packet_definition ${CMAKE_CURRENT_SOURCE_DIR}/Net/Packets/${source} > ${CMAKE_CURRENT_BINARY_DIR}/Net/Packets/${output}
            VERBATIM
            DEPENDS Serializer
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endfunction()

function(compile_item_model source output)
    add_custom_command(
            OUTPUT Models/${output}
            COMMAND Serializer/Serializer -a item_model ${CMAKE_CURRENT_SOURCE_DIR}/Models/${source} > ${CMAKE_CURRENT_BINARY_DIR}/Models/${output}
            VERBATIM
            DEPENDS Serializer
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endfunction()

function(compile_tile_model source output)
    add_custom_command(
            OUTPUT Models/${output}
            COMMAND Serializer/Serializer -a tile_model ${CMAKE_CURRENT_SOURCE_DIR}/Models/${source} > ${CMAKE_CURRENT_BINARY_DIR}/Models/${output}
            VERBATIM
            DEPENDS Serializer
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endfunction()

function(compile_wall_model source output)
    add_custom_command(
            OUTPUT Models/${output}
            COMMAND Serializer/Serializer -a wall_model ${CMAKE_CURRENT_SOURCE_DIR}/Models/${source} > ${CMAKE_CURRENT_BINARY_DIR}/Models/${output}
            VERBATIM
            DEPENDS Serializer
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
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

compile_item_model(items.json Items.cpp)
compile_tile_model(tiles.json Tiles.cpp)
compile_wall_model(walls.json Walls.cpp)

add_library(Terraria
        ${CMAKE_CURRENT_BINARY_DIR}/Models/Items.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/Models/Tiles.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/Models/Walls.cpp
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
        Player.cpp
        PlayerDeathReason.cpp
        Net/NetworkText.cpp
        Character.cpp
        TileMap.cpp
        PlayerDeath.h
        SpawnData.h
        ModifyTile.h
        PlayerTeam.h
        SyncTalkNPC.h
        SyncTilePicking.h
        SpawnPlayer.h
        AddPlayerBuff.h
        PlayerBuffs.h
        TileModification.cpp
        PlayerItemAnimation.h
        ClientUUID.h
        PlayerHP.h
        Net/Packets/SyncItem.cpp
        Net/Packets/SyncInventorySlot.cpp
        SyncItemOwner.h
        PlayerMana.h
        Modules/Text.h
        PlayerActive.h
        PlayerDead.h
        World.cpp
        FileMetadata.cpp
        Tile.cpp
        )

target_include_directories(Terraria SYSTEM PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        )

target_lagom(Terraria)
