/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Character.h>
#include <LibTerraria/Net/Types.h>

InputStream& operator>>(InputStream& stream, Terraria::Character& value)
{
    u8 temporary_byte;
    Terraria::Color temporary_color;

    stream >> temporary_byte;
    value.set_skin_variant(temporary_byte);

    stream >> temporary_byte;
    value.set_hair(temporary_byte);

    String name;
    Terraria::Net::Types::read_string(stream, name);
    value.set_name(name);

    stream >> temporary_byte;
    value.set_hair_dye(temporary_byte);

    stream >> temporary_byte;
    value.set_hide_visuals(temporary_byte);

    stream >> temporary_byte;
    value.set_hide_visuals2(temporary_byte);

    stream >> temporary_byte;
    value.set_hide_misc(temporary_byte);

    stream >> temporary_color;
    value.set_hair_color(temporary_color);

    stream >> temporary_color;
    value.set_skin_color(temporary_color);

    stream >> temporary_color;
    value.set_eye_color(temporary_color);

    stream >> temporary_color;
    value.set_shirt_color(temporary_color);

    stream >> temporary_color;
    value.set_undershirt_color(temporary_color);

    stream >> temporary_color;
    value.set_pants_color(temporary_color);

    stream >> temporary_color;
    value.set_shoe_color(temporary_color);

    stream >> temporary_byte;
    value.set_difficulty_flags(temporary_byte);

    stream >> temporary_byte;
    value.set_torch_flags(temporary_byte);

    return stream;
}

OutputStream& operator<<(OutputStream& stream, const Terraria::Character& value)
{
    stream << value.skin_variant();
    stream << value.hair();
    Terraria::Net::Types::write_string(stream, value.name());
    stream << value.hair_dye();
    stream << value.hide_visuals();
    stream << value.hide_visuals2();
    stream << value.hide_misc();
    stream << value.hair_color();
    stream << value.skin_color();
    stream << value.eye_color();
    stream << value.shirt_color();
    stream << value.undershirt_color();
    stream << value.pants_color();
    stream << value.shoe_color();
    stream << value.difficulty_flags();
    stream << value.torch_flags();

    return stream;
}