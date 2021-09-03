/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Optional.h>
#include <AK/StringView.h>
#include <LibTerraria/Point.h>

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

struct Prefix
{
    StringView english_name;
    StringView internal_name;
};

struct TileObject
{
    u8 width;
    u8 height;
    i16 type;
    Terraria::TilePoint origin;
    u8 direction;
    u8 style_wrap_limit;
    u8 style_multiplier;
    u8 random_style_range;
    bool style_horizontal;
    u8 style_line_skip;
    u8 coordinate_width;
    u8 coordinate_full_width;
    u8 coordinate_full_height;
    u8 coordinate_padding;
    u8 style;
    Vector<u8> coordinate_heights;
};
}

extern Terraria::Model::Item s_items[];
extern Terraria::Model::Tile s_tiles[];
extern Terraria::Model::Wall s_walls[];
extern Terraria::Model::Prefix s_prefixes[];
extern Terraria::Model::TileObject s_tile_objects[];

extern const int s_total_items;
extern const int s_total_tiles;
extern const int s_total_walls;
extern const int s_total_prefixes;
}