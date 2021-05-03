#include <AK/Stream.h>
#include <LibTerraria/Item.h>

InputStream& operator>>(InputStream& stream, Terraria::Item::Id& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Item::Id value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}

InputStream& operator>>(InputStream& stream, Terraria::Item::Prefix& value)
{
    stream.read_or_error({&value, sizeof(value)});
    return stream;
}

OutputStream& operator<<(OutputStream& stream, Terraria::Item::Prefix value)
{
    stream.write_or_error({&value, sizeof(value)});
    return stream;
}
