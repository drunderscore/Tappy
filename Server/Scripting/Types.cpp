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
    proj.position() = point(state, lua_gettop(state));

    lua_pushstring(state, "velocity");
    lua_gettable(state, index);
    proj.velocity() = point(state, lua_gettop(state));

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
    lua_createtable(state, 2, 0);
    lua_pushstring(state, "x");
    lua_pushnumber(state, point.x());
    lua_settable(state, -3);
    lua_pushstring(state, "y");
    lua_pushnumber(state, point.y());
    lua_settable(state, -3);
}

Terraria::EntityPoint Types::point(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::EntityPoint point;

    lua_pushstring(state, "x");
    lua_gettable(state, index);
    point.set_x(luaL_checknumber(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "y");
    lua_gettable(state, index);
    point.set_y(luaL_checknumber(state, -1));
    lua_pop(state, 1);

    return point;
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

void Types::color(lua_State* state, const Terraria::Color& color)
{
    lua_createtable(state, 3, 0);
    lua_pushstring(state, "r");
    lua_pushinteger(state, color.r);
    lua_settable(state, -3);

    lua_pushstring(state, "g");
    lua_pushinteger(state, color.g);
    lua_settable(state, -3);

    lua_pushstring(state, "b");
    lua_pushinteger(state, color.b);
    lua_settable(state, -3);
}

Terraria::Color Types::color(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::Color color;

    lua_pushstring(state, "r");
    lua_gettable(state, index);
    color.r = luaL_checkinteger(state, -1);
    lua_pop(state, 1);

    lua_pushstring(state, "g");
    lua_gettable(state, index);
    color.g = luaL_checkinteger(state, -1);
    lua_pop(state, 1);

    lua_pushstring(state, "b");
    lua_gettable(state, index);
    color.b = luaL_checkinteger(state, -1);
    lua_pop(state, 1);

    return color;
}

void Types::player_death_reason(lua_State* state, const Terraria::PlayerDeathReason& reason)
{
    lua_newtable(state);

    if (reason.player().has_value())
    {
        lua_pushstring(state, "player");
        lua_pushinteger(state, *reason.player());
        lua_settable(state, -3);
    }
    if (reason.npc().has_value())
    {
        lua_pushstring(state, "npc");
        lua_pushinteger(state, *reason.npc());
        lua_settable(state, -3);
    }
    if (reason.projectile().has_value())
    {
        lua_pushstring(state, "projectile");
        lua_pushinteger(state, *reason.projectile());
        lua_settable(state, -3);
    }
    if (reason.other().has_value())
    {
        lua_pushstring(state, "other");
        lua_pushinteger(state, *reason.other());
        lua_settable(state, -3);
    }
    if (reason.custom_reason().has_value())
    {
        lua_pushstring(state, "customReason");
        lua_pushstring(state, (*reason.custom_reason()).characters());
        lua_settable(state, -3);
    }
    if (reason.projectile_type().has_value())
    {
        lua_pushstring(state, "projectileType");
        lua_pushinteger(state, *reason.projectile_type());
        lua_settable(state, -3);
    }
    if (reason.item_id().has_value())
    {
        lua_pushstring(state, "itemId");
        lua_pushinteger(state, static_cast<i16>(*reason.item_id()));
        lua_settable(state, -3);
    }
    if (reason.item_prefix().has_value())
    {
        lua_pushstring(state, "itemPrefix");
        lua_pushinteger(state, static_cast<i8>(*reason.item_prefix()));
        lua_settable(state, -3);
    }
}

Terraria::PlayerDeathReason Types::player_death_reason(lua_State*, int index)
{

}

void Types::npc(lua_State* state, const Terraria::NPC& npc)
{
    lua_newtable(state);

    lua_pushstring(state, "id");
    lua_pushinteger(state, npc.id());
    lua_settable(state, -3);

    lua_pushstring(state, "position");
    point(state, npc.position());
    lua_settable(state, -3);

    lua_pushstring(state, "velocity");
    point(state, npc.velocity());
    lua_settable(state, -3);

    lua_pushstring(state, "target");
    lua_pushinteger(state, npc.target());
    lua_settable(state, -3);

    lua_pushstring(state, "hp");
    lua_pushinteger(state, npc.hp());
    lua_settable(state, -3);

    lua_pushstring(state, "type");
    lua_pushinteger(state, npc.type());
    lua_settable(state, -3);

    lua_pushstring(state, "direction");
    lua_pushboolean(state, npc.direction());
    lua_settable(state, -3);

    lua_pushstring(state, "directionY");
    lua_pushboolean(state, npc.direction_y());
    lua_settable(state, -3);

    lua_pushstring(state, "ai");
    lua_newtable(state);
    for (auto i = 0; i < npc.ai().size(); i++)
    {
        auto& ai = npc.ai()[i];
        if (ai.has_value())
        {
            lua_pushnumber(state, *ai);
            lua_rawseti(state, -2, i + 1);
        }
    }
    lua_settable(state, -3);

    lua_pushstring(state, "spriteDirection");
    lua_pushboolean(state, npc.sprite_direction());
    lua_settable(state, -3);

    if (npc.scaled_for_this_many_players().has_value())
    {
        lua_pushstring(state, "scaledForThisManyPlayers");
        lua_pushinteger(state, *npc.scaled_for_this_many_players());
        lua_settable(state, -3);
    }

    lua_pushstring(state, "spawnedFromStatue");
    lua_pushboolean(state, npc.spawned_from_statue());
    lua_settable(state, -3);

    if (npc.strength_multiplier().has_value())
    {
        lua_pushstring(state, "strengthMultiplier");
        lua_pushnumber(state, *npc.strength_multiplier());
        lua_settable(state, -3);
    }

    if (npc.release_owner().has_value())
    {
        lua_pushstring(state, "releaseOwner");
        lua_pushinteger(state, *npc.release_owner());
        lua_settable(state, -3);
    }
}

Terraria::NPC Types::npc(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::NPC npc;

    lua_pushstring(state, "id");
    lua_gettable(state, index);
    npc.set_id(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "position");
    lua_gettable(state, index);
    npc.position() = point(state, lua_gettop(state));
    lua_pop(state, 1);

    lua_pushstring(state, "velocity");
    lua_gettable(state, index);
    npc.velocity() = point(state, lua_gettop(state));
    lua_pop(state, 1);

    lua_pushstring(state, "target");
    lua_gettable(state, index);
    npc.set_target(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "hp");
    lua_gettable(state, index);
    npc.set_hp(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "type");
    lua_gettable(state, index);
    npc.set_type(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "direction");
    lua_gettable(state, index);
    npc.set_direction(lua_toboolean(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "directionY");
    lua_gettable(state, index);
    npc.set_direction_y(lua_toboolean(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "ai");
    lua_gettable(state, index);
    if (lua_type(state, -1) == LUA_TTABLE)
    {
        for (auto i = 0; i < npc.ai().size(); i++)
        {
            lua_rawgeti(state, -1, i + 1);
            if (lua_isnumber(state, -1))
                npc.ai()[i] = lua_tonumber(state, -1);
            lua_pop(state, 1);
        }
    }

    lua_pushstring(state, "spriteDirection");
    lua_gettable(state, index);
    npc.set_sprite_direction(lua_toboolean(state, -1));
    lua_pop(state, 1);

    int has_value;
    int value;

    lua_pushstring(state, "scaledForThisManyPlayers");
    lua_gettable(state, index);
    value = lua_tointegerx(state, -1, &has_value);
    if (has_value)
        npc.scaled_for_this_many_players() = value;
    lua_pop(state, 1);

    lua_pushstring(state, "spawnedFromStatue");
    lua_gettable(state, index);
    npc.set_spawned_from_statue(lua_toboolean(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "strengthMultiplier");
    lua_gettable(state, index);
    float strengthMultiplier = lua_tonumberx(state, -1, &has_value);
    if (has_value)
        npc.strength_multiplier() = strengthMultiplier;
    lua_pop(state, 1);

    lua_pushstring(state, "releaseOwner");
    lua_gettable(state, index);
    value = lua_tointegerx(state, -1, &has_value);
    if (has_value)
        npc.release_owner() = value;
    lua_pop(state, 1);

    return npc;
}

void Types::item(lua_State* state, const Terraria::Item& item)
{
    lua_createtable(state, 3, 0);

    lua_pushstring(state, "id");
    lua_pushinteger(state, static_cast<i16>(item.id()));
    lua_settable(state, -3);

    lua_pushstring(state, "prefix");
    lua_pushinteger(state, static_cast<i8>(item.prefix()));
    lua_settable(state, -3);

    lua_pushstring(state, "stack");
    lua_pushinteger(state, item.stack());
    lua_settable(state, -3);
}

Terraria::Item Types::item(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::Item item;

    lua_pushstring(state, "id");
    lua_gettable(state, index);
    item.set_id(static_cast<Terraria::Item::Id>(luaL_checkinteger(state, -1)));
    lua_pop(state, 1);

    lua_pushstring(state, "stack");
    lua_gettable(state, index);
    item.set_stack(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    int value;
    int has_value;

    lua_pushstring(state, "prefix");
    lua_gettable(state, index);
    value = lua_tointegerx(state, -1, &has_value);
    if (has_value)
        item.set_prefix(static_cast<Terraria::Item::Prefix>(value));
    lua_pop(state, 1);

    return item;
}

void Types::character(lua_State* state, const Terraria::Character& character)
{
    lua_newtable(state);

    lua_pushstring(state, "skinVariant");
    lua_pushinteger(state, character.skin_variant());
    lua_settable(state, -3);

    lua_pushstring(state, "hair");
    lua_pushinteger(state, character.hair());
    lua_settable(state, -3);

    lua_pushstring(state, "name");
    lua_pushstring(state, character.name().characters());
    lua_settable(state, -3);

    lua_pushstring(state, "hairDye");
    lua_pushinteger(state, character.hair_dye());
    lua_settable(state, -3);

    lua_pushstring(state, "hideVisuals");
    lua_pushinteger(state, character.hide_visuals());
    lua_settable(state, -3);

    lua_pushstring(state, "hideVisuals2");
    lua_pushinteger(state, character.hide_visuals2());
    lua_settable(state, -3);

    lua_pushstring(state, "hideMisc");
    lua_pushinteger(state, character.hide_misc());
    lua_settable(state, -3);

    lua_pushstring(state, "hairColor");
    color(state, character.hair_color());
    lua_settable(state, -3);

    lua_pushstring(state, "skinColor");
    color(state, character.skin_color());
    lua_settable(state, -3);

    lua_pushstring(state, "eyeColor");
    color(state, character.eye_color());
    lua_settable(state, -3);

    lua_pushstring(state, "shirtColor");
    color(state, character.shirt_color());
    lua_settable(state, -3);

    lua_pushstring(state, "undershirtColor");
    color(state, character.undershirt_color());
    lua_settable(state, -3);

    lua_pushstring(state, "pantsColor");
    color(state, character.pants_color());
    lua_settable(state, -3);

    lua_pushstring(state, "shoeColor");
    color(state, character.shoe_color());
    lua_settable(state, -3);

    lua_pushstring(state, "difficultyFlags");
    lua_pushinteger(state, character.difficulty_flags());
    lua_settable(state, -3);

    lua_pushstring(state, "torchFlags");
    lua_pushinteger(state, character.torch_flags());
    lua_settable(state, -3);
}

Terraria::Character Types::character(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::Character character;

    lua_pushstring(state, "skinVariant");
    lua_gettable(state, index);
    character.set_skin_variant(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "hair");
    lua_gettable(state, index);
    character.set_hair(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "name");
    lua_gettable(state, index);
    character.set_name(luaL_checkstring(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "hairDye");
    lua_gettable(state, index);
    character.set_hair_dye(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "hideVisuals");
    lua_gettable(state, index);
    character.set_hide_visuals(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "hideVisuals2");
    lua_gettable(state, index);
    character.set_hide_visuals2(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "hideMisc");
    lua_gettable(state, index);
    character.set_hide_misc(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "hairColor");
    lua_gettable(state, index);
    character.set_hair_color(color(state, lua_gettop(state)));
    lua_pop(state, 1);

    lua_pushstring(state, "skinColor");
    lua_gettable(state, index);
    character.set_skin_color(color(state, lua_gettop(state)));
    lua_pop(state, 1);

    lua_pushstring(state, "eyeColor");
    lua_gettable(state, index);
    character.set_eye_color(color(state, lua_gettop(state)));
    lua_pop(state, 1);

    lua_pushstring(state, "shirtColor");
    lua_gettable(state, index);
    character.set_shirt_color(color(state, lua_gettop(state)));
    lua_pop(state, 1);

    lua_pushstring(state, "undershirtColor");
    lua_gettable(state, index);
    character.set_undershirt_color(color(state, lua_gettop(state)));
    lua_pop(state, 1);

    lua_pushstring(state, "pantsColor");
    lua_gettable(state, index);
    character.set_pants_color(color(state, lua_gettop(state)));
    lua_pop(state, 1);

    lua_pushstring(state, "shoeColor");
    lua_gettable(state, index);
    character.set_shoe_color(color(state, lua_gettop(state)));
    lua_pop(state, 1);

    lua_pushstring(state, "difficultyFlags");
    lua_gettable(state, index);
    character.set_difficulty_flags(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "torchFlags");
    lua_gettable(state, index);
    character.set_torch_flags(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    return character;
}

void Types::tile_modification(lua_State* state, const Terraria::TileModification& modification)
{
    lua_createtable(state, 5, 0);

    lua_pushstring(state, "action");
    lua_pushinteger(state, modification.action);
    lua_settable(state, -3);

    // FIXME: Shouldn't this be a tile position Lua type?
    lua_pushstring(state, "x");
    lua_pushinteger(state, modification.position.x());
    lua_settable(state, -3);

    lua_pushstring(state, "y");
    lua_pushinteger(state, modification.position.y());
    lua_settable(state, -3);

    lua_pushstring(state, "flags1");
    lua_pushinteger(state, modification.flags_1);
    lua_settable(state, -3);

    lua_pushstring(state, "flags2");
    lua_pushinteger(state, modification.flags_2);
    lua_settable(state, -3);
}

Terraria::TileModification Types::tile_modification(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::TileModification modification;

    lua_pushstring(state, "action");
    lua_gettable(state, index);
    modification.action = luaL_checkinteger(state, -1);
    lua_pop(state, 1);

    lua_pushstring(state, "x");
    lua_gettable(state, index);
    modification.position.set_x(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "y");
    lua_gettable(state, index);
    modification.position.set_y(luaL_checkinteger(state, -1));
    lua_pop(state, 1);

    lua_pushstring(state, "flags1");
    lua_gettable(state, index);
    modification.flags_1 = luaL_checkinteger(state, -1);
    lua_pop(state, 1);

    lua_pushstring(state, "flags2");
    lua_gettable(state, index);
    modification.flags_2 = luaL_checkinteger(state, -1);
    lua_pop(state, 1);

    return modification;
}

void Types::dropped_item(lua_State* state, const Terraria::DroppedItem& value)
{
    lua_createtable(state, 5, 0);

    lua_pushstring(state, "item");
    item(state, value.item());
    lua_settable(state, -3);

    lua_pushstring(state, "position");
    point(state, value.position());
    lua_settable(state, -3);

    lua_pushstring(state, "velocity");
    point(state, value.velocity());
    lua_settable(state, -3);

    if (value.owner().has_value())
    {
        lua_pushstring(state, "owner");
        lua_pushinteger(state, *value.owner());
        lua_settable(state, -3);
    }
}

Terraria::DroppedItem Types::dropped_item(lua_State* state, int index)
{
    luaL_checktype(state, index, LUA_TTABLE);

    Terraria::DroppedItem dropped_item;

    lua_pushstring(state, "item");
    lua_gettable(state, index);
    dropped_item.item() = item(state, lua_gettop(state));
    lua_pop(state, 1);

    lua_pushstring(state, "position");
    lua_gettable(state, index);
    dropped_item.position() = point(state, lua_gettop(state));
    lua_pop(state, 1);

    lua_pushstring(state, "velocity");
    lua_gettable(state, index);
    dropped_item.velocity() = point(state, lua_gettop(state));
    lua_pop(state, 1);

    LUA_NUMBER value;
    int has_value;

    lua_pushstring(state, "owner");
    lua_gettable(state, index);
    value = lua_tonumberx(state, -1, &has_value);
    if (has_value)
        dropped_item.owner() = value;
    lua_pop(state, 1);

    return dropped_item;
}
}