/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Optional.h>
#include <AK/Stream.h>
#include <LibTerraria/Net/Types.h>
#include <AK/MemoryStream.h>

namespace Terraria::Net
{
class NetworkText
{
public:
    NetworkText() = default;

    NetworkText(String text) : m_text(move(text))
    {}

    NetworkText(const char* text) : NetworkText(String(text))
    {}

    static Optional<NetworkText> from_bytes(InputStream& stream)
    {
        NetworkText network_text;
        u8 mode;
        stream >> mode;
        network_text.m_mode = static_cast<Mode>(mode);
        Types::read_string(stream, network_text.m_text);
        return network_text;
    }

    ByteBuffer to_bytes() const
    {
        // TODO: Support non-literal network text
        VERIFY(m_mode == Mode::Literal);
        auto bytes = ByteBuffer::create_uninitialized(256);
        AK::OutputMemoryStream stream(bytes);
        stream << static_cast<u8>(m_mode);
        Types::write_string(stream, m_text);
        bytes.trim(stream.size());
        return bytes;
    }

    enum class Mode : u8
    {
        Literal,
        Formattable,
        LocalizationKey
    };

    const String& as_literal_string() const
    { return m_text; }

private:
    String m_text;
    Mode m_mode{Mode::Literal};
};
}