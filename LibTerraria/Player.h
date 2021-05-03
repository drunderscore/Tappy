/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <LibTerraria/PlayerInventory.h>
#include <LibTerraria/Character.h>

namespace Terraria
{
class Player
{
public:
    explicit Player(Character c) : m_character(move(c))
    {}

    const Character& character() const
    { return m_character; }

    const PlayerInventory& inventory() const
    { return m_inventory; }

    PlayerInventory& inventory()
    { return m_inventory; }

private:
    Character m_character;
    PlayerInventory m_inventory;
};
}