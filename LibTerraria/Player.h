/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Weakable.h>
#include <LibTerraria/PlayerInventory.h>
#include <LibTerraria/Character.h>
#include <LibTerraria/Point.h>

namespace Terraria
{
class Player : public Weakable<Player>
{
public:
    using BuffsArray = Array<u16, 22>;

    const Character& character() const
    { return m_character; }

    Character& character()
    { return m_character; }

    const PlayerInventory& inventory() const
    { return m_inventory; }

    PlayerInventory& inventory()
    { return m_inventory; }

    const BuffsArray& buffs() const
    { return m_buffs; }

    BuffsArray& buffs()
    { return m_buffs; }

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

    const EntityPoint& position() const
    { return m_position; }

    const EntityPoint& velocity() const
    { return m_velocity; }

    EntityPoint& position()
    { return m_position; }

    EntityPoint& velocity()
    { return m_velocity; }

    u8 control_bits() const
    { return m_control_bits; }

    u8 bits_2() const
    { return m_bits_2; }

    u8 bits_3() const
    { return m_bits_3; }

    u8 bits_4() const
    { return m_bits_4; }

    void set_control_bits(u8 value)
    { m_control_bits = value; }

    void set_bits_2(u8 value)
    { m_bits_2 = value; }

    void set_bits_3(u8 value)
    { m_bits_3 = value; }

    void set_bits_4(u8 value)
    { m_bits_4 = value; }

    bool pvp() const
    { return m_pvp; }

    void set_pvp(bool value)
    { m_pvp = value; }

private:
    i16 m_hp{};
    i16 m_max_hp{};
    i16 m_mana{};
    i16 m_max_mana{};
    Character m_character;
    PlayerInventory m_inventory;
    BuffsArray m_buffs{}; // The network is limited to 22 concurrent buffs.
    EntityPoint m_position{};
    EntityPoint m_velocity{};

    u8 m_control_bits{};
    u8 m_bits_2{};
    u8 m_bits_3{};
    u8 m_bits_4{};
    u8 m_selected_item{};

    bool m_pvp{};
};
}