/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <LibTerraria/Projectile.h>
#include <AK/String.h>
#include <LibTerraria/Color.h>
#include <LibTerraria/PlayerDeathReason.h>

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
};
}