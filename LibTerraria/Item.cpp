/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/Stream.h>
#include <LibTerraria/Item.h>

InputStream& operator>>(InputStream& stream, Terraria::Item::Id& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Item::Id value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}

InputStream& operator>>(InputStream& stream, Terraria::Item::Prefix& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Item::Prefix value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}

InputStream& operator>>(InputStream& stream, Optional<Terraria::Item::Id>& value)
{
    Terraria::Item::Id temp;
    stream >> temp;
    value = temp;
    return stream;
}

InputStream& operator>>(InputStream& stream, Optional<Terraria::Item::Prefix>& value)
{
    Terraria::Item::Prefix temp;
    stream >> temp;
    value = temp;
    return stream;
}