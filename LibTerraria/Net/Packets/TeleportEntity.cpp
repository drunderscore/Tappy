/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/MemoryStream.h>
#include <LibTerraria/Net/Packets/TeleportEntity.h>

namespace Terraria::Net::Packets
{
Optional<TeleportEntity> TeleportEntity::from_bytes(InputStream& stream)
{
    TeleportEntity packet;

    u8 flags = 0;
    stream >> flags;

    if ((flags & m_teleport_npc_bit) == m_teleport_npc_bit)
        packet.m_type = TeleportType::NPCToPosition;
    else if ((flags & m_teleport_player_to_player_bit) == m_teleport_player_to_player_bit)
        packet.m_type = TeleportType::PlayerToPlayer;
    else
        packet.m_type = TeleportType::PlayerToPosition;

    if ((flags & m_get_position_from_target_bit) == m_get_position_from_target_bit)
        packet.m_get_position_from_target = true;

    stream >> packet.m_target;
    stream >> packet.m_position;
    stream >> packet.m_style;

    if ((flags & m_extra_info_bit) == m_extra_info_bit)
        stream >> packet.m_extra_info;

    return packet;
}

ByteBuffer TeleportEntity::to_bytes() const
{
    u8 flags = 0;

    if (m_type == TeleportType::NPCToPosition)
        flags |= m_teleport_npc_bit;
    else if (m_type == TeleportType::PlayerToPosition)
        flags |= m_teleport_player_to_player_bit;

    if (m_get_position_from_target)
        flags |= m_get_position_from_target_bit;

    if (m_extra_info.has_value())
        flags |= m_extra_info_bit;

    static constexpr auto packet_id = Terraria::Net::Packet::Id::TeleportEntity;
    DuplexMemoryStream stream;
    stream << packet_id;
    stream << flags;
    stream << m_target;
    stream << m_position;
    stream << m_style;

    if (m_extra_info.has_value())
        stream << *m_extra_info;

    return stream.copy_into_contiguous_buffer();
}
}