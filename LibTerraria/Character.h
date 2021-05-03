#pragma once

#include <AK/Types.h>
#include <LibTerraria/Net/Color.h>
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

    const Net::Color& hair_color() const
    { return m_hair_color; }

    const Net::Color& skin_color() const
    { return m_skin_color; }

    const Net::Color& eye_color() const
    { return m_eye_color; }

    const Net::Color& shirt_color() const
    { return m_shirt_color; }

    const Net::Color& undershirt_color() const
    { return m_undershirt_color; }

    const Net::Color& pants_color() const
    { return m_pants_color; }

    const Net::Color& shoe_color() const
    { return m_shoe_color; }

    u8 difficulty_flags() const
    { return m_difficulty_flags; }

    u8 torch_flags() const
    { return m_torch_flags; }

    static Character create_from_packet(const Net::Packets::PlayerInfo& player_info)
    {
        Character c;
        c.m_skin_variant = player_info.skin_variant();
        c.m_hair = player_info.hair();
        c.m_name = player_info.name().isolated_copy();
        c.m_hair_dye = player_info.hair_dye();
        c.m_hide_visuals = player_info.hide_visuals();
        c.m_hide_visuals2 = player_info.hide_visuals2();
        c.m_hide_misc = player_info.hide_misc();
        c.m_hair_color = player_info.hair_color();
        c.m_skin_color = player_info.skin_color();
        c.m_eye_color = player_info.eye_color();
        c.m_shirt_color = player_info.shirt_color();
        c.m_undershirt_color = player_info.undershirt_color();
        c.m_pants_color = player_info.pants_color();
        c.m_shoe_color = player_info.shoe_color();
        c.m_difficulty_flags = player_info.difficulty_flags();
        c.m_torch_flags = player_info.torch_flags();

        return c;
    }

private:
    u8 m_skin_variant{};
    u8 m_hair{};
    String m_name;
    u8 m_hair_dye{};
    u8 m_hide_visuals{};
    u8 m_hide_visuals2{};
    u8 m_hide_misc{};
    Net::Color m_hair_color{};
    Net::Color m_skin_color{};
    Net::Color m_eye_color{};
    Net::Color m_shirt_color{};
    Net::Color m_undershirt_color{};
    Net::Color m_pants_color{};
    Net::Color m_shoe_color{};
    u8 m_difficulty_flags{};
    u8 m_torch_flags{};
};
}