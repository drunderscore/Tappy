/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Packet.h>

InputStream& operator>>(InputStream& stream, Terraria::Net::Packet::Id& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Net::Packet::Id value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}

InputStream& operator>>(InputStream& stream, Terraria::Net::Packet::ModuleId& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Net::Packet::ModuleId value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}
