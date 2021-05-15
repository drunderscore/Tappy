/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Optional.h>
#include <AK/Array.h>
#include <AK/String.h>
#include <LibTerraria/Point.h>

namespace Terraria
{
class Projectile
{
public:
    using AIArray = Array<Optional<float>, 2>;

    explicit Projectile(i16 id) : m_id(id)
    {}

    Projectile() = default;

    i16 id() const
    { return m_id; }

    void set_id(i16 value)
    { m_id = value; }

    const EntityPoint& position() const
    { return m_position; }

    const EntityPoint& velocity() const
    { return m_velocity; }

    EntityPoint& position()
    { return m_position; }

    EntityPoint& velocity()
    { return m_velocity; }

    u8 owner() const
    { return m_owner; }

    void set_owner(u8 value)
    { m_owner = value; }

    i16 type() const
    { return m_type; }

    void set_type(i16 value)
    { m_type = value; }

    const AIArray& ai() const
    { return m_ai; }

    AIArray& ai()
    { return m_ai; }

    const Optional<u16>& banner_id_to_respond_to() const
    { return m_banner_id_to_respond_to; }

    const Optional<i16>& damage() const
    { return m_damage; }

    Optional<i16>& damage()
    { return m_damage; }

    const Optional<float>& knockback() const
    { return m_knockback; }

    const Optional<i16>& original_damage() const
    { return m_original_damage; }

    const Optional<i16>& uuid() const
    { return m_uuid; }

    Optional<u16>& banner_id_to_respond_to()
    { return m_banner_id_to_respond_to; }

    Optional<float>& knockback()
    { return m_knockback; }

    Optional<i16>& original_damage()
    { return m_original_damage; }

    Optional<i16>& uuid()
    { return m_uuid; }

private:
    i16 m_id{};
    EntityPoint m_position;
    EntityPoint m_velocity;
    u8 m_owner{};
    i16 m_type{};
    AIArray m_ai{};
    Optional<u16> m_banner_id_to_respond_to{};
    Optional<i16> m_damage{};
    Optional<float> m_knockback{};
    Optional<i16> m_original_damage{};
    Optional<i16> m_uuid{};
};
}