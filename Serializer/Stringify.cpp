/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/String.h>

void stringify(const String& name, const StringView& lines)
{
    outln("const char* s_{}[] =", name);
    outln("{{");
    for (auto line : lines.split_view('\n'))
    {
        outln("\"{}\",", line);
    }
    outln("}};");
}