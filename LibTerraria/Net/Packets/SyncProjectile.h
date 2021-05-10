/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Optional.h>
#include <AK/Array.h>
#include <AK/String.h>
#include <AK/Random.h>
#include <AK/MemoryStream.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Point.h>

namespace Terraria::Net::Packets
{
class SyncProjectile : public Terraria::Net::Packet
{
public:
    using AIArray = Array<Optional<float>, 2>;

    SyncProjectile() = default;

    const char* packet_name() const override
    {
        return "SyncProjectile";
    }

    static Optional<SyncProjectile> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    i16 id() const
    { return m_id; }

    void set_id(i16 value)
    { m_id = value; }

    const EntityPoint& position() const
    { return m_position; }

    const EntityPoint& velocity() const
    { return m_velocity; }

    EntityPoint& position()
    { return m_position; }

    EntityPoint& velocity()
    { return m_velocity; }

    u8 owner() const
    { return m_owner; }

    void set_owner(u8 value)
    { m_owner = value; }

    i16 type() const
    { return m_type; }

    void set_type(i16 value)
    { m_type = value; }

    const AIArray& ai() const
    { return m_ai; }

    AIArray& ai()
    { return m_ai; }

    const Optional<u16>& banner_id_to_respond_to() const
    { return m_banner_id_to_respond_to; }

    const Optional<i16>& damage() const
    { return m_damage; }

    Optional<i16>& damage()
    { return m_damage; }

    const Optional<float>& knockback() const
    { return m_knockback; }

    const Optional<i16>& original_damage() const
    { return m_original_damage; }

    const Optional<i16>& uuid() const
    { return m_uuid; }

private:
    i16 m_id{};
    EntityPoint m_position;
    EntityPoint m_velocity;
    u8 m_owner{};
    i16 m_type{};
    AIArray m_ai{};
    Optional<u16> m_banner_id_to_respond_to{};
    Optional<i16> m_damage{};
    Optional<float> m_knockback{};
    Optional<i16> m_original_damage{};
    Optional<i16> m_uuid{};

    static constexpr u8 m_ai_1_bit = 0b0000'0001;
    static constexpr u8 m_ai_2_bit = 0b0000'0010;
    static constexpr u8 m_banner_id_to_respond_to_bit = 0b0000'1000;
    static constexpr u8 m_damage_bit = 0b0001'0000;
    static constexpr u8 m_knockback_bit = 0b0010'0000;
    static constexpr u8 m_uuid_bit = 0b1000'0000;
    static constexpr u8 m_original_damage_bit = 0b0100'0000;
};
}