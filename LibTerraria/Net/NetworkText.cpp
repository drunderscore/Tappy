/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/NetworkText.h>

InputStream& operator>>(InputStream& stream, Terraria::Net::NetworkText::Mode& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Net::NetworkText::Mode value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}

InputStream& operator>>(InputStream& stream, Terraria::Net::NetworkText& value)
{
    Terraria::Net::NetworkText::Mode mode;
    stream >> mode;
    value.set_mode(mode);
    String text;
    Terraria::Net::Types::read_string(stream, text);
    value.set_text(text);
    return stream;
}

OutputStream& operator<<(OutputStream& stream, const Terraria::Net::NetworkText& value)
{
    VERIFY(value.mode() == Terraria::Net::NetworkText::Mode::Literal);
    stream << static_cast<u8>(value.mode());
    Terraria::Net::Types::write_string(stream, value.text());
    return stream;
}