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

    u8 packet_id;
    m_input_stream >> packet_id;
    if (m_input_stream.has_any_error())
    {
        dbgln("Stream errored trying to read packet id");
        m_input_stream.handle_any_error();
        return;
    }

    // The packet size (2 bytes) and id (1 byte) are part of this.
    auto bytes = m_socket->read(packet_size - 3);

    // Connection request, let's send a user slot
    if (packet_id == 1)
    {
        Terraria::Net::Packets::SetUserSlot set_user_slot;
        set_user_slot.set_player_id(m_id);
        send(set_user_slot);
    }
    else if (packet_id == 4)
    {
        InputMemoryStream packet_bytes_stream(bytes);
        auto player_info = Terraria::Net::Packets::PlayerInfo::from_bytes(packet_bytes_stream);
        m_player = Terraria::Player(Terraria::Character::create_from_packet(*player_info));
        outln("Got character, created player for {}", m_player->character().name());
    }
    else if (packet_id == 5)
    {
        InputMemoryStream packet_bytes_stream(bytes);
        auto inv_slot = Terraria::Net::Packets::SyncInventorySlot::from_bytes(packet_bytes_stream);
        if (inv_slot->id() != Terraria::Item::Id::None)
        {
            m_player->inventory().insert(inv_slot->slot(),
                                         Terraria::Item(inv_slot->id(), inv_slot->prefix(), inv_slot->stack()));
        }
    }
    else if (packet_id == 6)
    {
        outln("Client wants world info, lets just list items tho");
        m_player->inventory().for_each([](auto slot, auto item)
        {
            outln("Slot {}: {} of {}", slot, item.stack(), item.id());
        });
    }
}