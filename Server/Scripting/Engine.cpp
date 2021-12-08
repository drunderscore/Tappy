/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/Assertions.h>
#include <AK/JsonObject.h>
#include <AK/LexicalPath.h>
#include <LibCore/DirIterator.h>
#include <LibCore/File.h>
#include <LibTerraria/Net/Packets/KillProjectile.h>
#include <LibTerraria/Net/Packets/Modules/Text.h>
#include <LibTerraria/Net/Packets/SyncNPC.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/SyncTileRect.h>
#include <LibTerraria/Net/Packets/TeleportEntity.h>
#include <Server/Scripting/Engine.h>
#include <Server/Scripting/Format.h>
#include <Server/Scripting/Lua.h>
#include <Server/Scripting/Types.h>
#include <Server/Server.h>

namespace Scripting
{
HashMap<lua_State*, Engine*> Engine::s_engines;

Engine::Engine(Server& server) : m_server(server)
{
    m_state = luaL_newstate();
    VERIFY(m_state);
    s_engines.set(m_state, this);
    lua_atpanic(m_state, at_panic_thunk);
    luaL_openlibs(m_state);

    static const struct luaL_Reg game_lib[] = {
        {"client", game_client_thunk},
        {"clients", game_clients_thunk},
        {"addProjectile", game_add_projectile_thunk},
        {"addDroppedItem", game_add_dropped_item_thunk},
        {"removeDroppedItem", game_remove_dropped_item_thunk},
        {"setItemOwner", game_set_item_owner_thunk},
        {"nextAvailableDroppedItemId", game_next_available_dropped_item_id_thunk},
        {}};

    static const struct luaL_Reg timer_lib[] = {
        {"create", timer_create_thunk}, {"destroy", timer_destroy_thunk}, {"invoke", timer_invoke_thunk}, {}};

    static const struct luaL_Reg json_lib[] = {
        {"serialize", json_serialize_thunk}, {"deserialize", json_deserialize_thunk}, {}};

    static const struct luaL_Reg client_lib[] = {{"id", client_id_thunk},
                                                 {"isConnected", client_is_connected_thunk},
                                                 {"sendMessage", client_send_message_thunk},
                                                 {"disconnect", client_disconnect_thunk},
                                                 {"player", client_player_thunk},
                                                 {"address", client_address_thunk},
                                                 {"syncProjectile", client_sync_projectile_thunk},
                                                 {"killProjectile", client_kill_projectile_thunk},
                                                 {"__eq", client_equals_thunk},
                                                 {"syncNpc", client_sync_npc_thunk},
                                                 {"syncTileRect", client_sync_tile_rect_thunk},
                                                 {"modifyTile", client_modify_tile_thunk},
                                                 {"uuid", client_uuid_thunk},
                                                 {}};

    static const struct luaL_Reg player_lib[] = {{"character", player_character_thunk},
                                                 {"buffs", player_buffs_thunk},
                                                 {"setPvp", player_set_pvp_thunk},
                                                 {"buffs", player_buffs_thunk},
                                                 {"position", player_position_thunk},
                                                 {"character", player_character_thunk},
                                                 {"updateCharacter", player_update_character_thunk},
                                                 {"teleport", player_teleport_thunk},
                                                 {"setTeam", player_set_team_thunk},
                                                 {"setHealth", player_set_health_thunk},
                                                 {"setMana", player_set_mana_thunk},
                                                 {"setItemInSlot", player_set_item_in_slot_thunk},
                                                 {"itemInSlot", player_item_in_slot_thunk},
                                                 {}};

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

    luaL_newmetatable(m_state, "Engine::Timer");
    lua_pushstring(m_state, "__index");
    lua_pushvalue(m_state, -2);
    lua_settable(m_state, -3);

    luaL_setfuncs(m_state, timer_lib, 0);
    lua_pop(m_state, 1);

    luaL_newlib(m_state, game_lib);
    lua_setglobal(m_state, "Game");

    luaL_newlib(m_state, timer_lib);
    lua_setglobal(m_state, "Timer");

    luaL_newlib(m_state, json_lib);
    lua_setglobal(m_state, "JSON");

    lua_pushcfunction(m_state, format_thunk);
    lua_setglobal(m_state, "format");

    auto errored = luaL_dofile(m_state, "Base/Base.lua");
    if (errored)
    {
        warnln("Engine failed startup: {}", lua_tostring(m_state, -1));
        VERIFY_NOT_REACHED();
    }

    m_base_ref = luaL_ref(m_state, LUA_REGISTRYINDEX);

    constexpr StringView plugins_directory = "plugins";

    if (Core::File::exists(plugins_directory) && Core::File::is_directory(plugins_directory))
    {
        auto plugins_dir_iterator = Core::DirIterator(plugins_directory, Core::DirIterator::SkipDots);

        while (plugins_dir_iterator.has_next())
        {
            auto entry = plugins_dir_iterator.next_full_path();
            if (!Core::File::is_directory(entry))
                continue;

            auto entry_path = LexicalPath(entry);
            auto plugin_main_path = entry_path.append("init.lua");
            if (Core::File::exists(plugin_main_path.string()))
            {
                errored = luaL_dofile(m_state, plugin_main_path.string().characters());
                if (errored)
                {
                    warnln("\u001b[31mFailed to load plugin from path {}\u001b[0m", plugin_main_path);
                    warnln("\u001b[31m{}\u001b[0m", lua_tostring(m_state, -1));
                }
                else
                {
                    outln("\u001b[36mLoaded plugin {}\u001b[0m", plugin_main_path);
                }
            }
        }
    }
    else
    {
        warnln("No plugins directory found, not loading any plugins.");
    }
}

int Engine::format()
{
    lua_pushstring(m_state, Scripting::format(m_state, 1).characters());
    return 1;
}

Engine::~Engine()
{
    s_engines.remove(m_state);
    luaL_unref(m_state, LUA_REGISTRYINDEX, m_base_ref);
    m_base_ref = 0;
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
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onClientChat");
    client_userdata(who.id());
    lua_pushstring(m_state, message.characters());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_sync_projectile(Badge<Server>, const Client& who,
                                        const Terraria::Net::Packets::SyncProjectile& proj_sync)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onClientSyncProjectile");
    client_userdata(who.id());
    Types::projectile(m_state, proj_sync.projectile());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_connect_request(Badge<Server>, const Client& client, const String& version)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onConnectRequest");
    client_userdata(client.id());
    lua_pushstring(m_state, version.characters());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_toggle_pvp(Badge<Server>, const Client& who, const Terraria::Net::Packets::TogglePvp& toggle)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onTogglePvp");
    client_userdata(who.id());
    lua_pushboolean(m_state, toggle.pvp());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_hurt_player(Badge<Server>, Client& who, const Terraria::Net::Packets::PlayerHurt& player_hurt)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onPlayerHurt");
    client_userdata(who.id());
    client_userdata(player_hurt.player_id());
    Types::player_death_reason(m_state, player_hurt.reason());
    lua_pushinteger(m_state, player_hurt.damage());
    lua_pushinteger(m_state, player_hurt.direction());
    lua_pushinteger(m_state, player_hurt.flags());
    lua_pushinteger(m_state, player_hurt.cooldown_counter());
    lua_call(m_state, 7, 0);
}

void Engine::client_did_player_death(Badge<Server>, Client& who, const Terraria::Net::Packets::PlayerDeath& death)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onPlayerDeath");
    client_userdata(who.id());
    client_userdata(death.player_id());
    Types::player_death_reason(m_state, death.reason());
    lua_pushinteger(m_state, death.damage());
    lua_pushinteger(m_state, death.direction());
    lua_pushinteger(m_state, death.flags());
    lua_call(m_state, 6, 0);
}

