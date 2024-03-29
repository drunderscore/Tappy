/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/String.h>
#include <LibTerraria/Character.h>
#include <LibTerraria/Color.h>
#include <LibTerraria/DroppedItem.h>
#include <LibTerraria/NPC.h>
#include <LibTerraria/PlayerDeathReason.h>
#include <LibTerraria/Projectile.h>
#include <LibTerraria/TileModification.h>

typedef struct lua_State lua_State;

namespace Scripting
{
class Types
{
public:
    static void projectile(lua_State*, const Terraria::Projectile&);

    static Terraria::Projectile projectile(lua_State*, int index, bool takeId = true);

    static void point(lua_State*, const Terraria::EntityPoint&);

    static Terraria::EntityPoint point(lua_State*, int index);

    static void color(lua_State*, const Terraria::Color&);

    static Terraria::Color color(lua_State*, int index);

    static void player_death_reason(lua_State*, const Terraria::PlayerDeathReason&);

    static Terraria::PlayerDeathReason player_death_reason(lua_State*, int index);

    static void npc(lua_State*, const Terraria::NPC&);

    static Terraria::NPC npc(lua_State*, int index);

    static void item(lua_State*, const Terraria::Item&);

    static Terraria::Item item(lua_State*, int index);

    static void character(lua_State*, const Terraria::Character&);

    static Terraria::Character character(lua_State*, int index);

    static void tile_modification(lua_State*, const Terraria::TileModification&);

    static Terraria::TileModification tile_modification(lua_State*, int index);

    static void dropped_item(lua_State*, const Terraria::DroppedItem&);

    static Terraria::DroppedItem dropped_item(lua_State*, int index);
};
}