/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Model.h>
#include <LibTerraria/TileMap.h>

// FIXME: Do what Serenity does with their debug macros
#define SLOPE_DEBUG 0

namespace Terraria
{
void TileMap::process_tile_modification(const Terraria::TileModification& modification)
{
    auto& pos = modification.position;
    auto& tile = at(pos);

    switch (modification.action)
    {
        case 0:
            // FIXME: No tile drop
        case 4:
        {
            auto success = !modification.flags_1;
            if (success)
                tile.block() = {};
            else
            {
                // Picking at grass will remove it's grass
                if (tile.block()->id() == Tile::Block::Id::Grass)
                    tile.block()->set_id(Tile::Block::Id::Dirt);
            }
            break;
        }
        case 1:
        case 21:
        {
            auto block_id = static_cast<Terraria::Tile::Block::Id>(modification.flags_1);
            auto style = modification.flags_2;
            tile.block() = Tile::Block(block_id);
            if (block_id == Tile::Block::Id::Torches)
            {
                // FIXME: This depends on how the torch is placed! (background/floor, right/left block side, unlit)
                // Upon further testing, it seems the client just deals with it, because we tell it to re-frame the
                // section, so it fixes the torch placement... still not correct on our side though!
                tile.block()->frame_x() = 0;
                tile.block()->frame_y() = Tile::frame_y_for_style(style);
            }
            else if (Terraria::s_tiles[(int)block_id].frame_important)
            {
                // This tile is frame important, and the flags may tell us what the frame x or frame y should be
                // But we don't handle those cases yet, so just zero them.
                tile.block()->frame_x() = 0;
                tile.block()->frame_y() = 0;
            }
            break;
        }
        case 2:
        {
            auto success = !modification.flags_1;
            if (success)
                tile.wall_id() = {};
            break;
        }
        case 3:
        case 22:
            tile.wall_id() = static_cast<Terraria::Tile::WallId>(modification.flags_1);
            break;
        case 5:
            tile.set_red_wire(true);
            break;
        case 6:
            tile.set_red_wire(false);
            break;
        case 7:
            tile.block()->set_shape(1);
            dbgln_if(SLOPE_DEBUG, "Tile modify 7 with {}, we set the shape to 1", modification.flags_1);
            break;
        case 8:
            tile.set_has_actuator(true);
            break;
        case 9:
            tile.set_has_actuator(false);
            break;
        case 10:
            tile.set_blue_wire(true);
            break;
        case 11:
            tile.set_blue_wire(false);
            break;
        case 12:
            tile.set_green_wire(true);
            break;
        case 13:
            tile.set_green_wire(false);
            break;
        case 14:
        {
            auto shape = modification.flags_1 == 0 ? 0 : modification.flags_1 + 1;
            tile.block()->set_shape(shape);
            dbgln_if(SLOPE_DEBUG, "Tile modify 14 with {}, we set the shape to {}", modification.flags_1, shape);
        }
        break;
        case 16:
            tile.set_yellow_wire(true);
            break;
        case 17:
            tile.set_yellow_wire(false);
            break;
        default:
            dbgln("We are not handling tile modification action {}!", modification.action);
    }
}

void TileMap::place_object(const TilePoint& position, const Model::TileObject& object, i16 style, u8 alternate,
                           i8 random, bool direction)
{
    auto root = position - object.origin;
    auto real_style = style * object.style_multiplier;
    // FIXME: This below
    real_style += object.style;
    if (random > 0)
        real_style += random;

    auto real_style_other = 0;
    if (object.style_wrap_limit > 0)
    {
        real_style_other = real_style / object.style_wrap_limit * object.style_line_skip;
        real_style %= object.style_wrap_limit;
    }

    i16 initial_frame_x;
    i16 initial_frame_y;

    if (object.style_horizontal)
    {
        initial_frame_x = object.coordinate_full_width * real_style;
        initial_frame_y = object.coordinate_full_height * real_style_other;
    }
    else
    {
        initial_frame_x = object.coordinate_full_width * real_style_other;
        initial_frame_y = object.coordinate_full_height * real_style;
    }

    // TODO: Kill tiles in the way, anchors, correct sloped tiles
    //       Some tiles may have custom placement hooks that are used. We will have to replicate those as well.

    for (auto x = 0; x < object.width; x++)
    {
        auto frame_x = initial_frame_x + x * (object.coordinate_width + object.coordinate_padding);
        auto frame_y = initial_frame_y;
        for (auto y = 0; y < object.height; y++)
        {
            // TODO: Break tiles as necessary
            auto& tile = at(root.x() + x, root.y() + y) = Tile(Tile::Block(static_cast<Tile::Block::Id>(object.type)));
            tile.block()->frame_x() = frame_x;
            tile.block()->frame_y() = frame_y;

            frame_y += object.coordinate_heights[y] + object.coordinate_padding;
        }
    }
}
}