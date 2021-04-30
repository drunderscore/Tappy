#pragma once

#include <AK/Types.h>
#include <AK/Stream.h>

struct [[gnu::packed]] Color
{
    u8 r{};
    u8 g{};
    u8 b{};
};

namespace AK::Detail
{
InputStream& operator>>(InputStream& stream, Color& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Color value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}

}