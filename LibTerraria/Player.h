/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Weakable.h>
#include <LibTerraria/PlayerInventory.h>
#include <LibTerraria/Character.h>

namespace Terraria
{
class Player : public Weakable<Player>
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

    i16 hp() const
    { return m_hp; }

    i16 max_hp() const
    { return m_max_hp; }

    i16 mana() const
    { return m_mana; }

    i16 max_mana() const
    { return m_max_mana; }

    void set_hp(i16 value)
    { m_hp = value; }

    void set_max_hp(i16 value)
    { m_max_hp = value; }

    void set_mana(i16 value)
    { m_mana = value; }

    void set_max_mana(i16 value)
    { m_max_mana = value; }

private:
    i16 m_hp{};
    i16 m_max_hp{};
    i16 m_mana{};
    i16 m_max_mana{};
    Character m_character;
    PlayerInventory m_inventory;
};
}