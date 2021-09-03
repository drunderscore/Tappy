/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Array.h>
#include <AK/Optional.h>
#include <AK/Types.h>
#include <LibTerraria/Point.h>

namespace Terraria
{
class NPC
{
public:
    using AIArray = Array<Optional<float>, 4>;

    explicit NPC(i16 id) : m_id(id) {}

    NPC() = default;

    i16 id() const { return m_id; }

    const EntityPoint& position() const { return m_position; }

    const EntityPoint& velocity() const { return m_velocity; }

    EntityPoint& position() { return m_position; }

    EntityPoint& velocity() { return m_velocity; }

    u16 target() const { return m_target; }

    void set_target(u16 value) { m_target = value; }

    i32 hp() const { return m_hp; }

    i16 type() const { return m_type; }

    void set_id(i16 value) { m_id = value; }

    void set_hp(i32 value) { m_hp = value; }

    void set_type(i16 value) { m_type = value; }

    bool direction() const { return m_direction; }

    void set_direction(bool value) { m_direction = value; }

    bool direction_y() const { return m_direction_y; }

    void set_direction_y(bool value) { m_direction_y = value; }

    const AIArray& ai() const { return m_ai; }

    AIArray& ai() { return m_ai; }

    bool sprite_direction() const { return m_sprite_direction; }

    void set_sprite_direction(bool value) { m_sprite_direction = value; }

    const Optional<u8>& scaled_for_this_many_players() const { return m_scaled_for_this_many_players; }

    Optional<u8>& scaled_for_this_many_players() { return m_scaled_for_this_many_players; }

    bool spawned_from_statue() const { return m_spawned_from_statue; }

    void set_spawned_from_statue(bool value) { m_spawned_from_statue = value; }

    const Optional<float> strength_multiplier() const { return m_strength_multiplier; }

    Optional<float> strength_multiplier() { return m_strength_multiplier; }

    const Optional<u8> release_owner() const { return m_release_owner; }

    Optional<u8> release_owner() { return m_release_owner; }

private:
    i16 m_id{};
    EntityPoint m_position;
    EntityPoint m_velocity;
    u16 m_target{};
    i32 m_hp{};
    i16 m_type{};
    bool m_direction{};   // FIXME: Magic value
    bool m_direction_y{}; // FIXME: Magic value
    AIArray m_ai{};
    bool m_sprite_direction{};                     // FIXME: Magic value
    Optional<u8> m_scaled_for_this_many_players{}; // FIXME: This name sucks
    bool m_spawned_from_statue{};
    Optional<float> m_strength_multiplier{};
    Optional<u8> m_release_owner{};
};
}