void Engine::client_did_damage_npc(Badge<Server>, Client& who, const Terraria::Net::Packets::DamageNPC& damage_npc)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onDamageNpc");
    client_userdata(who.id());
    lua_pushinteger(m_state, damage_npc.npc_id());
    lua_pushinteger(m_state, damage_npc.damage());
    lua_pushnumber(m_state, damage_npc.knockback());
    lua_pushinteger(m_state, damage_npc.hit_direction());
    lua_pushboolean(m_state, damage_npc.crit());
    lua_call(m_state, 6, 0);
}

void Engine::client_did_finish_connecting(Badge<Server>, Client& who)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onClientFinishConnecting");
    client_userdata(who.id());
    lua_call(m_state, 1, 0);
}

void Engine::client_did_spawn_player(Badge<Server>, Client& who, const Terraria::Net::Packets::SpawnPlayer&)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onPlayerSpawn");
    client_userdata(who.id());
    lua_call(m_state, 1, 0);
}

void Engine::client_did_modify_tile(Badge<Server>, Client& who, const Terraria::Net::Packets::ModifyTile& modify)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onModifyTile");
    client_userdata(who.id());
    Types::tile_modification(m_state, modify.modification());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_disconnect(Badge<Server>, Client& who, Client::DisconnectReason reason)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onClientDisconnect");
    client_userdata(who.id());
    lua_pushinteger(m_state, static_cast<lua_Integer>(reason));
    lua_call(m_state, 2, 0);
}

