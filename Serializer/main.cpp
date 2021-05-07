/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/Format.h>
#include <AK/JsonObject.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <AK/String.h>
#include <AK/LexicalPath.h>

void stringify(const String& name, const StringView& lines);

void get_array_info(const String& name, StringView& type, StringView& length)
{
    GenericLexer lexer(name);
    lexer.consume_until('<');
    type = lexer.consume_until(',').trim_whitespace();
    length = lexer.consume_until('>').trim_whitespace();
}

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
    String input_file_path;
    String action;
    String name;
    args_parser.add_positional_argument(input_file_path, "Path to the input file", "file");
    args_parser.add_option(action, "Action to perform on the input file", "action", 'a', "action");
    args_parser.add_option(name, "Name to pass to the action", "name", 'n', "name");

    if (!args_parser.parse(argc, argv))
        return 1;

    auto file = Core::File::construct(input_file_path);
    if (!file->open(Core::IODevice::OpenMode::ReadOnly))
    {
        warnln("Failed to open file.");
        return 2;
    }

    auto content = file->read_all();

    if (action == "stringify")
    {
        stringify(name, content);
    }
    else if (action == "packet_definition")
    {
        auto json = JsonValue::from_string(content);
        if (!json.has_value() || !json->is_object())
        {
            warnln("Invalid JSON format.");
            return 3;
        }

        auto json_object = json->as_object();

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

        AK::LexicalPath lexical_path(input_file_path);
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
        outln("#include <LibTerraria/Net/Packet.h>");
        outln("#include <LibTerraria/Item.h>");
        outln("#include <LibTerraria/PlayerInventory.h>");
        outln();
        outln("// This was auto-generated from {}", lexical_path);
        outln("namespace Terraria::Net::Packets");
        outln("{{");
        outln("class {} : public Terraria::Net::Packet", class_name);
        outln("{{");

        outln("public:");
        outln("{}() = default;", class_name);
        outln();

        outln("const char* packet_name() const override");
        outln("{{");
        outln("return \"{}\";", class_name);
        outln("}}");

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
            else if (field.type().starts_with("Array"))
            {
                StringView array_type;
                StringView array_length;
                get_array_info(field.type(), array_type, array_length);
                outln("stream.read({{ packet.m_{}.data(), packet.m_{}.size() * sizeof({}) }});", field.name(), field.name(), array_type);
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

        outln("ByteBuffer to_bytes() const override");
        outln("{{");
        outln("static constexpr auto packet_id = Terraria::Net::Packet::Id::{};", class_name);
        outln("auto buffer = ByteBuffer::create_uninitialized(256);");
        outln("OutputMemoryStream stream(buffer);");
        outln("stream << packet_id;");
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
            else if (field.type().starts_with("Array"))
            {
                StringView array_type;
                StringView array_length;
                get_array_info(field.type(), array_type, array_length);
                outln("stream.write({{ m_{}.data(), m_{}.size() * sizeof({}) }});", field.name(), field.name(),
                      array_type);
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
            else if (field.type().starts_with("Array"))
            {
                outln("const {}& {}() const {{ return m_{}; }}", field.type(), field.name(), field.name());
                outln("{}& {}() {{ return m_{}; }}", field.type(), field.name(), field.name());
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
            if (field.type() != "String" && field.type() != "NetworkText" && !field.type().starts_with("Array"))
                outln("{} m_{}{{}};", field.type(), field.name());
            else
                outln("{} m_{};", field.type(), field.name());
        }

        outln("}};");
        outln("}}");
    }
    else
    {
        warnln("Unknown serializer action {}", action);
        return 6;
    }
}