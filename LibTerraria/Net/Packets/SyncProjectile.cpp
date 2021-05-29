/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <AK/MemoryStream.h>

namespace Terraria::Net::Packets
{
Optional<SyncProjectile> SyncProjectile::from_bytes(InputStream& stream)
{
    SyncProjectile packet;

    i16 id;
    u8 owner;
    i16 type;

    stream >> id;
    stream >> packet.projectile().position();
    stream >> packet.projectile().velocity();
    stream >> owner;
    stream >> type;

    packet.projectile().set_id(id);
    packet.projectile().set_owner(owner);
    packet.projectile().set_type(type);

    u8 flags;
    stream >> flags;
    if ((flags & m_ai_1_bit) == m_ai_1_bit)
        stream >> packet.projectile().ai()[0];

    if ((flags & m_ai_2_bit) == m_ai_2_bit)
        stream >> packet.projectile().ai()[1];

    if ((flags & m_banner_id_to_respond_to_bit) == m_banner_id_to_respond_to_bit)
        stream >> packet.projectile().banner_id_to_respond_to();

    if ((flags & m_damage_bit) == m_damage_bit)
        stream >> packet.projectile().damage();

    if ((flags & m_knockback_bit) == m_knockback_bit)
        stream >> packet.projectile().knockback();

    if ((flags & m_original_damage_bit) == m_original_damage_bit)
        stream >> packet.projectile().original_damage();

    if ((flags & m_uuid_bit) == m_uuid_bit)
        stream >> packet.projectile().uuid();

    return packet;
}

ByteBuffer SyncProjectile::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncProjectile;
    DuplexMemoryStream stream;
    stream << packet_id;
    stream << m_projectile.id();
    stream << m_projectile.position();
    stream << m_projectile.velocity();
    stream << m_projectile.owner();
    stream << m_projectile.type();

    u8 flags = 0;

    if (m_projectile.ai()[0].has_value())
        flags |= m_ai_1_bit;

    if (m_projectile.ai()[1].has_value())
        flags |= m_ai_2_bit;

    if (m_projectile.banner_id_to_respond_to().has_value())
        flags |= m_banner_id_to_respond_to_bit;

    if (m_projectile.damage().has_value())
        flags |= m_damage_bit;

    if (m_projectile.knockback().has_value())
        flags |= m_knockback_bit;

    if (m_projectile.original_damage().has_value())
        flags |= m_original_damage_bit;

    if (m_projectile.uuid().has_value())
        flags |= m_uuid_bit;

    stream << flags;

    if (m_projectile.ai()[0].has_value())
        stream << *m_projectile.ai()[0];

    if (m_projectile.ai()[1].has_value())
        stream << *m_projectile.ai()[1];

    if (m_projectile.banner_id_to_respond_to().has_value())
        stream << *m_projectile.banner_id_to_respond_to();

    if (projectile().damage().has_value())
        stream << *projectile().damage();

    if (projectile().knockback().has_value())
        stream << *projectile().knockback();

    if (projectile().original_damage().has_value())
        stream << *projectile().original_damage();

    if (projectile().uuid().has_value())
        stream << *projectile().uuid();

    return stream.copy_into_contiguous_buffer();
}
}