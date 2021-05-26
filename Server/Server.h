/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <LibCore/EventLoop.h>
#include <AK/HashMap.h>
#include <Server/Client.h>
#include <LibCore/TCPServer.h>
#include <AK/Badge.h>
#include <LibTerraria/Net/Packets/SyncPlayer.h>
#include <LibTerraria/Net/Packets/SpawnPlayer.h>
#include <LibTerraria/Net/Packets/PlayerInfo.h>
#include <LibTerraria/Net/Packets/SyncInventorySlot.h>
#include <LibTerraria/Net/Packets/PlayerBuffs.h>
#include <LibTerraria/Net/Packets/PlayerMana.h>
#include <LibTerraria/Net/Packets/PlayerHP.h>
#include <LibTerraria/Net/Packets/PlayerMana.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/KillProjectile.h>
#include <LibTerraria/Net/Packets/TogglePvp.h>
#include <LibTerraria/Net/Packets/PlayerHurt.h>
#include <LibTerraria/Net/Packets/PlayerDeath.h>
#include <LibTerraria/Net/Packets/DamageNPC.h>
#include <LibTerraria/Net/Packets/PlayerItemAnimation.h>
#include <LibTerraria/Net/Packets/SpawnData.h>
#include <LibTerraria/Net/Packets/ModifyTile.h>
#include <LibTerraria/Net/Packets/SyncTilePicking.h>
#include <LibTerraria/TileMap.h>
#include <LibTerraria/Projectile.h>

namespace Scripting
{
class Engine;
}

class Server
{
public:
    Server();

    bool listen(AK::IPv4Address addr = {}, u16 port = 7777);

    int exec();

    void client_did_send_message(Badge<Client>, const Client&, const String&);

    void client_did_sync_player(Badge<Client>, const Client&, Terraria::Net::Packets::SyncPlayer&);

    void client_did_request_world_data(Badge<Client>, Client&);

    void client_did_spawn_player(Badge<Client>, Client&, const Terraria::Net::Packets::SpawnPlayer&);

    void client_did_send_player_info(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerInfo&);

    void client_did_sync_inventory_slot(Badge<Client>, Client&, const Terraria::Net::Packets::SyncInventorySlot&);

    void client_did_sync_buffs(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerBuffs&);

    void client_did_sync_mana(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerMana&);

    void client_did_sync_hp(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerHP&);

    void client_did_sync_projectile(Badge<Client>, const Client&, const Terraria::Net::Packets::SyncProjectile&);

    void client_did_connect_request(Badge<Client>, const Client&, const String& version);

    void client_did_kill_projectile(Badge<Client>, const Client&, const Terraria::Net::Packets::KillProjectile&);

    void client_did_toggle_pvp(Badge<Client>, const Client&, const Terraria::Net::Packets::TogglePvp&);

    void client_did_hurt_player(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerHurt&);

    void client_did_player_death(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerDeath&);

    void client_did_damage_npc(Badge<Client>, Client&, const Terraria::Net::Packets::DamageNPC&);

    void client_did_finish_connecting(Badge<Client>, Client&);

    void client_did_item_animation(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerItemAnimation&);

    void client_did_request_spawn_sections(Badge<Client>, Client&, const Terraria::Net::Packets::SpawnData&);

    void client_did_modify_tile(Badge<Client>, Client&, const Terraria::Net::Packets::ModifyTile&);

    void client_did_sync_tile_picking(Badge<Client>, Client&, const Terraria::Net::Packets::SyncTilePicking&);

    Vector<WeakPtr<Client>> clients() const;

    const WeakPtr<Client> client(u8 id) const;

    const HashMap<i16, Terraria::Projectile>& projectiles() const
    { return m_projectiles; }

    HashMap<i16, Terraria::Projectile>& projectiles()
    { return m_projectiles; }

private:
    OwnPtr<Scripting::Engine> m_engine;
    NonnullRefPtr<Core::TCPServer> m_server;
    HashMap<u8, NonnullOwnPtr<Client>> m_clients;
    Core::EventLoop m_event_loop;
    HashMap<i16, Terraria::Projectile> m_projectiles;
    Terraria::MemoryTileMap m_tile_map;
};