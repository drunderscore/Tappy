/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/RefCounted.h>
#include <AK/Weakable.h>
#include <AK/HashMap.h>
#include <AK/Badge.h>
#include <Server/Client.h>
#include <LibTerraria/PlayerInventory.h>
#include <LibTerraria/Item.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/TogglePvp.h>
#include <LibTerraria/Net/Packets/PlayerHurt.h>

typedef struct lua_State lua_State;

#define DEFINE_LUA_METHOD(name) static int name##_thunk(lua_State* state)   \
{                                                                           \
    return (*s_engines.get(state))->name();                                 \
}                                                                           \
int name();

class Server;

namespace Scripting
{
class Engine : public Weakable<Engine>
{
public:
    explicit Engine(Server&);

    ~Engine();

    void client_did_send_message(Badge<Server>, const Client&, const String&);

    void client_did_sync_projectile(Badge<Server>, const Client&, const Terraria::Net::Packets::SyncProjectile&);

    void client_did_connect_request(Badge<Server>, const Client&, const String& version);

    void client_did_toggle_pvp(Badge<Server>, const Client&, const Terraria::Net::Packets::TogglePvp&);

    void client_did_hurt_player(Badge<Server>, Client&, const Terraria::Net::Packets::PlayerHurt&);

private:
    static HashMap<lua_State*, Engine*> s_engines;
    lua_State* m_state;
    Server& m_server;

    void* client_userdata(u8 id) const;

    void* player_userdata(u8 id) const;

    void* character_userdata(u8 id) const;

    DEFINE_LUA_METHOD(at_panic);

    // Game
    DEFINE_LUA_METHOD(client);

    DEFINE_LUA_METHOD(clients);

    DEFINE_LUA_METHOD(add_projectile);

    DEFINE_LUA_METHOD(projectile_exists);

    // Client
    DEFINE_LUA_METHOD(client_id);

    DEFINE_LUA_METHOD(client_is_connected);

    DEFINE_LUA_METHOD(client_send_message);

    DEFINE_LUA_METHOD(client_player);

    DEFINE_LUA_METHOD(client_address);

    DEFINE_LUA_METHOD(client_disconnect);

    DEFINE_LUA_METHOD(client_sync_projectile);

    DEFINE_LUA_METHOD(client_kill_projectile);

    DEFINE_LUA_METHOD(client_set_pvp);

    // Player
    DEFINE_LUA_METHOD(player_character);

    DEFINE_LUA_METHOD(player_buffs);

    DEFINE_LUA_METHOD(player_hp);

    DEFINE_LUA_METHOD(player_max_hp);

    // Character
    DEFINE_LUA_METHOD(character_name);
};
}

#undef DEFINE_LUA_METHOD