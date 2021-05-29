/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packets/SyncTileRect.h>

namespace Terraria::Net::Packets
{
ByteBuffer SyncTileRect::to_bytes() const
{
    static constexpr auto packet_id = Terraria::Net::Packet::Id::SyncTileRect;
    DuplexMemoryStream stream;
    stream << packet_id;
    stream << m_position;
    stream << m_width;
    stream << m_height;
    stream << m_change_type;

    // Unlike TileSection, this packet is written x by y.
    for (u16 x = m_position.x; x < m_position.x + m_width; x++)
    {
        for (u16 y = m_position.y; y < m_position.y + m_height; y++)
        {
            const auto& tile = m_tile_map.at({x, y});

            // Unlike TileSection, this packet only has two headers, but both are always present.
            u8 header1 = 0;
            u8 header2 = 0;

            if (tile.block().has_value())
            {
                header1 |= m_block_bit;

                if (tile.block()->shape() == 1)
                    header1 |= m_half_brick_bit;
                else if (tile.block()->shape() > 1)
                    header2 |= ((tile.block()->shape() - 1) << m_shape_shift) & m_shape_bits;
            }

            if (tile.wall_id().has_value())
                header1 |= m_wall_bit;

            if (tile.has_red_wire())
                header1 |= m_red_wire_bit;

            if (tile.has_actuator())
                header1 |= m_actuator_bit;

            if (tile.is_actuated())
                header1 |= m_actuated_bit;

            if (tile.has_blue_wire())
                header2 |= m_blue_wire_bit;

            if (tile.has_green_wire())
                header2 |= m_green_wire_bit;

            if (tile.has_green_wire())
                header2 |= m_green_wire_bit;

            stream << header1;
            stream << header2;

            if (tile.block().has_value())
            {
                stream << static_cast<u16>(tile.block()->id());

                stream << tile.block()->frame_x();
                stream << tile.block()->frame_y();
            }

            if (tile.wall_id().has_value())
                stream << static_cast<u16>(*tile.wall_id());
        }
    }

    return stream.copy_into_contiguous_buffer();
}
}