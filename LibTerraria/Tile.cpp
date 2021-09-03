/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Model.h>
#include <LibTerraria/Tile.h>

namespace Terraria
{
// TODO: Randomness
constexpr Tile::PackedFrames frames_for_wires[16] = {
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
    {18 * 1, 18 * 1}  // ALL OF THEM, massive junction
};

// TODO: Randomness
constexpr Tile::PackedFrames frames_for_general_blocks[16] = {
    {18 * 9, 18 * 3},  // nothing adjacent, single block
    {18 * 6, 18 * 3},  // just the top, terminating on bottom vertical
    {18 * 6, 18 * 0},  // just the bottom, terminating on top vertical
    {18 * 5, 18 * 0},  // bottom and top, vertical line
    {18 * 12, 18 * 0}, // just the left, terminating on right horizontal
    {18 * 1, 18 * 4},  // left and top, bottom right angle
    {18 * 1, 18 * 3},  // left and bottom, top right junction
    {18 * 4, 18 * 0},  // top, left and bottom, left vertical junction
    {18 * 9, 18 * 0},  // just the right, terminating on left horizontal
    {18 * 0, 18 * 4},  // right and top, bottom left angle
    {18 * 0, 18 * 3},  // bottom and right, top left angle
    {18 * 0, 18 * 1},  // right, bottom and top, right vertical junction
    {18 * 6, 18 * 4},  // right and left, horizontal line
    {18 * 1, 18 * 2},  // right, left and top, top horizontal junction
    {18 * 1, 18 * 0},  // right, left and bottom, bottom horizontal junction
    {18 * 1, 18 * 1}   // ALL OF THEM, massive junction
};

// TODO: Randomness
Optional<Tile::PackedFrames> Tile::Block::frame_for_block(const Tile& the_tile, const Tile& top, const Tile& bottom,
                                                          const Tile& left, const Tile& right)
{
    // We cannot frame this tile if it's frame is important
    if (s_tiles[static_cast<int>(the_tile.block()->id())].frame_important)
        return {};

    u8 bits{0};

    if (top.block().has_value() && top.block()->id() == the_tile.block()->id())
        bits |= 0b0000'0001;

    if (bottom.block().has_value() && bottom.block()->id() == the_tile.block()->id())
        bits |= 0b0000'0010;

    if (left.block().has_value() && left.block()->id() == the_tile.block()->id())
        bits |= 0b0000'0100;

    if (right.block().has_value() && right.block()->id() == the_tile.block()->id())
        bits |= 0b0000'1000;

    return frames_for_general_blocks[bits];
}

Tile::PackedFrames Tile::frames_for_wire(bool top, bool bottom, bool left, bool right)
{
    u8 bits{0};

    if (top)
        bits |= 0b0000'0001;

    if (bottom)
        bits |= 0b0000'0010;

    if (left)
        bits |= 0b0000'0100;

    if (right)
        bits |= 0b0000'1000;

    return frames_for_wires[bits];
}

}