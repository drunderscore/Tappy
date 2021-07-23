/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <AK/String.h>
#include <AK/HashMap.h>
#include <LibTerraria/Point.h>
#include <LibTerraria/Item.h>

namespace Terraria
{
class Chest
{
public:
    // FIXME: What does our name start out as? empty? "Chest"? is it localized?
    Chest() = default;

    const TilePoint& position() const
    { return m_position; }

    TilePoint& position()
    { return m_position; }

    const HashMap<u16, Item>& contents() const
    { return m_contents; }

    HashMap<u16, Item>& contents()
    { return m_contents; }

    const String& name() const
    { return m_name; }

    void set_name(String value)
    { m_name = move(value); }

private:
    HashMap<u16, Item> m_contents;
    TilePoint m_position{};
    String m_name;
};
}