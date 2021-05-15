/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <LibTerraria/Projectile.h>
#include <AK/String.h>

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
};
}