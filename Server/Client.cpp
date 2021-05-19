/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Client.h>
#include <Server/Server.h>
#include <LibTerraria/Net/NetworkText.h>
#include <LibTerraria/Net/Packets/SetUserSlot.h>
#include <LibTerraria/Net/Packets/PlayerInfo.h>
#include <LibTerraria/Net/Packets/SyncInventorySlot.h>
#include <LibTerraria/Net/Packets/ClientUUID.h>
#include <LibTerraria/Net/Packets/PlayerHP.h>
#include <LibTerraria/Net/Packets/PlayerMana.h>
#include <LibTerraria/Net/Packets/WorldData.h>
#include <LibTerraria/Net/Packets/PlayerBuffs.h>
#include <LibTerraria/Net/Packets/SpawnData.h>
#include <LibTerraria/Net/Packets/TileSection.h>
#include <LibTerraria/Net/Packets/SpawnPlayer.h>
#include <LibTerraria/Net/Packets/ConnectRequest.h>
#include <LibTerraria/Net/Packets/SpawnPlayerSelf.h>
#include <LibTerraria/Net/Packets/ConnectFinished.h>
#include <LibTerraria/Net/Packets/TileFrameSection.h>
#include <LibTerraria/Net/Packets/SyncPlayer.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/SyncNPC.h>
#include <LibTerraria/Net/Packets/ReleaseNPC.h>
#include <LibTerraria/Net/Packets/KillProjectile.h>
#include <LibTerraria/Net/Packets/Modules/Text.h>
#include <LibTerraria/Net/Packets/Disconnect.h>
#include <math.h>

Client::Client(NonnullRefPtr<Core::TCPSocket> socket, Server& server, u8 id) :
        m_socket(move(socket)),
        m_id(id),
        m_input_stream(m_socket),
        m_output_stream(m_socket),
        m_server(server)
{
    m_socket->on_ready_to_read = [this]()
    {
        on_ready_to_read();
    };
}

void Client::send(const Terraria::Net::Packet& packet)
{
    auto bytes = packet.to_bytes();
    m_output_stream << static_cast<u16>(bytes.size() + 2);
    m_output_stream << bytes;
}

void Client::disconnect(const Terraria::Net::NetworkText& reason)
{
    Terraria::Net::Packets::Disconnect disconnect;
    disconnect.set_reason(reason);
    send(disconnect);
    if (on_disconnect)
        on_disconnect(DisconnectReason::DisconnectedByServer);
}

