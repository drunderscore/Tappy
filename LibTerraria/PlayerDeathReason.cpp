/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/PlayerDeathReason.h>
#include <LibTerraria/Net/Types.h>
#include <AK/MemoryStream.h>

static constexpr u8 m_player_bit = 0b0000'0001;
static constexpr u8 m_npc_bit = 0b0000'0010;
static constexpr u8 m_projectile_bit = 0b0000'0100;
static constexpr u8 m_other_bit = 0b0000'1000;
static constexpr u8 m_projectile_type_bit = 0b0001'0000;
static constexpr u8 m_item_type_bit = 0b0010'0000;
static constexpr u8 m_item_prefix_bit = 0b0100'0000;
static constexpr u8 m_custom_reason_bit = 0b1000'0000;

InputStream& operator>>(InputStream& stream, Terraria::PlayerDeathReason& value)
{
    u8 flags;
    stream >> flags;

    if ((flags & m_player_bit) == m_player_bit)
        stream >> value.player();

    if ((flags & m_npc_bit) == m_npc_bit)
        stream >> value.npc();

    if ((flags & m_projectile_bit) == m_projectile_bit)
        stream >> value.projectile();

    if ((flags & m_other_bit) == m_other_bit)
        stream >> value.other();

    if ((flags & m_projectile_type_bit) == m_projectile_type_bit)
        stream >> value.projectile_type();

    if ((flags & m_item_type_bit) == m_item_type_bit)
        stream >> value.item_id();

    if ((flags & m_item_prefix_bit) == m_item_prefix_bit)
        stream >> value.item_prefix();

    if ((flags & m_custom_reason_bit) == m_custom_reason_bit)
    {
        String temporary;
        Terraria::Net::Types::read_string(stream, temporary);
        value.custom_reason() = move(temporary);
    }

    return stream;
}

OutputStream& operator<<(OutputStream& stream, const Terraria::PlayerDeathReason& value)
{
    u8 flags = 0;

    if (value.player().has_value())
        flags |= m_player_bit;

    if (value.npc().has_value())
        flags |= m_npc_bit;

    if (value.projectile().has_value())
        flags |= m_projectile_bit;

    if (value.other().has_value())
        flags |= m_other_bit;

    if (value.projectile_type().has_value())
        flags |= m_projectile_type_bit;

    if (value.item_id().has_value())
        flags |= m_item_type_bit;

    if (value.item_prefix().has_value())
        flags |= m_item_prefix_bit;

    if (value.custom_reason().has_value())
        flags |= m_custom_reason_bit;

    stream << flags;

    if ((flags & m_player_bit) == m_player_bit)
        stream << *value.player();

    if ((flags & m_npc_bit) == m_npc_bit)
        stream << *value.npc();

    if ((flags & m_projectile_bit) == m_projectile_bit)
        stream << *value.projectile();

    if ((flags & m_other_bit) == m_other_bit)
        stream << *value.other();

    if ((flags & m_projectile_type_bit) == m_projectile_type_bit)
        stream << *value.projectile_type();

    if ((flags & m_item_type_bit) == m_item_type_bit)
        stream << *value.item_id();

    if ((flags & m_item_prefix_bit) == m_item_prefix_bit)
        stream << *value.item_prefix();

    if ((flags & m_custom_reason_bit) == m_custom_reason_bit)
        Terraria::Net::Types::write_string(stream, *value.custom_reason());

    return stream;
}
