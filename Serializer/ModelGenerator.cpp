/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/JsonArray.h>
#include <AK/JsonObject.h>

void generate_item_models(const JsonArray& items)
{
    outln("#include <LibTerraria/Model.h>");
    outln("namespace Terraria");
    outln("{{");
    outln("Terraria::Model::Item s_items[] = {{");

    int count = 0;

    // @formatter:off
    items.for_each([&](auto& value)
    {
        auto& value_obj = value.as_object();
        out("{{\"{}\", {}, ",
              value_obj.get("englishName").as_string(),
              value_obj.get("maxStackSize").to_i32());

        auto create_tile = value_obj.get("createTile");
        if(create_tile.is_null())
            out("{{}}");
        else
            out("{}", create_tile.to_i32());

        if(count < items.size() - 1)
            outln("}},");
        else
            outln("}}");
        count++;
    });
    // @formatter:on

    outln("}};");

    outln("const int s_total_items = {};", items.size());

    outln("}}");
}

void generate_tile_models(const JsonArray& items)
{
    outln("#include <LibTerraria/Model.h>");
    outln("namespace Terraria");
    outln("{{");
    outln("Terraria::Model::Tile s_tiles[] = {{");

    int count = 0;

    // @formatter:off
    items.for_each([&](auto& value)
    {
        auto& value_obj = value.as_object();
        out("{{\"{}\", {}, {}, {}, {}", value_obj.get("internalName").as_string(),
            value_obj.get("frameImportant").to_bool(),
            value_obj.get("tileSolid").to_bool(),
            value_obj.get("tileSolidTop").to_bool(),
            value_obj.get("stone").to_bool());

        if(count < items.size() - 1)
            outln("}},");
        else
            outln("}}");
        count++;
    });
    // @formatter:on

    outln("}};");

    outln("const int s_total_tiles = {};", items.size());

    outln("}}");
}

void generate_wall_models(const JsonArray& items)
{
    outln("#include <LibTerraria/Model.h>");
    outln("namespace Terraria");
    outln("{{");
    outln("Terraria::Model::Wall s_walls[] = {{");

    int count = 0;

    // @formatter:off
    items.for_each([&](auto& value)
    {
        auto& value_obj = value.as_object();
        out("{{\"{}\"", value_obj.get("internalName").as_string());

        if(count < items.size() - 1)
            outln("}},");
        else
            outln("}}");
        count++;
    });
    // @formatter:on

    outln("}};");

    outln("const int s_total_walls = {};", items.size());

    outln("}}");
}

void generate_prefix_models(const JsonArray& items)
{
    outln("#include <LibTerraria/Model.h>");
    outln("namespace Terraria");
    outln("{{");
    outln("Terraria::Model::Prefix s_prefixes[] = {{");

    int count = 0;

    // @formatter:off
    items.for_each([&](auto& value)
    {
        if(value.is_null())
        {
            out("{{");
        }
        else
        {
            auto& value_obj = value.as_object();
            out("{{\"{}\", \"{}\"", value_obj.get("englishName").as_string(), value_obj.get("internalName").as_string());
        }
        if(count < items.size() - 1)
            outln("}},");
        else
            outln("}}");
        count++;
    });
    // @formatter:on

    outln("}};");

    outln("const int s_total_prefixes = {};", items.size());

    outln("}}");
}


void generate_tile_object_models(const JsonArray& items)
{
    outln("#include <LibTerraria/Model.h>");
    outln("namespace Terraria");
    outln("{{");
    outln("Terraria::Model::TileObject s_tile_objects[] = {{");

    int count = 0;

    // @formatter:off
    items.for_each([&](auto& value)
    {
        if(value.is_null())
        {
            out("{{");
        }
        else
        {
            auto& value_obj = value.as_object();
            auto& origin = value_obj.get("origin").as_object();
            out("{{{}, {}, {}, {{{}, {}}}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {{",
                value_obj.get("width").to_i32(),
                value_obj.get("height").to_i32(),
                value_obj.get("type").to_i32(),
                origin.get("X").to_i32(),
                origin.get("Y").to_i32(),
                value_obj.get("direction").to_i32(),
                value_obj.get("styleWrapLimit").to_i32(),
                value_obj.get("styleMultiplier").to_i32(),
                value_obj.get("randomStyleRange").to_i32(),
                value_obj.get("styleHorizontal").as_bool(),
                value_obj.get("styleLineSkip").to_i32(),
                value_obj.get("coordinateWidth").to_i32(),
                value_obj.get("coordinateFullWidth").to_i32(),
                value_obj.get("coordinateFullHeight").to_i32(),
                value_obj.get("coordinatePadding").to_i32(),
                value_obj.get("style").to_i32());

            auto coord_height_count = 0;
            auto coordinate_heights = value_obj.get("coordinateHeights").as_array();
            coordinate_heights.for_each([&](auto& value)
            {
                out("{}", value.to_i32());
                if(coord_height_count < coordinate_heights.size() - 1)
                    out(", ");

                coord_height_count++;
            });

            out("}}");
        };
        if(count < items.size() - 1)
            outln("}},");
        else
            outln("}}");
        count++;
    });
    // @formatter:on

    outln("}};");

    outln("const int s_total_tile_objects = {};", items.size());

    outln("}}");
}