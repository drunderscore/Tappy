#include <LibTerraria/Net/Packets/TileSection.h>

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
    // FIXME: This size is probably way too big always, or might not be enough. All packets have this problem, this needs a true fix.
    auto buffer = ByteBuffer::create_uninitialized(KiB * 2048);
    OutputMemoryStream stream(buffer);
    stream << packet_id;

    // TODO: Support compressed tile sections
    stream << static_cast<u8>(0); // Compressed?
    stream << m_starting_x;
    stream << m_starting_y;
    stream << m_tile_map.width();
    stream << m_tile_map.height();

    for (auto& tile : m_tile_map.tiles())
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

        stream << header;
        if (tile_id.has_value())
        {
            outln("LMAOOOO");
            if (additional_tile_byte)
                stream << static_cast<u16>(*tile_id);
            else
                stream << static_cast<u8>(*tile_id);
        }

        if (wall_id.has_value())
            stream << static_cast<u8>(*wall_id);
    }

    // TODO: Support chests, signs, and tile entities
    stream << static_cast<u16>(0);  // Chest Count
    stream << static_cast<u16>(0);  // Sign Count
    stream << static_cast<u16>(0);  // Tile Entity Count

    buffer.trim(stream.size());
    return buffer;
}

}