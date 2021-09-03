/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <LibTerraria/Item.h>
#include <LibTerraria/Point.h>

namespace Terraria
{
class DroppedItem
{
public:
    const Item& item() const { return m_item; }

    const EntityPoint& position() const { return m_position; }

    const EntityPoint& velocity() const { return m_velocity; }

    Item& item() { return m_item; }

    EntityPoint& position() { return m_position; }

    EntityPoint& velocity() { return m_velocity; }

    const Optional<u8>& owner() const { return m_owner; }

    Optional<u8>& owner() { return m_owner; }

private:
    Item m_item;
    EntityPoint m_position{};
    EntityPoint m_velocity{};
    Optional<u8> m_owner{};
};
}