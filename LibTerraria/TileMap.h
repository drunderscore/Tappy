/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <AK/Array.h>
#include <LibTerraria/Tile.h>
#include <LibTerraria/Point.h>
#include <LibTerraria/Net/Packets/ModifyTile.h>

namespace Terraria
{
class TileMap
{
public:
    virtual u16 width() const = 0;

    virtual u16 height() const = 0;

    virtual const Tile& at(const TilePoint& position) const = 0;

    virtual Tile& at(const TilePoint& position) = 0;

    virtual const Span<const Tile> tiles() const = 0;

    virtual Span<Tile> tiles() = 0;

    virtual void process_tile_modification(const Terraria::Net::Packets::ModifyTile&);

    constexpr size_t index_for_position(const TilePoint& point) const
    {
        return point.x + (width() * point.y);
    }
};

class MemoryTileMap : public TileMap
{
public:
    MemoryTileMap(u16 width, u16 height) : m_width(width), m_height(height)
    {
        m_tiles.resize(width * height);
    }

    u16 width() const override
    { return m_width; }

    u16 height() const override
    { return m_height; }

    const Tile& at(const TilePoint& position) const override
    {
        return m_tiles.at(index_for_position(position));
    }

    Tile& at(const TilePoint& position) override
    {
        return m_tiles.at(index_for_position(position));
    }

    ALWAYS_INLINE Tile& at(u16 x, u16 y)
    {
        return at({x, y});
    }

    ALWAYS_INLINE const Tile& at(u16 x, u16 y) const
    {
        return at({x, y});
    }

    const Span<const Tile> tiles() const override
    {
        return m_tiles.span();
    }

    Span<Tile> tiles() override
    {
        return m_tiles.span();
    }

private:
    const u16 m_width;
    const u16 m_height;
    Vector<Tile> m_tiles;
};
}