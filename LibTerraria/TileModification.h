/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Stream.h>
#include <LibTerraria/Point.h>

namespace Terraria
{
struct TileModification
{
    u8 action{};
    TilePoint position{};
    i16 flags_1{};
    u8 flags_2{};
};
}

OutputStream& operator<<(OutputStream& stream, const Terraria::TileModification value);

InputStream& operator>>(InputStream& stream, Terraria::TileModification& value);