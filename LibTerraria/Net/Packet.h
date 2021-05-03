/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Format.h>
#include <AK/Stream.h>

namespace Terraria::Net
{
class Packet
{
public:
    enum class Id : u8
    {
        ConnectRequest = 1,
        Disconnect,
        SetUserSlot,
        PlayerInfo,
        SyncInventorySlot,
        RequestWorldData,
        PlayerHP = 16,
        PlayerMana = 42,
        PlayerBuffs = 50,
        ClientUUID = 68
    };

    const char* packet_name() const
    { VERIFY_NOT_REACHED(); }

    virtual ByteBuffer to_bytes() const
    { VERIFY_NOT_REACHED(); }
};
}

template<>
struct AK::Formatter<Terraria::Net::Packet::Id> : AK::Formatter<String>
{
    void format(FormatBuilder& builder, Terraria::Net::Packet::Id id)
    {
        builder.put_i64(static_cast<u8>(id));
    }
};

InputStream& operator>>(InputStream& stream, Terraria::Net::Packet::Id& value);

OutputStream& operator<<(OutputStream& stream, Terraria::Net::Packet::Id value);
