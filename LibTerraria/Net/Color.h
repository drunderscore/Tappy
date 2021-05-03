/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <AK/Stream.h>

namespace Terraria::Net
{
struct [[gnu::packed]] Color
{
    u8 r{};
    u8 g{};
    u8 b{};
};
}

InputStream& operator>>(InputStream& stream, Terraria::Net::Color& value);

OutputStream& operator<<(OutputStream& stream, Terraria::Net::Color value);