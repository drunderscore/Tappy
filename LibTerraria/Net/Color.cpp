#include <LibTerraria/Net/Color.h>

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