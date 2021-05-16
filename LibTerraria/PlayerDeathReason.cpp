/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/PlayerDeathReason.h>
#include <LibTerraria/Net/Types.h>
#include <AK/MemoryStream.h>

namespace Terraria
{
Optional<PlayerDeathReason> PlayerDeathReason::from_bytes(InputStream& stream)
{
    PlayerDeathReason reason;

    u8 flags;
    stream >> flags;

    if ((flags & m_player_bit) == m_player_bit)
        stream >> reason.m_player;

    if ((flags & m_npc_bit) == m_npc_bit)
        stream >> reason.m_npc;

    if ((flags & m_projectile_bit) == m_projectile_bit)
        stream >> reason.m_projectile;

    if ((flags & m_other_bit) == m_other_bit)
        stream >> reason.m_other;

    if ((flags & m_projectile_type_bit) == m_projectile_type_bit)
        stream >> reason.m_projectile_type;

    if ((flags & m_item_type_bit) == m_item_type_bit)
        stream >> reason.m_item_id;

    if ((flags & m_item_prefix_bit) == m_item_prefix_bit)
        stream >> reason.m_item_prefix;

    if ((flags & m_custom_reason_bit) == m_custom_reason_bit)
    {
        String temporary;
        Net::Types::read_string(stream, temporary);
        reason.m_custom_reason = move(temporary);
    }

    return reason;
}

ByteBuffer PlayerDeathReason::to_bytes() const
{
    auto buffer = ByteBuffer::create_uninitialized(256);
    OutputMemoryStream stream(buffer);

    u8 flags = 0;

    if (m_player.has_value())
        flags |= m_player_bit;

    if (m_npc.has_value())
        flags |= m_npc_bit;

    if (m_projectile.has_value())
        flags |= m_projectile_bit;

    if (m_other.has_value())
        flags |= m_other_bit;

    if (m_projectile_type.has_value())
        flags |= m_projectile_type_bit;

    if (m_item_id.has_value())
        flags |= m_item_type_bit;

    if (m_item_prefix.has_value())
        flags |= m_item_prefix_bit;

    if (m_custom_reason.has_value())
        flags |= m_custom_reason_bit;

    stream << flags;

    if ((flags & m_player_bit) == m_player_bit)
        stream << *m_player;

    if ((flags & m_npc_bit) == m_npc_bit)
        stream << *m_npc;

    if ((flags & m_projectile_bit) == m_projectile_bit)
        stream << *m_projectile;

    if ((flags & m_other_bit) == m_other_bit)
        stream << *m_other;

    if ((flags & m_projectile_type_bit) == m_projectile_type_bit)
        stream << *m_projectile_type;

    if ((flags & m_item_type_bit) == m_item_type_bit)
        stream << *m_item_id;

    if ((flags & m_item_prefix_bit) == m_item_prefix_bit)
        stream << *m_item_prefix;

    if ((flags & m_custom_reason_bit) == m_custom_reason_bit)
        Net::Types::write_string(stream, *m_custom_reason);

    buffer.trim(stream.size());

    return buffer;
}
}