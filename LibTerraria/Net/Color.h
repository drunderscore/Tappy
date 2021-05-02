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

InputStream& operator>>(InputStream& stream, Terraria::Net::Color& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Net::Color value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}