void Engine::client_did_sync_player_team(Badge<Server>, Client& who, const Terraria::Net::Packets::PlayerTeam& packet)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onClientSyncPlayerTeam");
    client_userdata(who.id());
    lua_pushinteger(m_state, static_cast<lua_Integer>(packet.team()));
    lua_call(m_state, 2, 0);
}

void Engine::client_did_sync_item(Badge<Server>, Client& who, const Terraria::Net::Packets::SyncItem& packet)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onClientSyncItem");
    client_userdata(who.id());
    Types::dropped_item(m_state, packet.dropped_item());
    lua_pushboolean(m_state, packet.has_pickup_delay());
    lua_pushinteger(m_state, packet.id());
    lua_call(m_state, 4, 0);
}

void Engine::client_did_sync_item_owner(Badge<Server>, Client& who, const Terraria::Net::Packets::SyncItemOwner& packet)
{
    UsingBaseTable base(*this);
    lua_getfield(m_state, -1, "onClientSyncItemOwner");
    client_userdata(who.id());
    lua_pushinteger(m_state, packet.item_id());
    lua_pushinteger(m_state, packet.player_id());
    lua_call(m_state, 3, 0);
}

void* Engine::client_userdata(u8 id) const
{
    auto* client_ud = lua_newuserdata(m_state, sizeof(id));
    memcpy(client_ud, &id, sizeof(id));
    luaL_getmetatable(m_state, "Server::Client");
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

void* Engine::timer_userdata(i32 id) const
{
    auto* inventory_ud = lua_newuserdata(m_state, sizeof(id));
    memcpy(inventory_ud, &id, sizeof(id));
    luaL_getmetatable(m_state, "Engine::Timer");
    lua_setmetatable(m_state, -2);
    return inventory_ud;
}

void Engine::push_base_table() const { lua_rawgeti(m_state, LUA_REGISTRYINDEX, m_base_ref); }

int Engine::timer_create()
{
    bool found_available_id = false;
    i32 timer_id = 0;

    for (auto i = 0; i < NumericLimits<i32>::max(); i++)
    {
        if (!m_timers.contains(i))
        {
            timer_id = i;
            found_available_id = true;
            break;
        }
    }

    VERIFY(found_available_id);

    // Push first function argument to the top of the stack as required by luaL_ref
    lua_pushvalue(m_state, 1);
    auto function_ref = luaL_ref(m_state, LUA_REGISTRYINDEX);
    auto timer = Core::Timer::construct(luaL_checkinteger(m_state, 2), nullptr, nullptr);

    timer->on_timeout = [this, timer_id, function_ref]() {
        lua_rawgeti(m_state, LUA_REGISTRYINDEX, function_ref);
        lua_call(m_state, 0, 1);
        if (lua_toboolean(m_state, -1))
        {
            luaL_unref(m_state, LUA_REGISTRYINDEX, function_ref);

            auto timer = m_timers.get(timer_id);
            VERIFY(timer.has_value());

            timer.value()->stop();
            m_timers.remove(timer_id);
        }
    };

    m_timers.set(timer_id, timer);

    timer_userdata(timer_id);

    return 1;
}

int Engine::timer_destroy()
{
    auto timer_id = *reinterpret_cast<i32*>(luaL_checkudata(m_state, 1, "Engine::Timer"));
    lua_pushboolean(m_state, m_timers.remove(timer_id));

    return 1;
}

int Engine::timer_invoke()
{
    auto timer = *m_timers.get(*reinterpret_cast<i32*>(luaL_checkudata(m_state, 1, "Engine::Timer")));
    timer->on_timeout();

    return 0;
}

JsonObject Engine::serialize_object_to_json(int index)
{
    JsonObject constructed_json_object;

    // TODO: What if we have a table with only numeric keys and wish to serialize it as a JSON array?
    lua_pushnil(m_state);
    while (lua_next(m_state, index) != 0)
    {
        VERIFY(lua_type(m_state, -2) == LUA_TSTRING);
        auto key = lua_tostring(m_state, -2);
        switch (lua_type(m_state, -1))
        {
            case LUA_TSTRING:
                constructed_json_object.set(key, lua_tostring(m_state, -1));
                break;
            case LUA_TNUMBER:
                // TODO: Also handle LUA_TINTEGER type (which doesn't exist?)
                constructed_json_object.set(key, lua_tonumber(m_state, -1));
                break;
            case LUA_TTABLE:
                constructed_json_object.set(key, serialize_object_to_json(lua_gettop(m_state)));
                break;
            default:
                // FIXME: This should probably just luaL_error
                VERIFY_NOT_REACHED();
        }

        lua_pop(m_state, 1);
    }

    return constructed_json_object;
}

void Engine::deserialize_object_to_lua_object(const JsonObject& object)
{
    lua_createtable(m_state, 0, object.size());

    object.for_each_member([&](auto& key, auto& value) {
        if (value.is_string())
        {
            lua_pushstring(m_state, key.characters());
            lua_pushstring(m_state, value.as_string().characters());
            lua_settable(m_state, -3);
        }
        else if (value.is_number())
        {
            lua_pushstring(m_state, key.characters());
            lua_pushnumber(m_state, value.template to_number<LUA_NUMBER>());
            lua_settable(m_state, -3);
        }
        else if (value.is_object())
        {
            lua_pushstring(m_state, key.characters());
            deserialize_object_to_lua_object(value.as_object());
            lua_settable(m_state, -3);
        }
        else
        {
            // FIXME: This should probably not be fatal
            VERIFY_NOT_REACHED();
        }
    });
}

int Engine::json_serialize()
{
    luaL_checktype(m_state, 1, LUA_TTABLE);

    auto constructed_json_object = serialize_object_to_json(1);

    lua_pushstring(m_state, constructed_json_object.to_string().characters());

    return 1;
}

int Engine::json_deserialize()
{
    auto json_string = luaL_checkstring(m_state, 1);

    auto maybe_json = JsonValue::from_string(json_string);
    if (maybe_json.is_error())
    {
        // TODO: Can we include the error from the error type here? Does Lua copy the string?
        luaL_error(m_state, "the supplied JSON was invalid");
        return 0;
    }

    auto json = maybe_json.release_value();

    if (!json.is_object())
    {
        luaL_error(m_state, "the supplied JSON was invalid");
        return 0;
    }

    deserialize_object_to_lua_object(maybe_json.value().as_object());

    return 1;
}

int Engine::game_client()
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

int Engine::game_clients()
{
    auto clients = m_server.clients();
    lua_newtable(m_state);
    for (auto& c : clients)
    {
        if (!c->in_process_of_disconnecting())
        {
            client_userdata(c->id());
            lua_rawseti(m_state, 1, c->id() + 1);
        }
    }

    return 1;
}

int Engine::game_add_projectile()
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

int Engine::game_add_dropped_item()
{
    auto item = Types::dropped_item(m_state, 1);
    auto maybe_id = luaL_optinteger(m_state, 2, -1);

    if (maybe_id == -1)
        maybe_id = m_server.next_available_dropped_item_id();

    m_server.sync_dropped_item(maybe_id, item);

    lua_pushinteger(m_state, maybe_id);

    return 1;
}

int Engine::game_remove_dropped_item()
{
    auto id = luaL_checkinteger(m_state, 1);
    m_server.remove_dropped_item(id);

    return 0;
}

int Engine::game_set_item_owner()
{
    auto id = luaL_checkinteger(m_state, 1);
    auto owner = luaL_checkinteger(m_state, 2);

    auto item = *m_server.dropped_items().get(id);
    item.owner() = owner;

    Terraria::Net::Packets::SyncItemOwner sync_item_owner;
    sync_item_owner.set_item_id(id);
    sync_item_owner.set_player_id(owner);

    for (auto& client : m_server.clients())
        client->send(sync_item_owner);

    m_server.dropped_items().set(id, move(item));

    return 0;
}

int Engine::game_next_available_dropped_item_id()
{
    lua_pushinteger(m_state, m_server.next_available_dropped_item_id());

    return 1;
}

int Engine::client_id()
{
    lua_pushinteger(m_state, *reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    return 1;
}

int Engine::client_is_connected()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    lua_pushboolean(m_state, !client.is_null() && !client->in_process_of_disconnecting());

    return 1;
}

int Engine::client_send_message()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    auto text = luaL_checkstring(m_state, 2);
    auto author = luaL_optinteger(m_state, 3, 255);
    Terraria::Color col;
    if (lua_type(m_state, 4) == LUA_TTABLE)
        col = Types::color(m_state, 4);
    else
        col = {255, 255, 255};

    Terraria::Net::Packets::Modules::Text text_packet;
    text_packet.set_text(text);
    text_packet.set_author(author);
    text_packet.set_color(col);

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
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        lua_pushnil(m_state);
    else
        Types::character(m_state, client->player().character());

    return 1;
}

