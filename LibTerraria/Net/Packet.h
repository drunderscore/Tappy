/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>

namespace Terraria::Net
{
class Packet
{
public:
    const char* packet_name() const
    { VERIFY_NOT_REACHED(); }

    virtual ByteBuffer to_bytes() const
    { VERIFY_NOT_REACHED(); }
};
}