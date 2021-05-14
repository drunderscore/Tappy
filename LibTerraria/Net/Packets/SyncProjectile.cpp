/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/SyncProjectile.h>

namespace Terraria::Net::Packets
{
Optional<SyncProjectile> SyncProjectile::from_bytes(InputStream& stream)
{
    SyncProjectile packet;

    stream >> packet.m_id;
    stream >> packet.m_position;
    stream >> packet.m_velocity;
    stream >> packet.m_owner;
    stream >> packet.m_type;

    u8 flags;
    stream >> flags;
    if ((flags & m_ai_1_bit) == m_ai_1_bit)
        stream >> packet.m_ai[0];

    if ((flags & m_ai_2_bit) == m_ai_2_bit)
        stream >> packet.m_ai[1];

    if ((flags & m_banner_id_to_respond_to_bit) == m_banner_id_to_respond_to_bit)
        stream >> packet.m_banner_id_to_respond_to;

    if ((flags & m_damage_bit) == m_damage_bit)
        stream >> packet.m_damage;

    if ((flags & m_knockback_bit) == m_knockback_bit)
        stream >> packet.m_knockback;

    if ((flags & m_original_damage_bit) == m_original_damage_bit)
        stream >> packet.m_original_damage;

    if ((flags & m_uuid_bit) == m_uuid_bit)
        stream >> packet.m_uuid;

    return packet;
}

ByteBuffer SyncProjectile::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncProjectile;
    auto buffer = ByteBuffer::create_uninitialized(256);
    OutputMemoryStream stream(buffer);
    stream << packet_id;
    stream << m_id;
    stream << m_position;
    stream << m_velocity;
    stream << m_owner;
    stream << m_type;

    u8 flags = 0;

    if (m_ai[0].has_value())
        flags |= m_ai_1_bit;

    if (m_ai[1].has_value())
        flags |= m_ai_2_bit;

    if (m_banner_id_to_respond_to.has_value())
        flags |= m_banner_id_to_respond_to_bit;

    if (m_damage.has_value())
        flags |= m_damage_bit;

    if (m_knockback.has_value())
        flags |= m_knockback_bit;

    if (m_original_damage.has_value())
        flags |= m_original_damage_bit;

    if (m_uuid.has_value())
        flags |= m_uuid_bit;

    stream << flags;

    if (m_ai[0].has_value())
        stream << *m_ai[0];

    if (m_ai[1].has_value())
        stream << *m_ai[1];

    if (m_banner_id_to_respond_to.has_value())
        stream << *m_banner_id_to_respond_to;

    if (m_damage.has_value())
        stream << *m_damage;

    if (m_knockback.has_value())
        stream << *m_knockback;

    if (m_original_damage.has_value())
        stream << *m_original_damage;

    if (m_uuid.has_value())
        stream << *m_uuid;

    return buffer;
}
}