int Engine::player_update_character()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        return 0;

    auto character = Types::character(m_state, 2);
    client->player().character() = character;

    Terraria::Net::Packets::PlayerInfo player_info;
    player_info.set_player_id(client->id());
    player_info.set_character(character);

    for (auto& c : m_server.clients())
        c->send(player_info);

    return 0;
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

int Engine::client_equals()
{
    lua_pushboolean(m_state, *reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")) ==
                                 *reinterpret_cast<u8*>(luaL_checkudata(m_state, 2, "Server::Client")));

    return 1;
}

int Engine::client_sync_npc()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    auto npc = Types::npc(m_state, 2);

    Terraria::Net::Packets::SyncNPC sync_npc;
    sync_npc.npc() = npc;

    client->send(sync_npc);

    return 0;
}

int Engine::client_sync_tile_rect()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    u16 x = luaL_checkinteger(m_state, 2);
    u16 y = luaL_checkinteger(m_state, 3);

    Terraria::Net::Packets::SyncTileRect sync_tile_rect(m_server.tile_map(), {x, y}, luaL_checkinteger(m_state, 4),
                                                        luaL_checkinteger(m_state, 5));

    client->send(sync_tile_rect);

    return 0;
}

int Engine::client_modify_tile()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client)
        return 0;

    auto modification = Types::tile_modification(m_state, 2);
    Terraria::Net::Packets::ModifyTile modify_tile;
    modify_tile.modification() = modification;

    for (auto& kv : m_server.clients())
    {
        if (kv->id() == client->id())
            continue;

        kv->send(modify_tile);
    }

    m_server.tile_map().process_tile_modification(modification);

    return 0;
}

