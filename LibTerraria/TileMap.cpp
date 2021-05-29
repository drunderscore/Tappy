/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/TileMap.h>

namespace Terraria
{
void TileMap::process_tile_modification(const Net::Packets::ModifyTile& modify_tile)
{
    auto& pos = modify_tile.position();
    auto& tile = at(pos);

    switch (modify_tile.action())
    {
        case 0:
            // FIXME: No tile drop
        case 4:
        {
            auto success = !modify_tile.flags_1();
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
            auto block_id = static_cast<Terraria::Tile::Block::Id>(modify_tile.flags_1());
            auto style = modify_tile.flags_2();
            tile.block() = block_id;
            if (block_id == Tile::Block::Id::Torches)
            {
                // FIXME: This depends on how the torch is placed! (background/floor, right/left block side, unlit)
                // Upon further testing, it seems the client just deals with it, because we tell it to re-frame the
                // section, so it fixes the torch placement... still not correct on our side though!
                tile.block()->frame_x() = 0;
                tile.block()->frame_y() = Tile::frame_y_for_style(style);
            }
            break;
        }
        case 2:
            tile.wall_id() = {};
            break;
        case 3:
        case 22:
            tile.wall_id() = static_cast<Terraria::Tile::WallId>(modify_tile.flags_1());
            break;
        case 5:
            tile.set_red_wire(true);
            break;
        case 6:
            tile.set_red_wire(false);
            break;
        case 7:
            tile.block()->set_shape(1);
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
            tile.block()->set_shape(modify_tile.flags_1() + 1);
            break;
        case 16:
            tile.set_yellow_wire(true);
            break;
        case 17:
            tile.set_yellow_wire(false);
            break;
        default:
            dbgln("We are not handling tile modification action {}!", modify_tile.action());
    }
}
}