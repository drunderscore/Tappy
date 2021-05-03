/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/PlayerInventory.h>

InputStream& operator>>(InputStream& stream, Terraria::PlayerInventory::Slot& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::PlayerInventory::Slot value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}