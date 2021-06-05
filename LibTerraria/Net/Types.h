/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Vector.h>
#include <AK/String.h>
#include <AK/Types.h>
#include <AK/Optional.h>
#include <AK/Stream.h>

namespace Terraria::Net
{
// We can't just overload operator>> and operator<< because
// it might be ambiguous as to how we want to write the data.
class Types
{
public:
    static size_t write_7bit_encoded_int(OutputStream& stream, u32 value)
    {
        // This implements a .NET System.IO.BinaryWriter#Write7BitEncodedInt
        // This specific implementation is borrowed (stolen) from mscorlib by Microsoft.

        size_t bytes_written = 1;

        while (value >= 0x80)
        {
            stream << static_cast<u8>(value | 0x80);
            value >>= 7;
            bytes_written++;
        }

        stream << static_cast<u8>(value);
        return bytes_written;
    }

    static size_t read_7bit_encoded_int(InputStream& stream, u32& value)
    {
        // This implements a .NET System.IO.BinaryReader#Read7BitEncodedInt
        // This specific implementation is borrowed (stolen) from mscorlib by Microsoft.

        value = 0;
        u32 shift = 0;
        u8 b;
        do
        {
            if (shift == 5 * 7)
            {
                dbgln("7-bit encoded int has too many bytes!");
                stream.set_fatal_error();
                return 0;
            }

            stream >> b;
            value |= (b & 0x7F) << shift;
            shift += 7;
        } while ((b & 0x80) != 0);
        return shift / 7;
    }

    static size_t write_string(OutputStream& stream, const String& value)
    {
        auto bytes_written = write_7bit_encoded_int(stream, value.length());
        bytes_written += stream.write(value.bytes());
        return bytes_written;
    }

    static size_t read_string(InputStream& stream, String& value)
    {
        u32 length;
        auto bytes_read = read_7bit_encoded_int(stream, length);
        Vector<u8> characters;
        characters.grow_capacity(length);
        bytes_read += stream.read({characters.data(), length});
        value = String(reinterpret_cast<char*>(characters.data()), length);
        return bytes_read;
    }
};
}

template<typename T>
OutputStream& operator<<(OutputStream& stream, const Optional<T> value)
{
    if (value.has_value())
        stream << *value;
    return stream;
}