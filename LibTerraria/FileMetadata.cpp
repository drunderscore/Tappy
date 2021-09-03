/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/Format.h>
#include <LibTerraria/FileMetadata.h>

static constexpr u64 magic = 27981915666277746;

OutputStream& operator<<(OutputStream& stream, const Terraria::FileMetadata& value)
{
    stream << (magic | (static_cast<u64>(value.type()) << 56));
    stream << value.revision();
    stream << value.flags();
    return stream;
}

InputStream& operator>>(InputStream& stream, Terraria::FileMetadata& value)
{
    u64 temporary_64;
    stream >> temporary_64;
    if ((temporary_64 & 72057594037927935) != magic)
    {
        dbgln("File metadata has invalid magic!");
        stream.set_recoverable_error();
        return stream;
    }

    value.set_type(static_cast<Terraria::FileMetadata::FileType>(temporary_64 >> 56));
    u32 temporary_32;
    stream >> temporary_32;
    value.set_revision(temporary_32);
    stream >> temporary_64;
    value.set_flags(temporary_64);

    return stream;
}