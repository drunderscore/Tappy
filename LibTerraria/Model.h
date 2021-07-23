/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Optional.h>
#include <AK/StringView.h>

namespace Terraria
{
namespace Model
{
struct Item
{
    StringView english_name;
    i16 max_stack_size{};
    Optional<i16> create_tile;
};

struct Tile
{
    StringView internal_name;
    bool frame_important{};
    bool solid{};
    bool solid_top{};
    // This is stone-like, all these are framed the same way.
    // Stone doesn't actually set this, but it should be treated the same way.
    bool stone{};
};


struct Wall
{
    StringView internal_name;
};
}

extern Terraria::Model::Item s_items[];
extern Terraria::Model::Tile s_tiles[];
extern Terraria::Model::Wall s_walls[];

extern const int s_total_items;
extern const int s_total_tiles;
extern const int s_total_walls;
}