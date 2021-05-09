/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <AK/Array.h>
#include <LibTerraria/Tile.h>

namespace Terraria
{
class TileMap
{
public:
    TileMap(u16 width, u16 height) :
            m_width(width),
            m_height(height)
    {
        m_tiles.resize(width * height);
    }

    Tile& at(u16 x, u16 y)
    {
        auto index = index_for_position(x, y);
        VERIFY(index <= m_tiles.size());
        return m_tiles.at(index);
    }

    const Tile& at(u16 x, u16 y) const
    {
        auto index = index_for_position(x, y);
        VERIFY(index <= m_tiles.size());
        return m_tiles.at(index);
    }

    const Vector<Tile>& tiles() const
    { return m_tiles; }

    Vector<Tile>& tiles()
    { return m_tiles; }

    size_t index_for_position(u16 x, u16 y) const
    {
        return x + (m_width * y);
    }

    u16 width() const
    { return m_width; }

    u16 height() const
    { return m_height; }

private:
    const u16 m_width;
    const u16 m_height;
    Vector<Tile> m_tiles;
};
}