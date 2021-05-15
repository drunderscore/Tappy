/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <LibTerraria/Color.h>
#include <LibTerraria/Net/Packets/PlayerInfo.h>

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

    void update_from_packet(const Net::Packets::PlayerInfo& player_info)
    {
        m_skin_variant = player_info.skin_variant();
        m_hair = player_info.hair();
        m_name = player_info.name().isolated_copy();
        m_hair_dye = player_info.hair_dye();
        m_hide_visuals = player_info.hide_visuals();
        m_hide_visuals2 = player_info.hide_visuals2();
        m_hide_misc = player_info.hide_misc();
        m_hair_color = player_info.hair_color();
        m_skin_color = player_info.skin_color();
        m_eye_color = player_info.eye_color();
        m_shirt_color = player_info.shirt_color();
        m_undershirt_color = player_info.undershirt_color();
        m_pants_color = player_info.pants_color();
        m_shoe_color = player_info.shoe_color();
        m_difficulty_flags = player_info.difficulty_flags();
        m_torch_flags = player_info.torch_flags();
    }

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