int Engine::client_uuid()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Server::Client")));
    if (!client || !client->uuid().has_value())
        lua_pushnil(m_state);
    else
        lua_pushstring(m_state, client->uuid()->to_string().characters());

    return 1;
}

int Engine::player_set_pvp()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        return 0;

    auto pvp = lua_toboolean(m_state, 2);
    client->player().set_pvp(pvp);

    Terraria::Net::Packets::TogglePvp toggle_pvp;
    toggle_pvp.set_player_id(client->id());
    toggle_pvp.set_pvp(pvp);

    auto syncToSelf = lua_toboolean(m_state, 3);

    for (auto& c : m_server.clients())
    {
        if (!syncToSelf && c->id() == client->id())
            continue;

        c->send(toggle_pvp);
    }

    return 0;
}

int Engine::player_position()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));

    if (!client)
        lua_pushnil(m_state);
    else
        Types::point(m_state, client->player().position());

    return 1;
}

int Engine::player_teleport()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        return 0;

    auto pos = Types::point(m_state, 2);

    client->player().position() = pos;

    Terraria::Net::Packets::TeleportEntity teleport_entity;
    teleport_entity.set_target(client->id());
    teleport_entity.set_type(Terraria::Net::Packets::TeleportEntity::TeleportType::PlayerToPosition);
    teleport_entity.position() = pos;
    teleport_entity.set_style(luaL_optinteger(m_state, 3, 0));

    for (auto& c : m_server.clients())
        c->send(teleport_entity);

    return 0;
}

