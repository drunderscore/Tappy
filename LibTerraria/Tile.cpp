/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Tile.h>
#include <LibTerraria/Model.h>
#include <AK/Format.h>

namespace Terraria
{
// TODO: Randomness
constexpr Tile::PackedFrames frames_for_wires[16] =
        {
                {18 * 0, 18 * 3}, // nothing adjacent, single dot
                {18 * 2, 18 * 2}, // just the top, terminating on bottom vertical
                {18 * 1, 18 * 2}, // just the bottom, terminating on top vertical
                {18 * 0, 18 * 0}, // bottom and top, vertical line
                {18 * 3, 18 * 2}, // just the left, terminating on right horizontal
                {18 * 3, 18 * 1}, // left and top, bottom right angle
                {18 * 4, 18 * 1}, // left and bottom, top right junction
                {18 * 3, 18 * 0}, // top, left and bottom, left vertical junction
                {18 * 4, 18 * 2}, // just the right, terminating on left horizontal
                {18 * 2, 18 * 1}, // right and top, bottom left angle
                {18 * 0, 18 * 2}, // bottom and right, top left angle
                {18 * 2, 18 * 0}, // right, bottom and top, right vertical junction
                {18 * 1, 18 * 0}, // right and left, horizontal line
                {18 * 0, 18 * 1}, // right, left and top, top horizontal junction
                {18 * 4, 18 * 0}, // right, left and bottom, bottom horizontal junction
                {18 * 1, 18 * 1}, // ALL OF THEM, massive junction
        };

// TODO: Randomness
Optional<Tile::PackedFrames> Tile::Block::frame_for_block(const Tile& the_tile,
                                                          const Tile& top,
                                                          const Tile& bottom,
                                                          const Tile& left,
                                                          const Tile& right,
                                                          const Tile& bottom_left,
                                                          const Tile& bottom_right,
                                                          const Tile& top_left,
                                                          const Tile& top_right)
{
    auto the_id = the_tile.block()->id();
    if (the_tile.block()->id() == Id::Stone || s_tiles[static_cast<int>(the_id)].stone)
    {
        // This is stone or stone-like, so we frame it as such.
        if (top.block().has_value() && top.block()->id() == the_id)
        {
            if (bottom.block().has_value() && bottom.block()->id() == the_id)
            {
                if (left.block().has_value() && left.block()->id() == the_id)
                {
                    if (right.block().has_value() && right.block()->id() == the_id)
                    {
                        return {{18 * 1, 18 * 1}};
                    }
                    else
                    {
                        return {{18 * 4, 18 * 0}};
                    }
                }
                else
                {
                    return {{18 * 5, 18 * 0}};
                }
            }
            else
            {
                return {{18 * 6, 18 * 3}};
            }
        }
        else
        {
            return {{18 * 9, 18 * 3}};
        }
    }
    return {};
}

Tile::PackedFrames Tile::frames_for_wire(bool top, bool bottom, bool left, bool right)
{
    u8 bits{0};

    if (top)
        bits |= 0b0000'0001;//

    if (bottom)
        bits |= 0b0000'0010;//

    if (left)
        bits |= 0b0000'0100;

    if (right)
        bits |= 0b0000'1000;//

    return frames_for_wires[bits];
}

}