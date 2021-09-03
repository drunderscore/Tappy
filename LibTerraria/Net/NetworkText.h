/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/MemoryStream.h>
#include <AK/Optional.h>
#include <AK/Stream.h>
#include <LibTerraria/Net/Types.h>

namespace Terraria::Net
{
class NetworkText
{
public:
    enum class Mode : u8
    {
        Literal,
        Formattable,
        LocalizationKey
    };

    NetworkText() = default;

    NetworkText(String text) : m_text(move(text)) {}

    NetworkText(const char* text) : NetworkText(String(text)) {}

    Mode mode() const { return m_mode; }

    void set_mode(Mode value) { m_mode = value; }

    const String& text() const { return m_text; }

    void set_text(String value) { m_text = move(value); }

private:
    String m_text;
    Mode m_mode{Mode::Literal};
};
}

InputStream& operator>>(InputStream& stream, Terraria::Net::NetworkText::Mode& value);

OutputStream& operator<<(OutputStream& stream, Terraria::Net::NetworkText::Mode value);

InputStream& operator>>(InputStream& stream, Terraria::Net::NetworkText& value);

OutputStream& operator<<(OutputStream& stream, const Terraria::Net::NetworkText& value);