int Engine::player_set_team()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        return 0;

    auto team = luaL_checkinteger(m_state, 2);
    if (team < 0 || team > static_cast<u8>(Terraria::Player::Team::__Count))
        VERIFY_NOT_REACHED();

    client->player().set_team(static_cast<Terraria::Player::Team>(team));

    Terraria::Net::Packets::PlayerTeam player_team;
    player_team.set_player_id(client->id());
    player_team.set_team(client->player().team());

    auto syncToSelf = lua_toboolean(m_state, 3);

    for (auto& c : m_server.clients())
    {
        if (!syncToSelf && c->id() == client->id())
            continue;

        c->send(player_team);
    }

    return 0;
}

int Engine::player_set_health()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        return 0;

    auto health = luaL_checkinteger(m_state, 2);
    auto max_health = luaL_optinteger(m_state, 3, -1);
    if (max_health == -1)
        max_health = client->player().max_hp();

    client->player().set_hp(health);
    client->player().set_max_hp(max_health);

    Terraria::Net::Packets::PlayerHP player_hp;
    player_hp.set_player_id(client->id());
    player_hp.set_hp(health);
    player_hp.set_max_hp(max_health);

    auto syncToSelf = lua_toboolean(m_state, 4);

    for (auto& c : m_server.clients())
    {
        if (!syncToSelf && c->id() == client->id())
            continue;

        c->send(player_hp);
    }

    return 0;
}

int Engine::player_set_mana()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        return 0;

    auto mana = luaL_checkinteger(m_state, 2);
    auto max_mana = luaL_optinteger(m_state, 3, -1);
    if (max_mana == -1)
        max_mana = client->player().max_hp();

    client->player().set_mana(mana);
    client->player().set_max_mana(max_mana);

    Terraria::Net::Packets::PlayerMana player_mana;
    player_mana.set_player_id(client->id());
    player_mana.set_mana(mana);
    player_mana.set_max_mana(max_mana);

    auto syncToSelf = lua_toboolean(m_state, 4);

    for (auto& c : m_server.clients())
    {
        if (!syncToSelf && c->id() == client->id())
            continue;

        c->send(player_mana);
    }

    return 0;
}

int Engine::player_item_in_slot()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    auto slot = static_cast<Terraria::PlayerInventory::Slot>(luaL_checkinteger(m_state, 2));

    if (slot < Terraria::PlayerInventory::Slot::Hotbar0 || slot >= Terraria::PlayerInventory::Slot::_Count)
    {
        luaL_error(m_state, "invalid slot");
        return 0;
    }

    if (!client)
        lua_pushnil(m_state);
    else
    {
        auto item = client->player().inventory().get(slot);
        if (item.has_value())
            Types::item(m_state, *item);
        else
            lua_pushnil(m_state);
    }

    return 1;
}

int Engine::player_set_item_in_slot()
{
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
    if (!client)
        return 0;

    auto slot = static_cast<Terraria::PlayerInventory::Slot>(luaL_checkinteger(m_state, 2));
    if (slot < Terraria::PlayerInventory::Slot::Hotbar0 || slot >= Terraria::PlayerInventory::Slot::_Count)
    {
        luaL_error(m_state, "invalid slot");
        return 0;
    }

    Optional<Terraria::Item> item;
    if (lua_gettop(m_state) >= 3 && !lua_isnil(m_state, 3))
        item = Types::item(m_state, 3);

    client->player().inventory().set_item(slot, item);

    Terraria::Net::Packets::SyncInventorySlot inv_slot;
    inv_slot.set_player_id(client->id());
    inv_slot.set_slot(slot);
    if (item.has_value())
        inv_slot.item() = *item;
    else
        inv_slot.item() = Terraria::Item(Terraria::Item::Id::None);

    for (auto& c : m_server.clients())
        c->send(inv_slot);

    return 0;
}

Engine::UsingBaseTable::~UsingBaseTable() { lua_pop(m_engine.m_state, 1); }

}