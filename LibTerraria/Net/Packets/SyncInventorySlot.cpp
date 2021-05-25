/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/SyncInventorySlot.h>

namespace Terraria::Net::Packets
{
Optional<SyncInventorySlot> SyncInventorySlot::from_bytes(InputStream& stream)
{
    SyncInventorySlot packet;

    Terraria::Item::Id id;
    i16 stack;
    Terraria::Item::Prefix prefix;

    stream >> packet.m_player_id;
    stream >> packet.m_slot;
    stream >> stack;
    stream >> prefix;
    stream >> id;

    Terraria::Item item;
    item.set_id(id);
    item.set_stack(stack);
    item.set_prefix(prefix);

    packet.item() = item;

    return packet;
}

ByteBuffer SyncInventorySlot::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncInventorySlot;
    DuplexMemoryStream stream;
    stream << packet_id;

    stream << m_player_id;
    stream << m_slot;
    stream << m_item.stack();
    stream << m_item.prefix();
    stream << m_item.id();

    return stream.copy_into_contiguous_buffer();
}
}