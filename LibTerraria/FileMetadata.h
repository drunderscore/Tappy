/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Stream.h>
#include <AK/Types.h>

namespace Terraria
{
class FileMetadata
{
public:
    enum class FileType : u8
    {
        None,
        Map,
        World,
        Player
    };

    FileType type() const { return m_type; }

    u32 revision() const { return m_revision; }

    u64 flags() const { return m_flags; }

    void set_type(FileType value) { m_type = value; }

    void set_revision(u32 value) { m_revision = value; }

    void set_flags(u64 value) { m_flags = value; }

private:
    u32 m_revision{};
    FileType m_type{};
    u64 m_flags{};
};
}

OutputStream& operator<<(OutputStream&, const Terraria::FileMetadata&);

InputStream& operator>>(InputStream&, Terraria::FileMetadata&);