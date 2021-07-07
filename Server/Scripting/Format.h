/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/String.h>
#include <AK/StringView.h>

typedef struct lua_State lua_State;

namespace Scripting
{
String format(lua_State*, int index);

String format(lua_State*, int index, const StringView&);
}