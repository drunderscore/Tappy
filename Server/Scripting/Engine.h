/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Badge.h>
#include <AK/HashMap.h>
#include <AK/RefCounted.h>
#include <AK/Weakable.h>
#include <LibCore/Timer.h>
#include <LibTerraria/Item.h>
#include <LibTerraria/Net/Packets/DamageNPC.h>
#include <LibTerraria/Net/Packets/ModifyTile.h>
#include <LibTerraria/Net/Packets/PlayerDeath.h>
#include <LibTerraria/Net/Packets/PlayerHurt.h>
#include <LibTerraria/Net/Packets/PlayerTeam.h>
#include <LibTerraria/Net/Packets/SpawnPlayer.h>
#include <LibTerraria/Net/Packets/SyncItem.h>
#include <LibTerraria/Net/Packets/SyncItemOwner.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/TogglePvp.h>
#include <LibTerraria/PlayerInventory.h>
#include <Server/Client.h>

typedef struct lua_State lua_State;

#define DEFINE_LUA_METHOD(name)                                                                                        \
    static int name##_thunk(lua_State* state) { return (*s_engines.get(state))->name(); }                              \
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

    void client_did_player_death(Badge<Server>, Client&, const Terraria::Net::Packets::PlayerDeath&);

    void client_did_damage_npc(Badge<Server>, Client&, const Terraria::Net::Packets::DamageNPC&);

    void client_did_finish_connecting(Badge<Server>, Client&);

    void client_did_spawn_player(Badge<Server>, Client&, const Terraria::Net::Packets::SpawnPlayer&);

    void client_did_modify_tile(Badge<Server>, Client&, const Terraria::Net::Packets::ModifyTile&);

    void client_did_disconnect(Badge<Server>, Client&, Client::DisconnectReason);

    void client_did_sync_player_team(Badge<Server>, Client&, const Terraria::Net::Packets::PlayerTeam&);

    void client_did_sync_item(Badge<Server>, Client&, const Terraria::Net::Packets::SyncItem&);

    void client_did_sync_item_owner(Badge<Server>, Client&, const Terraria::Net::Packets::SyncItemOwner&);

private:
    static HashMap<lua_State*, Engine*> s_engines;
    lua_State* m_state;
    Server& m_server;
    HashMap<i32, NonnullRefPtr<Core::Timer>> m_timers;
    int m_base_ref{};

    void* client_userdata(u8 id) const;

    void* player_userdata(u8 id) const;

    void* timer_userdata(i32 id) const;

    ALWAYS_INLINE void push_base_table() const;

    JsonObject serialize_object_to_json(int index);

    void deserialize_object_to_lua_object(const JsonObject&);

    DEFINE_LUA_METHOD(at_panic);

    DEFINE_LUA_METHOD(format);

    // Json
    DEFINE_LUA_METHOD(json_serialize);

    DEFINE_LUA_METHOD(json_deserialize);

    // Timer
    DEFINE_LUA_METHOD(timer_create);

    DEFINE_LUA_METHOD(timer_destroy);

    DEFINE_LUA_METHOD(timer_invoke);

    // Game
    DEFINE_LUA_METHOD(game_client);

    DEFINE_LUA_METHOD(game_clients);

    DEFINE_LUA_METHOD(game_add_projectile);

    DEFINE_LUA_METHOD(game_add_dropped_item);

    DEFINE_LUA_METHOD(game_remove_dropped_item);

    DEFINE_LUA_METHOD(game_set_item_owner);

    DEFINE_LUA_METHOD(game_next_available_dropped_item_id);

    // Client
    DEFINE_LUA_METHOD(client_id);

    DEFINE_LUA_METHOD(client_is_connected);

    DEFINE_LUA_METHOD(client_send_message);

    DEFINE_LUA_METHOD(client_player);

    DEFINE_LUA_METHOD(client_address);

    DEFINE_LUA_METHOD(client_disconnect);

    DEFINE_LUA_METHOD(client_sync_projectile);

    DEFINE_LUA_METHOD(client_kill_projectile);

    DEFINE_LUA_METHOD(client_equals);

    DEFINE_LUA_METHOD(client_sync_npc);

    DEFINE_LUA_METHOD(client_sync_tile_rect);

    DEFINE_LUA_METHOD(client_modify_tile);

    DEFINE_LUA_METHOD(client_uuid);

    // Player
    DEFINE_LUA_METHOD(player_character);

    DEFINE_LUA_METHOD(player_update_character);

    DEFINE_LUA_METHOD(player_buffs);

    DEFINE_LUA_METHOD(player_hp);

    DEFINE_LUA_METHOD(player_max_hp);

    DEFINE_LUA_METHOD(player_set_pvp);

    DEFINE_LUA_METHOD(player_position);

    DEFINE_LUA_METHOD(player_teleport);

    DEFINE_LUA_METHOD(player_set_team);

    DEFINE_LUA_METHOD(player_set_health);

    DEFINE_LUA_METHOD(player_set_mana);

    DEFINE_LUA_METHOD(player_set_item_in_slot);

    DEFINE_LUA_METHOD(player_item_in_slot);

    class UsingBaseTable
    {
        friend Engine;

        UsingBaseTable(const Engine& engine) : m_engine(engine) { engine.push_base_table(); }

        ~UsingBaseTable();

        const Engine& m_engine;
    };
};
}

#undef DEFINE_LUA_METHOD