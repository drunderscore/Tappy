/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Optional.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Point.h>
#include <LibTerraria/TileMap.h>

namespace Terraria::Net::Packets
{
class SyncTileRect : public Terraria::Net::Packet
{
public:
    SyncTileRect(const TileMap& tile_map, TilePoint pos, u8 width, u8 height)
        : m_tile_map(tile_map), m_position(move(pos)), m_width(width), m_height(height)
    {
    }

    const char* packet_name() const override { return "SyncTileRect"; }

    // FIXME: Unlike TileSection, I believe the client does actually send this one...
    // So eventually we will need to handle it.
    static Optional<SyncTileRect> from_bytes(InputStream& stream) { VERIFY_NOT_REACHED(); }

    ByteBuffer to_bytes() const override;

    const TilePoint& position() const { return m_position; }

    TilePoint& position() { return m_position; }

private:
    const TileMap& m_tile_map;
    TilePoint m_position;
    u8 m_width;
    u8 m_height;
    u8 m_change_type{};

    // Header 1
    static constexpr u8 m_block_bit = 0b0000'0001;
    static constexpr u8 m_wall_bit = 0b0000'0100;
    static constexpr u8 m_liquid_bit = 0b0000'1000;
    static constexpr u8 m_red_wire_bit = 0b0001'0000;
    static constexpr u8 m_half_brick_bit = 0b0010'0000;
    static constexpr u8 m_actuator_bit = 0b0100'0000;
    static constexpr u8 m_actuated_bit = 0b1000'0000;

    // Header 2
    static constexpr u8 m_blue_wire_bit = 0b0000'0001;
    static constexpr u8 m_green_wire_bit = 0b0000'0010;
    static constexpr u8 m_shape_bits = 0b0111'0000;
    static constexpr u8 m_shape_shift = 4;
    static constexpr u8 m_yellow_wire_bit = 0b1000'0000;
    // TODO: Block/wall color
};
}