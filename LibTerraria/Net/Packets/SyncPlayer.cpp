/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/SyncPlayer.h>
#include <LibTerraria/Net/Types.h>

namespace Terraria::Net::Packets
{

Optional<SyncPlayer> SyncPlayer::from_bytes(InputStream& stream)
{
    SyncPlayer packet;
    stream >> packet.m_player_id;
    stream >> packet.m_control_bits;
    stream >> packet.m_bits_2;
    stream >> packet.m_bits_3;
    stream >> packet.m_bits_4;
    stream >> packet.m_selected_item;
    stream >> packet.m_position;
    if ((packet.m_bits_2 & m_velocity_bit) == m_velocity_bit)
        stream >> packet.m_velocity;
    if ((packet.m_bits_3 & m_potion_of_return_bit) == m_potion_of_return_bit)
    {
        stream >> packet.m_potion_of_return_use_position;
        stream >> packet.m_potion_of_return_home_position;
    }

    return packet;
}


ByteBuffer SyncPlayer::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncPlayer;
    auto buffer = ByteBuffer::create_uninitialized(256);
    OutputMemoryStream stream(buffer);
    stream << packet_id;
    stream << m_player_id;
    stream << m_control_bits;
    stream << m_bits_2;
    stream << m_bits_3;
    stream << m_bits_4;
    stream << m_selected_item;
    stream << m_position;
    stream << m_velocity;
    stream << m_potion_of_return_use_position;
    stream << m_potion_of_return_home_position;

    buffer.trim(stream.size());

    return buffer;
}
}