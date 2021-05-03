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
    m_output_stream << static_cast<u16>(bytes.size());
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
        Terraria::Net::Packets::SetUserSlot set_user_slot;
        set_user_slot.set_player_id(m_id);
        send(set_user_slot);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerInfo)
    {
        auto player_info = Terraria::Net::Packets::PlayerInfo::from_bytes(packet_bytes_stream);
        m_player = make<Terraria::Player>(Terraria::Character::create_from_packet(*player_info));
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
        outln("Client wants world info, lets just list items tho");
        m_player->inventory().for_each([](auto slot, auto item)
        {
            outln("Slot {}: {} of {}", slot, item.stack(), item.id());
        });
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
    else if (packet_id == Terraria::Net::Packet::Id::PlayerMana)
    {
        auto player_mana = Terraria::Net::Packets::PlayerMana::from_bytes(packet_bytes_stream);
        m_player->set_mana(player_mana->mana());
        m_player->set_max_mana(player_mana->max_mana());
        outln("{} has {}/{} mana", m_id, player_mana->mana(), player_mana->max_mana());
    }
    else
    {
        warnln("Unhandled packet {}", packet_id);
    }
}