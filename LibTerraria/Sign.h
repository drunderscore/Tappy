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

namespace Terraria
{
class Sign
{
public:
    Sign() = default;

    const TilePoint& position() const
    { return m_position; }

    TilePoint& position()
    { return m_position; }

    const String& text() const
    { return m_text; }

    void set_text(String value)
    { m_text = move(value); }

private:
    TilePoint m_position{};
    String m_text;
};
}