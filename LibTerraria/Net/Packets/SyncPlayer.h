/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Optional.h>
#include <AK/String.h>
#include <AK/Random.h>
#include <AK/MemoryStream.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Point.h>

namespace Terraria::Net::Packets
{
class SyncPlayer : public Terraria::Net::Packet
{
public:
    SyncPlayer() = default;

    const char* packet_name() const override
    {
        return "SyncPlayer";
    }

    static Optional<SyncPlayer> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    u8 player_id() const
    { return m_player_id; }

    u8 control_bits() const
    { return m_control_bits; }

    u8 bits_2() const
    { return m_bits_2; }

    u8 bits_3() const
    { return m_bits_3; }

    u8 bits_4() const
    { return m_bits_4; }

    u8 selected_item() const
    { return m_selected_item; }

    const EntityPoint& position() const
    { return m_position; }

    const Optional<EntityPoint>& velocity() const
    { return m_velocity; }

    const Optional<EntityPoint>& potion_of_return_use_position() const
    { return m_potion_of_return_use_position; }

    const Optional<EntityPoint>& potion_of_return_home_position() const
    { return m_potion_of_return_home_position; }

private:
    u8 m_player_id{};
    u8 m_control_bits{};
    u8 m_bits_2{};
    u8 m_bits_3{};
    u8 m_bits_4{};
    u8 m_selected_item;
    EntityPoint m_position;
    Optional<EntityPoint> m_velocity;
    Optional<EntityPoint> m_potion_of_return_use_position;
    Optional<EntityPoint> m_potion_of_return_home_position;

    static constexpr u8 m_velocity_bit = 0b0000'0100;
    static constexpr u8 m_potion_of_return_bit = 0b0100'0000;
};
}
