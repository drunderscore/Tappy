/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <LibTerraria/NPC.h>
#include <LibTerraria/Net/Packet.h>

namespace Terraria::Net::Packets
{
class SyncNPC : public Terraria::Net::Packet
{
public:
    using AIArray = Array<Optional<float>, 4>;

    SyncNPC() = default;

    const char* packet_name() const override { return "SyncNPC"; }

    static Optional<SyncNPC> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    const NPC& npc() const { return m_npc; }

    NPC& npc() { return m_npc; }

private:
    NPC m_npc;

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