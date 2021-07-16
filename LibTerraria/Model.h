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
};
}

extern Terraria::Model::Item s_items[];
extern Terraria::Model::Tile s_tiles[];
}