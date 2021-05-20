/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/MemoryStream.h>
#include <LibTerraria/Item.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/PlayerInventory.h>

namespace Terraria::Net::Packets
{
class SyncInventorySlot : public Terraria::Net::Packet
{
public:
    const char* packet_name() const override
    {
        return "SyncInventorySlot";
    }

    static Optional<SyncInventorySlot> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    const Item& item() const
    { return m_item; }

    Item& item()
    { return m_item; }

    Terraria::PlayerInventory::Slot slot() const
    { return m_slot; }

    void set_slot(Terraria::PlayerInventory::Slot value)
    { m_slot = value; }

    u8 player_id() const
    { return m_player_id; }

    void set_player_id(u8 value)
    { m_player_id = value; }

private:
    Item m_item;
    u8 m_player_id;
    Terraria::PlayerInventory::Slot m_slot;
};
}