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
        WorldData,
        SpawnData,
        StatusText,
        TileSection,
        TileFrameSection,
        SpawnPlayer,
        SyncPlayer,
        PlayerActive = 14,
        PlayerHP = 16,
        ModifyTile = 17,
        SyncTileRect = 20,
        SyncNPC = 23,
        SyncProjectile = 27,
        DamageNPC = 28,
        KillProjectile = 29,
        TogglePvp = 30,
        PlayerItemAnimation = 41,
        PlayerMana = 42,
        SpawnPlayerSelf = 49,
        PlayerBuffs = 50,
        TeleportEntity = 65,
        ClientUUID = 68,
        ReleaseNPC = 71,
        NetModules = 82,
        PlayerHurt = 117,
        PlayerDeath = 118,
        SyncTilePicking = 125,
        ConnectFinished = 129,
        ClientSyncedInventory = 138
    };

    enum class ModuleId : u16
    {
        Text = 1
    };

    virtual const char* packet_name() const
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

template<>
struct AK::Formatter<Terraria::Net::Packet::ModuleId> : AK::Formatter<String>
{
    void format(FormatBuilder& builder, Terraria::Net::Packet::ModuleId id)
    {
        builder.put_i64(static_cast<u16>(id));
    }
};

InputStream& operator>>(InputStream& stream, Terraria::Net::Packet::Id& value);

OutputStream& operator<<(OutputStream& stream, Terraria::Net::Packet::Id value);

InputStream& operator>>(InputStream& stream, Terraria::Net::Packet::ModuleId& value);

OutputStream& operator<<(OutputStream& stream, Terraria::Net::Packet::ModuleId value);
