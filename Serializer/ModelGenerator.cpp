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

    outln("}}");
}