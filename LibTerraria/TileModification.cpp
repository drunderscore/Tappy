/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/TileModification.h>

OutputStream& operator<<(OutputStream& stream, const Terraria::TileModification value)
{
    stream << value.action;
    stream << value.position;
    stream << value.flags_1;
    stream << value.flags_2;

    return stream;
}

InputStream& operator>>(InputStream& stream, Terraria::TileModification& value)
{
    stream >> value.action;
    stream >> value.position;
    stream >> value.flags_1;
    stream >> value.flags_2;

    return stream;
}