#include <AK/Format.h>
#include <AK/JsonObject.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <AK/String.h>
#include <AK/LexicalPath.h>

class Field
{
public:
    Field(String name, String type) : m_name(move(name)), m_type(move(type))
    {}

    const String& name() const
    { return m_name; }

    const String& type() const
    { return m_type; }

private:
    String m_name;
    String m_type;
};

int main(int argc, char** argv)
{
    Core::ArgsParser args_parser;
    String serial_file_path;
    args_parser.add_positional_argument(serial_file_path, "Path to the serial file", "file");

    if (!args_parser.parse(argc, argv))
        return 1;

    auto file = Core::File::construct(serial_file_path);
    if (!file->open(Core::IODevice::OpenMode::ReadOnly))
    {
        warnln("Failed to open file.");
        return 2;
    }

    auto json = JsonValue::from_string(file->read_all());
    if (!json.has_value() || !json->is_object())
    {
        warnln("Invalid JSON format.");
        return 3;
    }

    auto json_object = json->as_object();
    auto packet_id = json_object.get("id");
    if (packet_id.type() != AK::JsonValue::Type::UnsignedInt32)
    {
        warnln("No packet id.");
        return 4;
    }

    auto json_fields = json_object.get("fields");
    if (json_fields.type() != AK::JsonValue::Type::Array)
    {
        warnln("No fields.");
        return 5;
    }

    Vector<Field> fields;

    json_fields.as_array().for_each([&](auto& value)
                                    {
                                        if (!value.is_object())
                                            return;

                                        auto name = value.as_object().get("name").as_string();
                                        auto type = value.as_object().get("type").as_string();
                                        fields.append(Field(move(name), move(type)));
                                    });

    AK::LexicalPath lexical_path(serial_file_path);
    auto& class_name = lexical_path.title();

    outln("#pragma once");
    outln();
    outln("#include <AK/ByteBuffer.h>");
    outln("#include <AK/Optional.h>");
    outln("#include <AK/String.h>");
    outln("#include <AK/Stream.h>");
    outln("#include <LibTerraria/Net/Types.h>");
    outln("#include <LibTerraria/Net/NetworkText.h>");
    outln("#include <LibTerraria/Net/Color.h>");
    outln();
    outln("// This was auto-generated from {}", lexical_path);
    outln("namespace Terraria::Net::Packets");
    outln("{{");
    outln("class {}", class_name);
    outln("{{");

    outln("public:");
    outln("{}() = default;", class_name);
    outln();

    outln("static Optional<{}> from_bytes(InputStream& stream)", class_name);
    outln("{{");

    outln("{} packet;", class_name);
    for (auto& field : fields)
    {
        if (field.type() == "String")
        {
            outln("Terraria::Net::Types::read_string(stream, packet.m_{});", field.name());
        }
        else if (field.type() == "NetworkText")
        {
            outln("auto op_{} = Terraria::Net::NetworkText::from_bytes(stream);", field.name());
            outln("if(!op_{}.has_value()) return {{}};", field.name());
            outln("packet.m_{} = op_{}.value();", field.name(), field.name());
        }
        else
        {
            // Let's be naive and hope an overload solves our problems :^)
            outln("stream >> packet.m_{};", field.name());
        }
    }
    outln("return packet;");

    outln("}}");
    outln();

    outln("ByteBuffer to_bytes()");
    outln("{{");
    outln("static constexpr u8 PACKET_ID = {};", packet_id);
    outln("auto buffer = ByteBuffer::create_uninitialized(256);");
    outln("OutputMemoryStream stream(buffer);");
    outln("stream << PACKET_ID;");
    for (auto& field : fields)
    {
        if (field.type() == "String")
        {
            outln("Terraria::Net::Types::write_string(stream, m_{});", field.name());
        }
        else if (field.type() == "NetworkText")
        {
            outln("stream.write(m_{}.to_bytes());", field.name());
        }
        else
        {
            // Let's be naive and hope the overload solves our problems :^)
            outln("stream << m_{};", field.name());
        }
    }

    outln("buffer.trim(stream.size());");
    outln("return buffer;");

    outln("}}");
    outln();
    for (auto& field : fields)
    {
        if (field.type() == "String")
        {
            outln("const String& {}() const {{ return m_{}; }}", field.name(), field.name());
            outln("void set_{}(String value) {{ m_{} = move(value); }}", field.name(), field.name());
        }
        else if (field.type() == "NetworkText")
        {
            outln("const NetworkText& {}() const {{ return m_{}; }}", field.name(), field.name());
            outln("void set_{}(NetworkText value) {{ m_{} = move(value); }}", field.name(), field.name());
        }
        else
        {
            // Let's be naive and hope this type is trivial
            outln("{} {}() const {{ return m_{}; }}", field.type(), field.name(), field.name());
            outln("void set_{}({} value) {{ m_{} = value; }}", field.name(), field.type(), field.name());
        }
    }

    outln();
    outln("private:");
    for (auto& field : fields)
    {
        // This should be correct for most cases.
        outln("{} m_{};", field.type(), field.name());
    }

    outln("}};");
    outln("}}");
}