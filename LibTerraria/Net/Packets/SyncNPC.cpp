/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/MemoryStream.h>
#include <LibTerraria/Net/Packets/SyncNPC.h>

namespace Terraria::Net::Packets
{
Optional<SyncNPC> SyncNPC::from_bytes(InputStream& stream)
{
    SyncNPC packet;

    i16 id;
    u16 target;

    stream >> id;
    stream >> packet.npc().position();
    stream >> packet.npc().velocity();
    stream >> target;

    packet.npc().set_id(id);
    packet.npc().set_target(target);

    u8 bits_1;
    u8 bits_2;

    stream >> bits_1;
    stream >> bits_2;

    packet.npc().set_direction((bits_1 & m_direction_bit) == m_direction_bit);
    packet.npc().set_direction_y((bits_1 & m_direction_y_bit) == m_direction_y_bit);
    packet.npc().set_sprite_direction((bits_1 & m_sprite_direction_bit) == m_sprite_direction_bit);
    packet.npc().set_spawned_from_statue((bits_2 & m_spawned_from_statue_bit) == m_spawned_from_statue_bit);

    if ((bits_1 & m_ai_1_bit) == m_ai_1_bit)
        stream >> packet.npc().ai()[0];

    if ((bits_1 & m_ai_2_bit) == m_ai_2_bit)
        stream >> packet.npc().ai()[1];

    if ((bits_1 & m_ai_3_bit) == m_ai_3_bit)
        stream >> packet.npc().ai()[2];

    if ((bits_1 & m_ai_4_bit) == m_ai_4_bit)
        stream >> packet.npc().ai()[3];

    i16 type;
    stream >> type;

    packet.npc().set_type(type);

    if ((bits_2 & m_stats_scaled_bit) == m_stats_scaled_bit)
        stream >> packet.npc().scaled_for_this_many_players();

    if ((bits_2 & m_strength_multiplier_bit) == m_strength_multiplier_bit)
    {
        float strength_multiplier;
        stream >> strength_multiplier;
        packet.npc().strength_multiplier() = strength_multiplier;
    }

    u8 hp_length;

    stream >> hp_length;

    if (hp_length == 4)
    {
        i32 hp;
        stream >> hp;
        packet.npc().set_hp(hp);
    }
    else if (hp_length == 2)
    {
        i16 hp;
        stream >> hp;
        packet.npc().set_hp(hp);
    }
    else if (hp_length == 1)
    {
        i8 hp;
        stream >> hp;
        packet.npc().set_hp(hp);
    }
    else
        VERIFY_NOT_REACHED();

    // TODO: If an NPC is catchable, we have to send who released it.

    return packet;
}

ByteBuffer SyncNPC::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncNPC;
    DuplexMemoryStream stream;
    stream << packet_id;

    u8 bits_1 = 0;

    if (npc().direction())
        bits_1 |= m_direction_bit;

    if (npc().direction_y())
        bits_1 |= m_direction_y_bit;

    if (npc().ai()[0].has_value())
        bits_1 |= m_ai_1_bit;

    if (npc().ai()[1].has_value())
        bits_1 |= m_ai_2_bit;

    if (npc().ai()[2].has_value())
        bits_1 |= m_ai_3_bit;

    if (npc().ai()[3].has_value())
        bits_1 |= m_ai_4_bit;

    if (npc().sprite_direction())
        bits_1 |= m_sprite_direction_bit;

    // TODO: Life max? I'm not sure what this is talking about.

    u8 bits_2 = 0;

    if (npc().scaled_for_this_many_players().has_value())
        bits_2 |= m_stats_scaled_bit;

    if (npc().spawned_from_statue())
        bits_2 |= m_spawned_from_statue_bit;

    if (npc().strength_multiplier().has_value())
        bits_2 |= m_strength_multiplier_bit;

    stream << npc().id();
    stream << npc().position();
    stream << npc().velocity();
    stream << npc().target();

    stream << bits_1;
    stream << bits_2;

    for (auto i = 0; i < npc().ai().size(); i++)
    {
        if (npc().ai()[i].has_value())
            stream << *npc().ai()[i];
    }

    stream << npc().type();

    if (npc().scaled_for_this_many_players().has_value())
        stream << *npc().scaled_for_this_many_players();

    if (npc().strength_multiplier().has_value())
        stream << *npc().strength_multiplier();

    // For whatever reason, instead of even variable length encoding, they have us send the
    // byte length of the NPC HP as a byte, before sending the HP.
    // This is baffling.

    if (npc().hp() > 32767)
    {
        stream << static_cast<u8>(4);
        stream << npc().hp();
    }
    else if (npc().hp() > 127)
    {
        stream << static_cast<u8>(2);
        stream << static_cast<i16>(npc().hp());
    }
    else
    {
        stream << static_cast<u8>(1);
        stream << static_cast<i8>(npc().hp());
    }

    // TODO: If an NPC is catchable, we have to send who released it.

    return stream.copy_into_contiguous_buffer();
}
}