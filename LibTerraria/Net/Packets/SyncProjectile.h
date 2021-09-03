/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Array.h>
#include <AK/ByteBuffer.h>
#include <AK/Optional.h>
#include <AK/Random.h>
#include <AK/String.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Point.h>
#include <LibTerraria/Projectile.h>

namespace Terraria::Net::Packets
{
class SyncProjectile : public Terraria::Net::Packet
{
public:
    SyncProjectile() = default;

    const char* packet_name() const override { return "SyncProjectile"; }

    static Optional<SyncProjectile> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    const Projectile& projectile() const { return m_projectile; }

    Projectile& projectile() { return m_projectile; }

private:
    Projectile m_projectile;

    static constexpr u8 m_ai_1_bit = 0b0000'0001;
    static constexpr u8 m_ai_2_bit = 0b0000'0010;
    static constexpr u8 m_banner_id_to_respond_to_bit = 0b0000'1000;
    static constexpr u8 m_damage_bit = 0b0001'0000;
    static constexpr u8 m_knockback_bit = 0b0010'0000;
    static constexpr u8 m_uuid_bit = 0b1000'0000;
    static constexpr u8 m_original_damage_bit = 0b0100'0000;
};
}