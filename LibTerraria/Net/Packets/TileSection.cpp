/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/TileSection.h>
#include <LibCompress/Deflate.h>

namespace Terraria::Net::Packets
{
TileSection::TileSection(const TileMap& tile_map, i32 starting_x, i32 starting_y) :
        m_tile_map(tile_map),
        m_starting_x(starting_x),
        m_starting_y(starting_y)
{}

ByteBuffer TileSection::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::TileSection;
    DuplexMemoryStream stream;
    DuplexMemoryStream stream_deflated;
    stream << packet_id;

    stream << static_cast<u8>(1); // Compressed?
    stream_deflated << m_starting_x;
    stream_deflated << m_starting_y;
    stream_deflated << m_tile_map.width();
    stream_deflated << m_tile_map.height();

    for (const auto& tile : m_tile_map.tiles())
    {
        // There are 3 bitmask headers, of which the first is always present.
        // The first header says if the second header is present.
        // The second header says if the third header is present.
        // TODO: Implement both additional headers and their values.
        u8 header = 0;
        u8 header2 = 0;
        u8 header3 = 0;

        auto& tile_id = tile.id();
        auto& wall_id = tile.wall_id();
        bool additional_tile_byte = false;

        if (tile_id.has_value())
        {
            header |= m_tile_bit;
            if (static_cast<u16>(*tile_id) > 255)
            {
                header |= m_additional_tile_byte_bit;
                additional_tile_byte = true;
            }
        }

        if (wall_id.has_value())
        {
            header |= m_wall_bit;
            // FIXME: Support extended wall bytes (and more headers to accomplish the former)
            VERIFY(static_cast<u16>(*wall_id) <= 255);
        }

        if (tile.has_red_wire())
            header2 |= m_red_wire_bit;

        if (tile.has_blue_wire())
            header2 |= m_blue_wire_bit;

        if (tile.has_green_wire())
            header2 |= m_green_wire_bit;

        header2 |= (tile.shape() << m_shape_shift) & m_shape_bits;

        if (tile.has_yellow_wire())
            header3 |= m_yellow_wire_bit;

        if (tile.has_actuator())
            header3 |= m_actuator_bit;

        if (tile.is_actuated())
            header3 |= m_actuated_bit;

        if (header2 != 0)
            header |= m_header_2_bit;

        if (header3 != 0)
        {
            header |= m_header_2_bit;
            header2 |= m_header_3_bit;
        }

        stream_deflated << header;
        if (header2 != 0)
            stream_deflated << header2;
        if (header3 != 0)
            stream_deflated << header3;

        if (tile_id.has_value())
        {
            if (additional_tile_byte)
                stream_deflated << static_cast<u16>(*tile_id);
            else
                stream_deflated << static_cast<u8>(*tile_id);
        }

        if (wall_id.has_value())
            stream_deflated << static_cast<u8>(*wall_id);
    }

    // TODO: Support chests, signs, and tile entities
    stream_deflated << static_cast<u16>(0);  // Chest Count
    stream_deflated << static_cast<u16>(0);  // Sign Count
    stream_deflated << static_cast<u16>(0);  // Tile Entity Count

    auto buffer_inflate = Compress::DeflateCompressor::compress_all(stream_deflated.copy_into_contiguous_buffer());

    stream << buffer_inflate;

    return stream.copy_into_contiguous_buffer();
}

}