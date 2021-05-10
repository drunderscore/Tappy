/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Client.h>
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

Client::Client(NonnullRefPtr<Core::TCPSocket> socket, u8 id) :
        m_socket(move(socket)),
        m_id(id),
        m_input_stream(m_socket),
        m_output_stream(m_socket)
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
        dbgln("Stream errored trying to read packet size");
        m_input_stream.handle_any_error();
        return;
    }

    Terraria::Net::Packet::Id packet_id;
    m_input_stream >> packet_id;
    if (m_input_stream.has_any_error())
    {
        dbgln("Stream errored trying to read packet id");
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
        outln("Client {} is connecting with version _{}_", m_id, request->version());

        Terraria::Net::Packets::SetUserSlot set_user_slot;
        set_user_slot.set_player_id(m_id);
        send(set_user_slot);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerInfo)
    {
        auto player_info = Terraria::Net::Packets::PlayerInfo::from_bytes(packet_bytes_stream);
        m_player = make<Terraria::Player>(Terraria::Character::create_from_packet(*player_info));
        m_player->inventory().on_selected_slot_change = [](auto from, auto to)
        {
            outln("Slot changed from {} to {}", from, to);
        };
        outln("Got character, created player for {}", m_player->character().name());
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncInventorySlot)
    {
        auto inv_slot = Terraria::Net::Packets::SyncInventorySlot::from_bytes(packet_bytes_stream);
        if (inv_slot->id() != Terraria::Item::Id::None)
        {
            m_player->inventory().insert(inv_slot->slot(),
                                         Terraria::Item(inv_slot->id(), inv_slot->prefix(), inv_slot->stack()));
        }
    }
    else if (packet_id == Terraria::Net::Packet::Id::RequestWorldData)
    {
        outln("Client wants world info, let's try our best");

        Terraria::Net::Packets::WorldData world_data;
        world_data.set_max_tiles_x(4200);
        world_data.set_max_tiles_y(1200);
        world_data.set_world_surface(400);
        world_data.set_world_name("lol im cool");
        world_data.set_spawn_x(world_data.world_surface() - 5);
        world_data.set_spawn_y(world_data.max_tiles_y() / 2);
        world_data.set_time(8000);
        world_data.set_day_state(1);
        world_data.set_world_flags_1(world_data.world_flags_1() | 0b0100'0000);
        outln("world data byte size is {}?", world_data.to_bytes().size());
        send(world_data);
    }
    else if (packet_id == Terraria::Net::Packet::Id::ClientUUID)
    {
        auto client_uuid = Terraria::Net::Packets::ClientUUID::from_bytes(packet_bytes_stream);
        if (client_uuid->uuid().length() != 36)
        {
            warnln("Client sent UUID that isn't 36 characters.");
        }
        else
        {
            m_uuid = UUID(client_uuid->uuid().view());
            if (m_uuid.has_value())
            {
                outln("Client {} has UUID {}", m_id, m_uuid->to_string());
            }
            else
            {
                warnln("Client sent invalid UUID that AK couldn't parse.");
            }
        }
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerHP)
    {
        auto player_hp = Terraria::Net::Packets::PlayerHP::from_bytes(packet_bytes_stream);
        m_player->set_hp(player_hp->hp());
        m_player->set_max_hp(player_hp->max_hp());
        outln("{} has {}/{} hp", m_id, player_hp->hp(), player_hp->max_hp());
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerBuffs)
    {
        auto player_buffs = Terraria::Net::Packets::PlayerBuffs::from_bytes(packet_bytes_stream);
        player_buffs->buffs().span().copy_to(m_player->buffs().span());
        outln("Got player buffs, here they are:");
        for (auto buff_id : m_player->buffs())
        {
            if (buff_id != 0)
                outln("Buff {}", buff_id);
        }
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerMana)
    {
        auto player_mana = Terraria::Net::Packets::PlayerMana::from_bytes(packet_bytes_stream);
        m_player->set_mana(player_mana->mana());
        m_player->set_max_mana(player_mana->max_mana());
        outln("{} has {}/{} mana", m_id, player_mana->mana(), player_mana->max_mana());
    }
    else if (packet_id == Terraria::Net::Packet::Id::SpawnData)
    {
        auto spawn_data = Terraria::Net::Packets::SpawnData::from_bytes(packet_bytes_stream);
        outln("Client {} wants to spawn at {}, {}", m_id, spawn_data->spawn_x(), spawn_data->spawn_y());
        outln("okay we'll send some weird tile section, and tell them to spawn themselves");
        static constexpr u16 width = 100;
        static constexpr u16 height = 3;
        static constexpr i32 starting_x = 41;
        static constexpr i32 starting_y = 200;
        Terraria::TileMap tiles(width, height);

        Terraria::Tile stone;
        stone.id() = Terraria::Tile::Id::Stone;

        for (auto& t : tiles.tiles())
            t = stone;

        Terraria::Net::Packets::TileSection section(tiles, starting_x, starting_y);
        send(section);

        Terraria::Net::Packets::TileFrameSection frame_section;
        frame_section.set_start_x(starting_x);
        frame_section.set_start_y(starting_y);
        frame_section.set_end_x(starting_x + width);
        frame_section.set_end_y(starting_x + height);
        send(frame_section);
        Terraria::Net::Packets::SpawnPlayerSelf spawn_self;
        send(spawn_self);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SpawnPlayer)
    {
        outln("Wants to spawn player, probably themselves. Fuck that, let's just tell them to finish.");
        Terraria::Net::Packets::ConnectFinished connect_finished;
        send(connect_finished);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncPlayer)
    {
        auto sync_player = Terraria::Net::Packets::SyncPlayer::from_bytes(packet_bytes_stream);
        m_player->set_control_bits(sync_player->control_bits());
        m_player->set_bits_2(sync_player->bits_2());
        m_player->set_bits_3(sync_player->bits_3());
        m_player->set_bits_4(sync_player->bits_4());
        m_player->inventory().set_selected_slot(
                static_cast<Terraria::PlayerInventory::Slot>(sync_player->selected_item()));
        m_player->set_position(sync_player->position());
        if (sync_player->velocity().has_value())
            m_player->set_velocity(*sync_player->velocity());
        // TODO: Do something with potion of return use and home position
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncProjectile)
    {
        auto proj = Terraria::Net::Packets::SyncProjectile::from_bytes(packet_bytes_stream);
        outln("Syncing projectile {} (type {}) at {}, velocity {}", proj->id(), proj->type(), proj->position(),
              proj->velocity());
        Terraria::Net::Packets::Modules::Text text;
        text.set_author(255);
        text.set_color(Terraria::Color{255, 0, 255});
        text.set_text("lmao im a message");
        send(text);
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