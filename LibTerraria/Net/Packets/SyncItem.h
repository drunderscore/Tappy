/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/MemoryStream.h>
#include <LibTerraria/DroppedItem.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Point.h>

namespace Terraria::Net::Packets
{
class SyncItem : public Terraria::Net::Packet
{
public:
    const char* packet_name() const override
    {
        return "SyncItem";
    }

    static Optional<SyncItem> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    const DroppedItem& dropped_item() const
    { return m_dropped_item; }

    DroppedItem& dropped_item()
    { return m_dropped_item; }

    i16 id() const
    { return m_id; }

    void set_id(i16 value)
    { m_id = value; }

private:
    i16 m_id{};
    DroppedItem m_dropped_item;
};
}