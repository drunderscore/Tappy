/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Optional.h>
#include <AK/String.h>
#include <AK/Stream.h>
#include <LibTerraria/Net/Types.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Tile.h>
#include <LibTerraria/TileMap.h>
#include <AK/Random.h>

namespace Terraria::Net::Packets
{
class TileSection : public Terraria::Net::Packet
{
public:
    TileSection(const TileMap& tile_map, i32 starting_x, i32 starting_y, u16 width, u16 height);

    const char* packet_name() const override
    {
        return "TileSection";
    }

    static Optional<TileSection> from_bytes(InputStream& stream)
    {
        VERIFY_NOT_REACHED();
    }

    ByteBuffer to_bytes() const override;

private:
    const TileMap& m_tile_map;
    i32 m_starting_x;
    i32 m_starting_y;
    u16 m_width;
    u16 m_height;

    // Header 1
    static constexpr u8 m_header_2_bit = 0b0000'0001;
    static constexpr u8 m_block_bit = 0b0000'0010;
    static constexpr u8 m_wall_bit = 0b0000'0100;
    static constexpr u8 m_additional_tile_byte_bit = 0b0010'0000;
    // TODO: Liquid

    // Header 2
    static constexpr u8 m_header_3_bit = 0b0000'0001;
    static constexpr u8 m_red_wire_bit = 0b0000'0010;
    static constexpr u8 m_blue_wire_bit = 0b0000'0100;
    static constexpr u8 m_green_wire_bit = 0b0000'1000;
    static constexpr u8 m_shape_bits = 0b0111'0000;
    static constexpr u8 m_shape_shift = 4;
    // TODO: Slopes

    // Header 3
    static constexpr u8 m_actuator_bit = 0b0000'0010;
    static constexpr u8 m_actuated_bit = 0b0000'0100;
    static constexpr u8 m_yellow_wire_bit = 0b0010'0000;
    static constexpr u8 m_additional_wall_byte_bit = 0b0100'0000;
    // TODO: Block/Wall color
};
}
