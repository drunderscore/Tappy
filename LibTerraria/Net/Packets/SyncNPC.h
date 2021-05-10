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
class SyncNPC : public Terraria::Net::Packet
{
public:
    using AIArray = Array<Optional<float>, 4>;

    SyncNPC() = default;

    const char* packet_name() const override
    {
        return "SyncNPC";
    }

    static Optional<SyncNPC> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    i16 id() const
    { return m_id; }

    const EntityPoint& position() const
    { return m_position; }

    const EntityPoint& velocity() const
    { return m_velocity; }

    EntityPoint& position()
    { return m_position; }

    EntityPoint& velocity()
    { return m_velocity; }

    u16 target() const
    { return m_target; }

    i32 hp() const
    { return m_hp; }

    i16 type() const
    { return m_type; }

    void set_id(i16 value)
    { m_id = value; }

    void set_hp(i32 value)
    { m_hp = value; }

    void set_type(i16 value)
    { m_type = value; }

private:
    i16 m_id{};
    EntityPoint m_position;
    EntityPoint m_velocity;
    u16 m_target{};
    i32 m_hp{};
    i16 m_type{};
    bool m_direction{}; // FIXME: Magic value
    bool m_direction_y{}; // FIXME: Magic value
    AIArray m_ai{};
    bool m_sprite_direction{}; // FIXME: Magic value
    Optional<u8> m_scaled_for_this_many_players{}; // FIXME: This name sucks
    bool m_spawned_from_statue{};
    Optional<float> m_strength_multiplier{};
    Optional<u8> m_release_owner{};

    static constexpr u8 m_direction_bit = 0b0000'0001;
    static constexpr u8 m_direction_y_bit = 0b0000'0010;
    static constexpr u8 m_ai_1_bit = 0b0000'0100;
    static constexpr u8 m_ai_2_bit = 0b0000'1000;
    static constexpr u8 m_ai_3_bit = 0b0001'0000;
    static constexpr u8 m_ai_4_bit = 0b0010'0000;
    static constexpr u8 m_sprite_direction_bit = 0b0100'0000;
    static constexpr u8 m_max_life_bit = 0b1000'0000;

    static constexpr u8 m_stats_scaled_bit = 0b0000'0001;
    static constexpr u8 m_spawned_from_statue_bit = 0b0000'0010;
    static constexpr u8 m_strength_multiplier_bit = 0b0000'0100;
};
}