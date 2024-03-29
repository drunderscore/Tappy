/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Badge.h>
#include <AK/HashMap.h>
#include <LibCore/EventLoop.h>
#include <LibCore/TCPServer.h>
#include <LibTerraria/DroppedItem.h>
#include <LibTerraria/Net/Packets/AddPlayerBuff.h>
#include <LibTerraria/Net/Packets/DamageNPC.h>
#include <LibTerraria/Net/Packets/KillProjectile.h>
#include <LibTerraria/Net/Packets/ModifyTile.h>
#include <LibTerraria/Net/Packets/PlaceObject.h>
#include <LibTerraria/Net/Packets/PlayerBuffs.h>
#include <LibTerraria/Net/Packets/PlayerDeath.h>
#include <LibTerraria/Net/Packets/PlayerHP.h>
#include <LibTerraria/Net/Packets/PlayerHurt.h>
#include <LibTerraria/Net/Packets/PlayerInfo.h>
#include <LibTerraria/Net/Packets/PlayerItemAnimation.h>
#include <LibTerraria/Net/Packets/PlayerMana.h>
#include <LibTerraria/Net/Packets/PlayerTeam.h>
#include <LibTerraria/Net/Packets/SpawnData.h>
#include <LibTerraria/Net/Packets/SpawnPlayer.h>
#include <LibTerraria/Net/Packets/SyncInventorySlot.h>
#include <LibTerraria/Net/Packets/SyncItem.h>
#include <LibTerraria/Net/Packets/SyncItemOwner.h>
#include <LibTerraria/Net/Packets/SyncPlayer.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/SyncTalkNPC.h>
#include <LibTerraria/Net/Packets/SyncTilePicking.h>
#include <LibTerraria/Net/Packets/TogglePvp.h>
#include <LibTerraria/Projectile.h>
#include <LibTerraria/TileMap.h>
#include <LibTerraria/World.h>
#include <Server/Client.h>

namespace Scripting
{
class Engine;
}

class Server : public Core::Object
{
    C_OBJECT(Server);

public:
    Server(RefPtr<Terraria::World>);

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

    void client_did_disconnect(Badge<Client>, Client&, Client::DisconnectReason);

    void client_did_add_player_buff(Badge<Client>, Client&, const Terraria::Net::Packets::AddPlayerBuff&);

    void client_did_sync_talk_npc(Badge<Client>, Client&, const Terraria::Net::Packets::SyncTalkNPC&);

    void client_did_sync_player_team(Badge<Client>, Client&, const Terraria::Net::Packets::PlayerTeam&);

    void client_did_sync_item(Badge<Client>, Client&, Terraria::Net::Packets::SyncItem&);

    void client_did_sync_item_owner(Badge<Client>, Client&, Terraria::Net::Packets::SyncItemOwner&);

    void client_did_place_object(Badge<Client>, Client&, Terraria::Net::Packets::PlaceObject&);

    Vector<WeakPtr<Client>> clients() const;

    const WeakPtr<Client> client(u8 id) const;

    const HashMap<i16, Terraria::Projectile>& projectiles() const { return m_projectiles; }

    HashMap<i16, Terraria::Projectile>& projectiles() { return m_projectiles; }

    const HashMap<i16, Terraria::DroppedItem>& dropped_items() const { return m_dropped_items; }

    HashMap<i16, Terraria::DroppedItem>& dropped_items() { return m_dropped_items; }

    const Terraria::TileMap& tile_map() const { return *m_world->tile_map(); }

    Terraria::TileMap& tile_map() { return *m_world->tile_map(); }

    const RefPtr<Terraria::World> world() const { return m_world; }

    i16 next_available_dropped_item_id() const;

    i16 sync_dropped_item(i16 id, const Terraria::DroppedItem);

    void remove_dropped_item(i16 id);

    WeakPtr<Client> find_owner_for_item(const Terraria::DroppedItem&, Optional<u8> ignore_id);

private:
    OwnPtr<Scripting::Engine> m_engine;
    NonnullRefPtr<Core::TCPServer> m_server;
    HashMap<u8, NonnullOwnPtr<Client>> m_clients;
    Core::EventLoop m_event_loop;
    HashMap<i16, Terraria::Projectile> m_projectiles;
    HashMap<i16, Terraria::DroppedItem> m_dropped_items;
    RefPtr<Terraria::World> m_world;
};