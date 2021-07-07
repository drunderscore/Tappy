/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Scripting/Format.h>
#include <Server/Scripting/Lua.h>
#include <AK/Format.h>
#include <math.h>

static String nil_string = "nil";
static String true_string = "true";
static String false_string = "false";

namespace Scripting
{
String format(lua_State* state, int index)
{
    auto format_string = luaL_checkstring(state, index);
    outln("format str is {}", format_string);
    // lua_gettop(state) - index
    Vector<u64, 16> values;
    Vector<AK::TypeErasedParameter, 16> type_erased_parameters;

    auto top = lua_gettop(state);

    // FIXME: code duplication when doing strings nils and bools

    for (auto i = index + 1; i <= top; i++)
    {
        auto type = lua_type(state, i);
        AK::TypeErasedParameter param;

        auto append_string = [&values, &param](const char* value)
        {
            auto value_index = values.size();
            values.append(reinterpret_cast<u64>((value ? true_string : false_string).characters()));

            param.type = AK::TypeErasedParameter::Type::Custom;
            param.value = reinterpret_cast<void*>(&values.at(value_index));
            param.formatter = AK::__format_value<char*>;
        };

        switch (type)
        {
            case LUA_TNUMBER:
            {
                auto value = lua_tonumber(state, i);
                [[maybe_unused]] double whole_part;
                auto fractional_part = modf(value, &whole_part);
                auto value_index = values.size();
                param.type = AK::TypeErasedParameter::Type::Custom;

                if (fractional_part == 0)
                {
                    values.append(value);
                    param.value = reinterpret_cast<void*>(&values.at(value_index));
                    param.formatter = AK::__format_value<i64>;
                }
                else
                {
                    values.append(*reinterpret_cast<u64*>(&value));
                    param.value = reinterpret_cast<void*>(&values.at(value_index));
                    param.formatter = AK::__format_value<LUA_NUMBER>;
                }

                break;
            }
            case LUA_TBOOLEAN:
            {
                auto value = lua_toboolean(state, i);
                append_string((value ? true_string : false_string).characters());
                break;
            }
            case LUA_TNIL:
            {
                append_string(nil_string.characters());
                break;
            }
            default:
                dbgln("Unimplemented string formatter for Lua type {} (aka {})", type, lua_typename(state, type));
            case LUA_TSTRING:
            {
                auto value = lua_tostring(state, i);
                append_string(value);
                break;
            }
        }
        type_erased_parameters.append(move(param));
    }

    AK::TypeErasedFormatParams params;
    params.set_parameters(type_erased_parameters);

    return String::vformatted(format_string, params);
}
}