void Client::on_ready_to_read()
{
    if (m_socket->eof())
    {
        if (on_disconnect)
            on_disconnect(DisconnectReason::EofReached);
        return;
    }

    u16 packet_size;
    m_input_stream >> packet_size;

    if (m_input_stream.has_any_error())
    {
        warnln("Stream errored trying to read packet size");
        m_input_stream.handle_any_error();
        return;
    }

    Terraria::Net::Packet::Id packet_id;
    m_input_stream >> packet_id;
    if (m_input_stream.has_any_error())
    {
        warnln("Stream errored trying to read packet id");
        m_input_stream.handle_any_error();
        return;
    }

    // The packet size (2 bytes) and id (1 byte) are part of this.
    auto bytes = m_socket->read(packet_size - 3);
    InputMemoryStream packet_bytes_stream(bytes);

    // TODO: Some of these packets contain the player id, but we ignore that and assume it's the player id we assigned to this socket.

    // Connection request, let's send a user slot
    if (packet_id == Terraria::Net::Packet::Id::ConnectRequest)
    {
        auto request = Terraria::Net::Packets::ConnectRequest::from_bytes(packet_bytes_stream);

        m_server.client_did_connect_request({}, *this, request->version());

        Terraria::Net::Packets::SetUserSlot set_user_slot;
        set_user_slot.set_player_id(m_id);
        send(set_user_slot);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerInfo)
    {
        auto player_info = Terraria::Net::Packets::PlayerInfo::from_bytes(packet_bytes_stream);
        m_player.character().update_from_packet(*player_info);
        outln("Got character, created player for {}", m_player.character().name());
        m_server.client_did_send_player_info({}, *this, *player_info);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncInventorySlot)
    {
        auto inv_slot = Terraria::Net::Packets::SyncInventorySlot::from_bytes(packet_bytes_stream);
        if (inv_slot->id() != Terraria::Item::Id::None)
        {
            m_player.inventory().insert(inv_slot->slot(),
                                        Terraria::Item(inv_slot->id(), inv_slot->prefix(), inv_slot->stack()));
        }
        m_server.client_did_sync_inventory_slot({}, *this, *inv_slot);
    }
    else if (packet_id == Terraria::Net::Packet::Id::RequestWorldData)
    {
        Terraria::Net::Packets::WorldData world_data;
        world_data.set_max_tiles_x(4200);
        world_data.set_max_tiles_y(1200);
        world_data.set_world_surface(400);
        world_data.set_world_name("lol im cool");
        world_data.set_spawn_x(100);
        world_data.set_spawn_y(200);
        world_data.set_time(8000);
        world_data.set_day_state(1);
        world_data.set_world_flags_1(world_data.world_flags_1() | 0b0100'0000);
        send(world_data);

        m_server.client_did_request_world_data({}, *this);
    }
    else if (packet_id == Terraria::Net::Packet::Id::ClientUUID)
    {
        auto client_uuid = Terraria::Net::Packets::ClientUUID::from_bytes(packet_bytes_stream);
        if (client_uuid->uuid().length() != 36)
            warnln("Client sent UUID that isn't 36 characters.");
        else
            m_uuid = UUID(client_uuid->uuid().view());
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerHP)
    {
        auto player_hp = Terraria::Net::Packets::PlayerHP::from_bytes(packet_bytes_stream);
        m_player.set_hp(player_hp->hp());
        m_player.set_max_hp(player_hp->max_hp());
        m_server.client_did_sync_hp({}, *this, *player_hp);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerBuffs)
    {
        auto player_buffs = Terraria::Net::Packets::PlayerBuffs::from_bytes(packet_bytes_stream);
        player_buffs->buffs().span().copy_to(m_player.buffs().span());
        m_server.client_did_sync_buffs({}, *this, *player_buffs);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerMana)
    {
        auto player_mana = Terraria::Net::Packets::PlayerMana::from_bytes(packet_bytes_stream);
        m_player.set_mana(player_mana->mana());
        m_player.set_max_mana(player_mana->max_mana());
        m_server.client_did_sync_mana({}, *this, *player_mana);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SpawnData)
    {
        auto spawn_data = Terraria::Net::Packets::SpawnData::from_bytes(packet_bytes_stream);
        static constexpr u16 width = 400;
        static constexpr u16 height = 50;
        static constexpr i32 starting_x = 41;
        static constexpr i32 starting_y = 200;
        Terraria::TileMap tiles(width, height);

        Terraria::Tile stone;
        stone.id() = Terraria::Tile::Id::Stone;
        Terraria::Tile dirt;
        dirt.id() = Terraria::Tile::Id::Dirt;
        Terraria::Tile grass;
        grass.id() = Terraria::Tile::Id::Grass;
        Terraria::Tile obsidian;
        obsidian.id() = Terraria::Tile::Id::Obsidian;

        for (auto y = 0; y < tiles.height(); y++)
        {
            for (auto x = 0; x < tiles.width(); x++)
            {
                if (y == 0)
                    tiles.at(x, y) = grass;
                else if (y == height - 1)
                    tiles.at(x, y) = obsidian;
                else if (y > 3)
                    tiles.at(x, y) = stone;
                else
                    tiles.at(x, y) = dirt;
            }
        }

        Terraria::Net::Packets::TileSection section(tiles, starting_x, starting_y);
        send(section);

        Terraria::Net::Packets::TileFrameSection frame_section;
        frame_section.set_start_x(floor(starting_x / 200));
        frame_section.set_start_y(floor(starting_y / 150));
        frame_section.set_end_x(floor((starting_x + width) / 200));
        frame_section.set_end_y(floor((starting_y + height) / 150));
        send(frame_section);

        Terraria::Net::Packets::SpawnPlayerSelf spawn_self;
        send(spawn_self);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SpawnPlayer)
    {
        auto spawn_player = Terraria::Net::Packets::SpawnPlayer::from_bytes(packet_bytes_stream);
        outln("Wants to spawn player, probably themselves. Fuck that, let's just tell them to finish.");
        if (!m_has_finished_connecting)
        {
            m_has_finished_connecting = true;
            m_server.client_did_finish_connecting({}, *this);
            Terraria::Net::Packets::ConnectFinished connect_finished;
            send(connect_finished);
        }
        m_server.client_did_spawn_player({}, *this, *spawn_player);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncPlayer)
    {
        auto sync_player = Terraria::Net::Packets::SyncPlayer::from_bytes(packet_bytes_stream);
        m_player.set_control_bits(sync_player->control_bits());
        m_player.set_bits_2(sync_player->bits_2());
        m_player.set_bits_3(sync_player->bits_3());
        m_player.set_bits_4(sync_player->bits_4());
        m_player.inventory().set_selected_slot(
                static_cast<Terraria::PlayerInventory::Slot>(sync_player->selected_item()));
        m_player.set_position(sync_player->position());
        if (sync_player->velocity().has_value())
            m_player.set_velocity(*sync_player->velocity());
        // TODO: Do something with potion of return use and home position
        m_server.client_did_sync_player({}, *this, *sync_player);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncProjectile)
    {
        auto proj = Terraria::Net::Packets::SyncProjectile::from_bytes(packet_bytes_stream);
        m_server.client_did_sync_projectile({}, *this, *proj);
    }
    else if (packet_id == Terraria::Net::Packet::Id::NetModules)
    {
        Terraria::Net::Packet::ModuleId module;
        packet_bytes_stream >> module;
        if (module == Terraria::Net::Packet::ModuleId::Text)
        {
            auto text = Terraria::Net::Packets::Modules::Text::from_bytes(packet_bytes_stream);
            if (text->command_name() == "Say")
                m_server.client_did_send_message({}, *this, text->message());
        }
    }
    else if (packet_id == Terraria::Net::Packet::Id::KillProjectile)
    {
        auto kill_proj = Terraria::Net::Packets::KillProjectile::from_bytes(packet_bytes_stream);
        m_server.client_did_kill_projectile({}, *this, *kill_proj);
    }
    else if (packet_id == Terraria::Net::Packet::Id::TogglePvp)
    {
        auto toggle_pvp = Terraria::Net::Packets::TogglePvp::from_bytes(packet_bytes_stream);
        m_server.client_did_toggle_pvp({}, *this, *toggle_pvp);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerHurt)
    {
        auto player_hurt = Terraria::Net::Packets::PlayerHurt::from_bytes(packet_bytes_stream);
        m_server.client_did_hurt_player({}, *this, *player_hurt);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerDeath)
    {
        auto player_death = Terraria::Net::Packets::PlayerDeath::from_bytes(packet_bytes_stream);
        m_server.client_did_player_death({}, *this, *player_death);
    }
    else if (packet_id == Terraria::Net::Packet::Id::DamageNPC)
    {
        auto damage_npc = Terraria::Net::Packets::DamageNPC::from_bytes(packet_bytes_stream);
        m_server.client_did_damage_npc({}, *this, *damage_npc);
    }
    else if (packet_id == Terraria::Net::Packet::Id::ClientSyncedInventory)
    {
        // This packet has no data, and is completely useless.
    }
    else
    {
        warnln("Unhandled packet {}", packet_id);
    }
}