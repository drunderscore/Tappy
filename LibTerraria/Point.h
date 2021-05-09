/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <AK/Stream.h>
#include <AK/Format.h>
#include <AK/StringBuilder.h>

namespace Terraria
{
template<typename T>
struct [[gnu::packed]] Point
{
    T x{};
    T y{};
};

using TilePoint = Point<u16>;
using EntityPoint = Point<float>;
}

template<typename T>
InputStream& operator>>(InputStream& stream, Terraria::Point<T>& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

template<typename T>
OutputStream& operator<<(OutputStream& stream, Terraria::Point<T> value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}

template<typename T>
InputStream& operator>>(InputStream& stream, Optional<Terraria::Point<T>>& value)
{
    Terraria::Point<T> temp;
    stream >> temp;
    value = temp;
    return stream;
}

template<typename T>
struct AK::Formatter<Terraria::Point<T>> : AK::Formatter<String>
{
    void format(FormatBuilder& builder, Terraria::Point<T> value)
    {
        builder.builder().appendff("({}, {})", value.x, value.y);
    }
};
