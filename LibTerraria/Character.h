/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <LibTerraria/Color.h>
#include <AK/String.h>

namespace Terraria
{
class Character
{
public:
    u8 skin_variant() const
    { return m_skin_variant; }

    u8 hair() const
    { return m_hair; }

    const String& name() const
    { return m_name; }

    u8 hair_dye() const
    { return m_hair_dye; }

    u8 hide_visuals() const
    { return m_hide_visuals; }

    u8 hide_visuals2() const
    { return m_hide_visuals2; }

    u8 hide_misc() const
    { return m_hide_misc; }

    const Color& hair_color() const
    { return m_hair_color; }

    const Color& skin_color() const
    { return m_skin_color; }

    const Color& eye_color() const
    { return m_eye_color; }

    const Color& shirt_color() const
    { return m_shirt_color; }

    const Color& undershirt_color() const
    { return m_undershirt_color; }

    const Color& pants_color() const
    { return m_pants_color; }

    const Color& shoe_color() const
    { return m_shoe_color; }

    u8 difficulty_flags() const
    { return m_difficulty_flags; }

    u8 torch_flags() const
    { return m_torch_flags; }

    void set_skin_variant(u8 value)
    { m_skin_variant = value; }

    void set_hair(u8 value)
    { m_hair = value; }

    void set_name(String value)
    { m_name = move(value); }

    void set_hair_dye(u8 value)
    { m_hair_dye = value; }

    void set_hide_visuals(u8 value)
    { m_hide_visuals = value; }

    void set_hide_visuals2(u8 value)
    { m_hide_visuals2 = value; }

    void set_hide_misc(u8 value)
    { m_hide_misc = value; }

    void set_hair_color(Color value)
    { m_hair_color = value; }

    void set_skin_color(Color value)
    { m_skin_color = value; }

    void set_eye_color(Color value)
    { m_eye_color = value; }

    void set_shirt_color(Color value)
    { m_shirt_color = value; }

    void set_undershirt_color(Color value)
    { m_undershirt_color = value; }

    void set_pants_color(Color value)
    { m_pants_color = value; }

    void set_shoe_color(Color value)
    { m_shoe_color = value; }

    void set_difficulty_flags(u8 value)
    { m_difficulty_flags = value; }

    void set_torch_flags(u8 value)
    { m_torch_flags = value; }

private:
    u8 m_skin_variant{};
    u8 m_hair{};
    String m_name;
    u8 m_hair_dye{};
    u8 m_hide_visuals{};
    u8 m_hide_visuals2{};
    u8 m_hide_misc{};
    Color m_hair_color{};
    Color m_skin_color{};
    Color m_eye_color{};
    Color m_shirt_color{};
    Color m_undershirt_color{};
    Color m_pants_color{};
    Color m_shoe_color{};
    u8 m_difficulty_flags{};
    u8 m_torch_flags{};
};
}

InputStream& operator>>(InputStream& stream, Terraria::Character& value);

OutputStream& operator<<(OutputStream& stream, const Terraria::Character& value);
