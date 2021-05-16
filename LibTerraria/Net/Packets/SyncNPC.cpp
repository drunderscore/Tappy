/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/SyncNPC.h>

namespace Terraria::Net::Packets
{
Optional<SyncNPC> SyncNPC::from_bytes(InputStream& stream)
{
    SyncNPC packet;

    stream >> packet.m_id;
    stream >> packet.m_position;
    stream >> packet.m_velocity;
    stream >> packet.m_target;

    u8 bits_1;
    u8 bits_2;

    stream >> bits_1;
    stream >> bits_2;

    packet.m_direction = (bits_1 & m_direction_bit) == m_direction_bit;
    packet.m_direction_y = (bits_1 & m_direction_y_bit) == m_direction_y_bit;
    packet.m_sprite_direction = (bits_1 & m_sprite_direction_bit) == m_sprite_direction_bit;
    packet.m_spawned_from_statue = (bits_2 & m_spawned_from_statue_bit) == m_spawned_from_statue_bit;

    if ((bits_1 & m_ai_1_bit) == m_ai_1_bit)
        stream >> packet.m_ai[0];

    if ((bits_1 & m_ai_2_bit) == m_ai_2_bit)
        stream >> packet.m_ai[1];

    if ((bits_1 & m_ai_3_bit) == m_ai_3_bit)
        stream >> packet.m_ai[2];

    if ((bits_1 & m_ai_4_bit) == m_ai_4_bit)
        stream >> packet.m_ai[3];

    stream >> packet.m_type;

    if ((bits_2 & m_stats_scaled_bit) == m_stats_scaled_bit)
        stream >> packet.m_scaled_for_this_many_players;

    if ((bits_2 & m_strength_multiplier_bit) == m_strength_multiplier_bit)
        stream >> packet.m_strength_multiplier;

    u8 hp_length;

    stream >> hp_length;

    if (hp_length == 4)
        stream >> packet.m_hp;
    else if (hp_length == 2)
        stream >> *reinterpret_cast<i16*>(packet.m_hp);
    else if (hp_length == 1)
        stream >> *reinterpret_cast<i8*>(packet.m_hp);
    else
        VERIFY_NOT_REACHED();

    // TODO: If an NPC is catchable, we have to send who released it.

    return packet;
}

ByteBuffer SyncNPC::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncNPC;
    auto buffer = ByteBuffer::create_uninitialized(256);
    OutputMemoryStream stream(buffer);
    stream << packet_id;

    u8 bits_1 = 0;

    if (m_direction)
        bits_1 |= m_direction_bit;

    if (m_direction_y)
        bits_1 |= m_direction_y_bit;

    if (m_ai[0].has_value())
        bits_1 |= m_ai_1_bit;

    if (m_ai[1].has_value())
        bits_1 |= m_ai_2_bit;

    if (m_ai[2].has_value())
        bits_1 |= m_ai_3_bit;

    if (m_ai[3].has_value())
        bits_1 |= m_ai_4_bit;

    if (m_sprite_direction)
        bits_1 |= m_sprite_direction_bit;

    // TODO: Life max? I'm not sure what this is talking about.

    u8 bits_2 = 0;

    if (m_scaled_for_this_many_players.has_value())
        bits_2 |= m_stats_scaled_bit;

    if (m_spawned_from_statue)
        bits_2 |= m_spawned_from_statue_bit;

    if (m_strength_multiplier.has_value())
        bits_2 |= m_strength_multiplier_bit;

    stream << m_id;
    stream << m_position;
    stream << m_velocity;
    stream << m_target;

    stream << bits_1;
    stream << bits_2;

    for (auto i = 0; i < m_ai.size(); i++)
    {
        if (m_ai[i].has_value())
            stream << *m_ai[i];
    }

    stream << m_type;

    if (m_scaled_for_this_many_players.has_value())
        stream << *m_scaled_for_this_many_players;

    if (m_strength_multiplier.has_value())
        stream << *m_strength_multiplier;

    // For whatever reason, instead of even variable length encoding, they have us send the
    // byte length of the NPC HP as a byte, before sending the HP.
    // This is baffling.

    if (m_hp > 32767)
    {
        stream << static_cast<u8>(4);
        stream << m_hp;
    }
    else if (m_hp > 127)
    {
        stream << static_cast<u8>(2);
        stream << static_cast<i16>(m_hp);
    }
    else
    {
        stream << static_cast<u8>(1);
        stream << static_cast<i8>(m_hp);
    }

    // TODO: If an NPC is catchable, we have to send who released it.

    buffer.trim(stream.size());

    return buffer;
}
}