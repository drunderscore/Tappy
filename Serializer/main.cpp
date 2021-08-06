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

void generate_item_models(const JsonArray& items);

void generate_tile_models(const JsonArray& items);

void generate_wall_models(const JsonArray& items);

void generate_prefix_models(const JsonArray& items);

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
    Field(String name, String type, Optional<String> when) : m_name(move(name)), m_type(move(type)), m_when(move(when))
    {}

    const String& name() const
    { return m_name; }

    const String& type() const
    { return m_type; }

    const Optional<String>& when() const
    { return m_when; }

private:
    String m_name;
    String m_type;
    Optional<String> m_when;
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
    if (!file->open(Core::OpenMode::ReadOnly))
    {
        warnln("Failed to open file.");
        return 2;
    }

    auto content = file->read_all();

    if (action == "stringify")
    {
        stringify(name, content);
    }
    else if (action == "item_model")
    {
        auto json = JsonValue::from_string(content);
        if (!json.has_value() || !json->is_array())
        {
            warnln("Invalid JSON format.");
            return 3;
        }

        auto json_array = json->as_array();
        generate_item_models(json_array);
    }
    else if (action == "tile_model")
    {
        auto json = JsonValue::from_string(content);
        if (!json.has_value() || !json->is_array())
        {
            warnln("Invalid JSON format.");
            return 3;
        }

        auto json_array = json->as_array();
        generate_tile_models(json_array);
    }
    else if (action == "wall_model")
    {
        auto json = JsonValue::from_string(content);
        if (!json.has_value() || !json->is_array())
        {
            warnln("Invalid JSON format.");
            return 3;
        }

        auto json_array = json->as_array();
        generate_wall_models(json_array);
    }
    else if (action == "prefix_model")
    {
        auto json = JsonValue::from_string(content);
        if (!json.has_value() || !json->is_array())
        {
            warnln("Invalid JSON format.");
            return 3;
        }

        auto json_array = json->as_array();
        generate_prefix_models(json_array);
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
                                            auto when_obj = value.as_object().get("when");
                                            Optional<String> when = when_obj.type() == AK::JsonValue::Type::String ?
                                                                    when_obj.as_string() : Optional<String>{};
                                            fields.append(Field(move(name), move(type), move(when)));
                                        });

        AK::LexicalPath lexical_path(input_file_path);
        Optional<String> module;
        auto module_obj = json_object.get("module");
        if (module_obj.type() == AK::JsonValue::Type::String)
            module = module_obj.as_string();

        auto class_name = module.has_value() ? *module : lexical_path.title().to_string();

        outln("#pragma once");
        outln();
        outln("#include <AK/ByteBuffer.h>");
        outln("#include <AK/Optional.h>");
        outln("#include <AK/String.h>");
        outln("#include <AK/Stream.h>");
        outln("#include <LibTerraria/Net/Types.h>");
        outln("#include <LibTerraria/Net/NetworkText.h>");
        outln("#include <LibTerraria/Net/Packet.h>");
        outln("#include <LibTerraria/Color.h>");
        outln("#include <LibTerraria/Item.h>");
        outln("#include <LibTerraria/PlayerInventory.h>");
        outln("#include <LibTerraria/Player.h>");
        outln("#include <LibTerraria/PlayerDeathReason.h>");
        outln("#include <LibTerraria/TileModification.h>");
        outln("#include <LibTerraria/Character.h>");
        outln("#include <AK/MemoryStream.h>");
        outln();
        outln("// This was auto-generated from {}", lexical_path);
        if (module.has_value())
            outln("namespace Terraria::Net::Packets::Modules");
        else
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
            if (field.when().has_value() && *field.when() != "read")
                continue;

            if (field.type() == "String")
            {
                outln("Terraria::Net::Types::read_string(stream, packet.m_{});", field.name());
            }
            else if (field.type().starts_with("Array"))
            {
                StringView array_type;
                StringView array_length;
                get_array_info(field.type(), array_type, array_length);
                outln("stream.read({{ packet.m_{}.data(), packet.m_{}.size() * sizeof({}) }});", field.name(),
                      field.name(), array_type);
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
        if (module.has_value())
        {
            outln("static constexpr auto packet_id = Terraria::Net::Packet::Id::NetModules;");
            outln("static constexpr auto module_id = Terraria::Net::Packet::ModuleId::{};", class_name);
        }
        else
        {
            outln("static constexpr auto packet_id = Terraria::Net::Packet::Id::{};", class_name);
        }

        outln("DuplexMemoryStream stream;");
        outln("stream << packet_id;");
        if (module.has_value())
            outln("stream << module_id;");
        for (auto& field : fields)
        {
            if (field.when().has_value() && *field.when() != "write")
                continue;

            if (field.type() == "String")
            {
                outln("Terraria::Net::Types::write_string(stream, m_{});", field.name());
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

        outln("return stream.copy_into_contiguous_buffer();");

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
            else if (field.type() == "PlayerDeathReason")
            {
                outln("const PlayerDeathReason& {}() const {{ return m_{}; }}", field.name(), field.name());
                outln("void set_{}(PlayerDeathReason value) {{ m_{} = move(value); }}", field.name(), field.name());
            }
            else if (field.type() == "Terraria::Character")
            {
                outln("const Terraria::Character& {}() const {{ return m_{}; }}", field.name(), field.name());
                outln("void set_{}(Terraria::Character value) {{ m_{} = move(value); }}", field.name(), field.name());
            }
            else if (field.type().starts_with("Array"))
            {
                outln("const {}& {}() const {{ return m_{}; }}", field.type(), field.name(), field.name());
                outln("{}& {}() {{ return m_{}; }}", field.type(), field.name(), field.name());
            }
            else if (field.type().starts_with("Terraria::TilePoint"))
            {
                outln("const Terraria::TilePoint& {}() const {{ return m_{}; }}", field.name(), field.name());
                outln("Terraria::TilePoint& {}() {{ return m_{}; }}", field.name(), field.name());
            }
            else if (field.type().starts_with("Terraria::TileModification"))
            {
                outln("const Terraria::TileModification& {}() const {{ return m_{}; }}", field.name(), field.name());
                outln("Terraria::TileModification& {}() {{ return m_{}; }}", field.name(), field.name());
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
            if (field.type() != "String" && field.type() != "NetworkText" && !field.type().starts_with("Array") &&
                !field.type().starts_with("Terraria::Character"))
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