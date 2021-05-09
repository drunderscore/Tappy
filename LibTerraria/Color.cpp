/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Color.h>

InputStream& operator>>(InputStream& stream, Terraria::Color& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Color value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}