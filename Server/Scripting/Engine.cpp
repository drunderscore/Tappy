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
#include <LibTerraria/Net/Packets/TeleportEntity.h>
#include <LibTerraria/Net/Packets/SyncTileRect.h>
#include <LibTerraria/Net/Packets/SyncNPC.h>
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
                    {"client",           game_client_thunk},
                    {"clients",          game_clients_thunk},
                    {"addProjectile",    game_add_projectile_thunk},
                    {"projectileExists", game_projectile_exists_thunk},
                    {}
            };

    static const struct luaL_Reg timer_lib[] =
            {
                    {"create", timer_create_thunk},
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
                    {"__eq",           client_equals_thunk},
                    {"syncNpc",        client_sync_npc_thunk},
                    {"syncTileRect",   client_sync_tile_rect_thunk},
                    {"modifyTile",     client_modify_tile_thunk},
                    {}
            };

    static const struct luaL_Reg player_lib[] =
            {
                    {"character",       player_character_thunk},
                    {"buffs",           player_buffs_thunk},
                    {"setPvp",          player_set_pvp_thunk},
                    {"buffs",           player_buffs_thunk},
                    {"setPvp",          player_set_pvp_thunk},
                    {"position",        player_position_thunk},
                    {"inventory",       player_inventory_thunk},
                    {"character",       player_character_thunk},
                    {"updateCharacter", player_update_character_thunk},
                    {"teleport",        player_teleport_thunk},
                    {}
            };

    static const struct luaL_Reg inventory_lib[] =
            {
                    {"item",    inventory_item_thunk},
                    {"setItem", inventory_set_item_thunk},
                    {"owner",   inventory_owner_thunk},
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

    luaL_newmetatable(m_state, "Terraria::PlayerInventory");
    lua_pushstring(m_state, "__index");
    lua_pushvalue(m_state, -2);
    lua_settable(m_state, -3);

    luaL_setfuncs(m_state, inventory_lib, 0);
    lua_pop(m_state, 1);

    luaL_newlib(m_state, game_lib);
    lua_setglobal(m_state, "Game");

    luaL_newlib(m_state, timer_lib);
    lua_setglobal(m_state, "Timer");

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

void Engine::client_did_toggle_pvp(Badge<Server>, const Client& who, const Terraria::Net::Packets::TogglePvp& toggle)
{
    lua_getfield(m_state, 1, "onTogglePvp");
    client_userdata(who.id());
    lua_pushboolean(m_state, toggle.pvp());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_hurt_player(Badge<Server>, Client& who, const Terraria::Net::Packets::PlayerHurt& player_hurt)
{
    lua_getfield(m_state, 1, "onPlayerHurt");
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
    lua_getfield(m_state, 1, "onPlayerDeath");
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
    lua_getfield(m_state, 1, "onDamageNpc");
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
    lua_getfield(m_state, 1, "onClientFinishConnecting");
    client_userdata(who.id());
    lua_call(m_state, 1, 0);
}

void Engine::client_did_spawn_player(Badge<Server>, Client& who, const Terraria::Net::Packets::SpawnPlayer&)
{
    lua_getfield(m_state, 1, "onPlayerSpawn");
    client_userdata(who.id());
    lua_call(m_state, 1, 0);
}

void Engine::client_did_modify_tile(Badge<Server>, Client& who, const Terraria::Net::Packets::ModifyTile& modify)
{
    lua_getfield(m_state, 1, "onModifyTile");
    client_userdata(who.id());
    Types::tile_modification(m_state, modify.modification());
    lua_call(m_state, 2, 0);
}

void Engine::client_did_disconnect(Badge<Server>, Client& who, Client::DisconnectReason reason)
{
    lua_getfield(m_state, 1, "onClientDisconnect");
    client_userdata(who.id());
    lua_pushinteger(m_state, static_cast<lua_Integer>(reason));
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

void* Engine::player_userdata(u8 id) const
{
    auto* player_ud = lua_newuserdata(m_state, sizeof(id));
    memcpy(player_ud, &id, sizeof(id));
    luaL_getmetatable(m_state, "Terraria::Player");
    lua_setmetatable(m_state, -2);
    return player_ud;
}

void* Engine::inventory_userdata(u8 id) const
{
    auto* inventory_ud = lua_newuserdata(m_state, sizeof(id));
    memcpy(inventory_ud, &id, sizeof(id));
    luaL_getmetatable(m_state, "Terraria::PlayerInventory");
    lua_setmetatable(m_state, -2);
    return inventory_ud;
}

int Engine::timer_create()
{
    auto function_ref = luaL_ref(m_state, LUA_REGISTRYINDEX);
    auto timer = Core::Timer::construct(luaL_checkinteger(m_state, 1), nullptr, nullptr);
    timer->on_timeout = [timer, this, function_ref]()
    {
        lua_rawgeti(m_state, LUA_REGISTRYINDEX, function_ref);
        lua_call(m_state, 0, 1);
        if (lua_toboolean(m_state, -1))
        {
            luaL_unref(m_state, LUA_REGISTRYINDEX, function_ref);
            auto index = m_timers.find_first_index(timer);
            // If this timer is still ticking, but not in our timers, we've got problems.
            VERIFY(index.has_value());
            // FIXME: This is the dumbest fucking thing on planet earth
            // What person writing C++ specification EVER thought using this oblique and arbitrary
            // keyword 'mutable' was a good idea to ever do???
            // If you've been const incorrect for 20+ years, now is not a good time to start.
            // Because now you're inconsistent with what is expected from everything else.
            // Fucking idiots.
            m_timers.at(*index)->stop();
            m_timers.remove(*index);
        }
    };

    m_timers.append(timer);

    return 0;
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
        client_userdata(c->id());
        lua_rawseti(m_state, 1, c->id() + 1);
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

int Engine::game_projectile_exists()
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

    Terraria::Net::Packets::SyncTileRect sync_tile_rect
            (
                    m_server.tile_map(),
                    {x, y},
                    luaL_checkinteger(m_state, 4),
                    luaL_checkinteger(m_state, 5)
            );

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

int Engine::player_inventory()
{
    inventory_userdata(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::Player")));
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

int Engine::inventory_owner()
{
    lua_pushinteger(m_state, *reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::PlayerInventory")));
    return 1;
}

int Engine::inventory_item()
{
    // FIXME: Should there be a difference between the client/PlayerInventory being invalid and there being no item?
    // FIXME: This Slot ID needs some validation.
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::PlayerInventory")));
    auto slot = luaL_checkinteger(m_state, 2);
    if (!client)
        lua_pushnil(m_state);
    else
    {
        auto item = client->player().inventory().get(static_cast<Terraria::PlayerInventory::Slot>(slot));
        if (item.has_value())
            Types::item(m_state, *item);
        else
            lua_pushnil(m_state);
    }

    return 1;
}

int Engine::inventory_set_item()
{
    // FIXME: This Slot ID needs some validation.
    auto client = m_server.client(*reinterpret_cast<u8*>(luaL_checkudata(m_state, 1, "Terraria::PlayerInventory")));
    if (!client)
        return 0;

    auto slot = static_cast<Terraria::PlayerInventory::Slot>(luaL_checkinteger(m_state, 2));
    Optional<Terraria::Item> item;
    if (lua_gettop(m_state) < 3 || lua_isnil(m_state, 3))
        item = Terraria::Item(Terraria::Item::Id::None);
    else
        item = Types::item(m_state, 3);

    client->player().inventory().set_item(slot, item);

    Terraria::Net::Packets::SyncInventorySlot inv_slot;
    inv_slot.set_player_id(client->id());
    inv_slot.set_slot(slot);
    inv_slot.item() = *item;

    for (auto& c : m_server.clients())
        c->send(inv_slot);

    return 0;
}

}