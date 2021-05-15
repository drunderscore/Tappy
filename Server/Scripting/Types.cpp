/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Scripting/Types.h>
#include <Server/Scripting/Lua.h>

namespace Scripting
{
Terraria::Projectile Types::projectile(lua_State* state, int index, bool takeId)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::Projectile proj;

    if (takeId)
    {
        lua_pushstring(state, "id");
        lua_gettable(state, index);
        proj.set_id(luaL_checkinteger(state, -1));
        lua_pop(state, 1);
    }

    lua_pushstring(state, "position");
    lua_gettable(state, index);
    proj.position() = point(state, -1);

    lua_pushstring(state, "velocity");
    lua_gettable(state, index);
    proj.velocity() = point(state, -1);

    lua_pushstring(state, "owner");
    lua_gettable(state, index);
    proj.set_owner(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "type");
    lua_gettable(state, index);
    proj.set_type(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "ai");
    lua_gettable(state, index);
    if (lua_type(state, -1) == LUA_TTABLE)
    {
        for (auto i = 0; i < proj.ai().size(); i++)
        {
            lua_rawgeti(state, -1, i + 1);
            if (lua_isnumber(state, -1))
                proj.ai()[i] = lua_tonumber(state, -1);
            lua_pop(state, 1);
        }
    }

    int has_value = false;
    int value;

    lua_pushstring(state, "bannerIdToRespondTo");
    lua_gettable(state, index);
    value = lua_tointegerx(state, -1, &has_value);
    if (has_value)
        proj.banner_id_to_respond_to() = value;
    lua_pop(state, 1);

    lua_pushstring(state, "damage");
    lua_gettable(state, index);
    value = lua_tointegerx(state, -1, &has_value);
    if (has_value)
        proj.damage() = value;
    lua_pop(state, 1);

    lua_pushstring(state, "knockback");
    lua_gettable(state, index);
    float knockback = lua_tonumberx(state, -1, &has_value);
    if (has_value)
        proj.knockback() = knockback;
    lua_pop(state, 1);

    lua_pushstring(state, "originalDamage");
    lua_gettable(state, index);
    value = lua_tointegerx(state, -1, &has_value);
    if (has_value)
        proj.original_damage() = value;
    lua_pop(state, 1);

    lua_pushstring(state, "uuid");
    lua_gettable(state, index);
    value = lua_tointegerx(state, -1, &has_value);
    if (has_value)
        proj.uuid() = value;
    lua_pop(state, 1);

    return proj;
}

void Types::point(lua_State* state, const Terraria::EntityPoint& point)
{
    lua_newtable(state);
    lua_pushstring(state, "x");
    lua_pushnumber(state, point.x);
    lua_settable(state, -3);
    lua_pushstring(state, "y");
    lua_pushnumber(state, point.y);
    lua_settable(state, -3);
}

Terraria::EntityPoint Types::point(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);
    lua_pushstring(state, "x");
    lua_gettable(state, index - 1);
    auto x = luaL_checknumber(state, -1);
    lua_pop(state, 1);
    lua_pushstring(state, "y");
    lua_gettable(state, index - 1);
    auto y = luaL_checknumber(state, -1);
    lua_pop(state, 1);
    return Terraria::EntityPoint{static_cast<float>(x), static_cast<float>(y)};
}

void Types::projectile(lua_State* state, const Terraria::Projectile& proj)
{
    lua_newtable(state);
    lua_pushstring(state, "id");
    lua_pushinteger(state, proj.id());
    lua_settable(state, -3);

    lua_pushstring(state, "position");
    point(state, proj.position());
    lua_settable(state, -3);

    lua_pushstring(state, "velocity");
    point(state, proj.velocity());
    lua_settable(state, -3);

    lua_pushstring(state, "owner");
    lua_pushinteger(state, proj.owner());
    lua_settable(state, -3);

    lua_pushstring(state, "type");
    lua_pushinteger(state, proj.type());
    lua_settable(state, -3);

    lua_pushstring(state, "ai");
    lua_newtable(state);
    for (auto i = 0; i < proj.ai().size(); i++)
    {
        auto& ai = proj.ai()[i];
        if (ai.has_value())
        {
            lua_pushnumber(state, *ai);
            lua_rawseti(state, -2, i + 1);
        }
    }
    lua_settable(state, -3);

    if (proj.banner_id_to_respond_to().has_value())
    {
        lua_pushstring(state, "bannerIdToRespondTo");
        lua_pushinteger(state, *proj.banner_id_to_respond_to());
        lua_settable(state, -3);
    }

    if (proj.damage().has_value())
    {
        lua_pushstring(state, "damage");
        lua_pushinteger(state, *proj.damage());
        lua_settable(state, -3);
    }

    if (proj.knockback().has_value())
    {
        lua_pushstring(state, "knockback");
        lua_pushnumber(state, *proj.knockback());
        lua_settable(state, -3);
    }

    if (proj.original_damage().has_value())
    {
        lua_pushstring(state, "originalDamage");
        lua_pushinteger(state, *proj.original_damage());
        lua_settable(state, -3);
    }

    if (proj.uuid().has_value())
    {
        lua_pushstring(state, "uuid");
        lua_pushinteger(state, *proj.uuid());
        lua_settable(state, -3);
    }
}
}