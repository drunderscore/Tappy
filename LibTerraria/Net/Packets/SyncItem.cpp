/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/SyncItem.h>

namespace Terraria::Net::Packets
{
Optional<SyncItem> SyncItem::from_bytes(InputStream& stream)
{
    SyncItem packet;

    stream >> packet.m_id;

    stream >> packet.dropped_item().position();
    stream >> packet.dropped_item().velocity();

    i16 temporary_16;
    stream >> temporary_16;
    packet.dropped_item().item().set_stack(temporary_16);

    u8 temporary_8;

    stream >> temporary_8;
    packet.dropped_item().item().set_prefix(static_cast<Terraria::Item::Prefix>(temporary_8));

    stream >> temporary_8;
    packet.set_has_pickup_delay(temporary_8 == 0);

    stream >> temporary_16;
    packet.dropped_item().item().set_id(static_cast<Terraria::Item::Id>(temporary_16));

    // TODO: Should we read the owner here into the dropped item?

    return packet;
}

ByteBuffer SyncItem::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncItem;
    DuplexMemoryStream stream;
    stream << packet_id;

    stream << m_id;
    stream << m_dropped_item.position();
    stream << m_dropped_item.velocity();
    stream << m_dropped_item.item().stack();
    stream << m_dropped_item.item().prefix();
    stream << static_cast<u8>(!m_has_pickup_delay);
    stream << m_dropped_item.item().id();

    return stream.copy_into_contiguous_buffer();
}
}