/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/Assertions.h>
#include <Server/Scripting/Engine.h>
#include <Server/Server.h>
#include <LibTerraria/Net/Packets/Modules/Text.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/KillProjectile.h>
#include <Server/Scripting/Types.h>
#include <Server/Scripting/Lua.h>

#define LOAD_TEST_SCRIPT 1

namespace Scripting
{
HashMap<lua_State*, Engine*> Engine::s_engines;

Engine::Engine(Server& server) :
        m_server(server)
{
    m_state = luaL_newstate();
            VERIFY(m_state);
    s_engines.set(m_state, this);
    lua_atpanic(m_state, at_panic_thunk);
    luaL_openlibs(m_state);

    static const struct luaL_Reg game_lib[] =
            {
                    {"client",           client_thunk},
                    {"clients",          clients_thunk},
                    {"addProjectile",    add_projectile_thunk},
                    {"projectileExists", projectile_exists_thunk},
                    {}
            };

    static const struct luaL_Reg client_lib[] =
            {
                    {"id",             client_id_thunk},
                    {"isConnected",    client_is_connected_thunk},
                    {"sendMessage",    client_send_message_thunk},
                    {"disconnect",     client_disconnect_thunk},
                    {"player",         client_player_thunk},
                    {"address",        client_address_thunk},
                    {"syncProjectile", client_sync_projectile_thunk},
                    {"killProjectile", client_kill_projectile_thunk},
                    {}
            };

    static const struct luaL_Reg player_lib[] =
            {
                    {"character", player_character_thunk},
                    {"buffs",     player_buffs_thunk},
                    {}
            };

    static const struct luaL_Reg character_lib[] =
            {
                    {"name", character_name_thunk},
                    {}
            };

    luaL_newmetatable(m_state, "Server::Client");
    lua_pushstring(m_state, "__index");
    lua_pushvalue(m_state, -2);
    lua_settable(m_state, -3);

    luaL_setfuncs(m_state, client_lib, 0);
    lua_pop(m_state, 1);

    luaL_newmetatable(m_state, "Terraria::Player");
    lua_pushstring(m_state, "__index");
    lua_pushvalue(m_state, -2);
    lua_settable(m_state, -3);

    luaL_setfuncs(m_state, player_lib, 0);
    lua_pop(m_state, 1);

    luaL_newmetatable(m_state, "Terraria::Character");
    lua_pushstring(m_state, "__index");
    lua_pushvalue(m_state, -2);
    lua_settable(m_state, -3);

    luaL_setfuncs(m_state, character_lib, 0);
    lua_pop(m_state, 1);

    luaL_newlib(m_state, game_lib);
    lua_setglobal(m_state, "Game");

    auto errored = luaL_dofile(m_state, "Base/Base.lua");
    if (errored)
    {
        warnln("Engine failed startup: {}", lua_tostring(m_state, -1));
        VERIFY_NOT_REACHED();
    }

    if constexpr (LOAD_TEST_SCRIPT)
    {
        errored = luaL_dofile(m_state, "Base/Test.lua");
        if (errored)
        {
            warnln("Engine failed test script: {}", lua_tostring(m_state, -1));
            VERIFY_NOT_REACHED();
        }
    }
}

Engine::~Engine()
{
    s_engines.remove(m_state);
    lua_close(m_state);
}

int Engine::at_panic()
{
    warnln("====LUA PANIC!====");
    auto error = lua_tostring(m_state, -1);
    warnln("Why: {}", error);
    luaL_traceback(m_state, m_state, nullptr, 1);
    auto traceback = lua_tostring(m_state, -1);
    warnln("{}", traceback);
    return 0;
}

void Engine::client_did_send_message(Badge<Server>, const Client& who, const String& message)
{
    lua_getfield(m_state, 1, "onClientChat");
    client_userdata(who.id());
    lua_pushstring(m_state, message.characters());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_sync_projectile(Badge<Server>, const Client& who,
                                        const Terraria::Net::Packets::SyncProjectile& proj_sync)
{
    lua_getfield(m_state, 1, "onClientSyncProjectile");
    client_userdata(who.id());
    Types::projectile(m_state, proj_sync.projectile());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_connect_request(Badge<Server>, const Client& client, const String& version)
{
    lua_getfield(m_state, 1, "onConnectRequest");
    client_userdata(client.id());
    lua_pushstring(m_state, version.characters());
    lua_call(m_state, 2, 0);
}

void* Engine::client_userdata(u8 id) const
{
    auto* client_ud = lua_newuserdata(m_state, sizeof(id));
    memcpy(client_ud, &id, sizeof(id));
    luaL_getmetatable(m_state, "Server::Client");
    lua_setmetatable(m_state, -2);
    return client_ud;
}

void* Engine::character_userdata(u8 id) const
{
    auto* client_ud = lua_newuserdata(m_state, sizeof(id));
    memcpy(client_ud, &id, sizeof(id));
    luaL_getmetatable(m_state, "Terraria::Character");
    lua_setmetatable(m_state, -2);
    return client_ud;
}

void* Engine::player_userdata(u8 id) const
{
    auto* player_ud = lua_newuserdata(m_state, sizeof(id));
    memcpy(player_ud, &id, sizeof(id));
    luaL_getmetatable(m_state, "Terraria::Player");
    lua_setmetatable(m_state, -2);
    return player_ud;
}

int Engine::client()
{
    auto id = luaL_checkinteger(m_state, 1);
    auto client = m_server.client(id);
    if (!client)
    {
        lua_pushnil(m_state);
        return 1;
    }

    client_userdata(client->id());

    return 1;
}

int Engine::clients()
{
    auto clients = m_server.clients();
    lua_newtable(m_state);
    for (auto& c : clients)
    {
        client_userdata(c->id());
        lua_rawseti(m_state, 1, c->id() + 1);
    }

    return 1;
}

int Engine::add_projectile()
{
    int is_integer = false;
    auto id = lua_tointegerx(m_state, 2, &is_integer);

    if (is_integer != 1)
    {
        for (auto i = 0; i < NumericLimits<i16>::max(); i++)
        {
            if (!m_server.projectiles().contains(i))
            {
                id = i;
                break;
            }
        }
    }

    Terraria::Projectile proj = Types::projectile(m_state, 1, false);
    proj.set_id(id);

    auto result = m_server.projectiles().set(id, proj);

    lua_pushinteger(m_state, id);
    lua_pushboolean(m_state, result == AK::HashSetResult::InsertedNewEntry);

    return 2;
}

int Engine::projectile_exists()
{
    lua_pushboolean(m_state, m_server.projectiles().contains(luaL_checkinteger(m_state, 1)));
    return 1;
}

int Engine::client_id()
{
    lua_pushinteger(m_state, *reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    return 1;
}

int Engine::client_is_connected()
{
    lua_pushboolean(m_state,
                    !m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client"))).is_null());

    return 1;
}

int Engine::client_send_message()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    auto text = luaL_checkstring(m_state, 2);
    auto author = luaL_optinteger(m_state, 3, 255);

    Terraria::Net::Packets::Modules::Text text_packet;
    text_packet.set_text(text);
    text_packet.set_author(author);
    // TODO: Color parameter
    text_packet.set_color({255, 255, 255});

    client->send(text_packet);

    return 0;
}

int Engine::client_player()
{
    player_userdata(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    return 1;
}

int Engine::client_address()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        lua_pushnil(m_state);
    else
        lua_pushstring(m_state, client->address().to_string().characters());

    return 1;
}

int Engine::client_disconnect()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    auto reason = luaL_checkstring(m_state, 2);

    // TODO: NetworkText support
    client->disconnect(reason);

    return 0;
}

int Engine::client_sync_projectile()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    auto id = luaL_checkinteger(m_state, 2);
    auto proj = m_server.projectiles().get(id);
    if (!proj.has_value())
        return 0;

    Terraria::Net::Packets::SyncProjectile sync_proj;
    sync_proj.projectile() = *proj;

    client->send(sync_proj);

    return 0;
}

int Engine::client_kill_projectile()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    Terraria::Net::Packets::KillProjectile kill_proj;
    kill_proj.set_projectile_id(luaL_checkinteger(m_state, 2));
    kill_proj.set_owner(luaL_checkinteger(m_state, 3));

    client->send(kill_proj);
    return 0;
}

int Engine::player_character()
{
    character_userdata(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    return 1;
}

int Engine::player_buffs()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        lua_pushnil(m_state);
    else
    {
        auto& buffs = client->player().buffs();
        lua_newtable(m_state);
        for (auto i = 0; i < buffs.size(); i++)
        {
            auto id = buffs[i];
            if (id == 0)
                continue;
            lua_pushinteger(m_state, id);
            lua_rawseti(m_state, 2, i + 1);
        }
    }

    return 1;
}

int Engine::character_name()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Character")));
    if (!client)
        lua_pushnil(m_state);
    else
        lua_pushstring(m_state, client->player().character().name().characters());

    return 1;
}

}