/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Model.h>
#include <LibTerraria/Net/Packets/Modules/Text.h>
#include <LibTerraria/Net/Packets/PlayerActive.h>
#include <LibTerraria/Net/Packets/SpawnPlayerSelf.h>
#include <LibTerraria/Net/Packets/SyncItemOwner.h>
#include <LibTerraria/Net/Packets/SyncPlayer.h>
#include <LibTerraria/Net/Packets/SyncTileRect.h>
#include <LibTerraria/Net/Packets/TileFrameSection.h>
#include <LibTerraria/Net/Packets/TileSection.h>
#include <LibTerraria/Net/Packets/WorldData.h>
#include <Server/Scripting/Engine.h>
#include <Server/Server.h>

constexpr i16 s_max_dropped_items = 400;

Server::Server(RefPtr<Terraria::World> world) : m_server(Core::TCPServer::construct()), m_world(world)
{
    m_engine = make<Scripting::Engine>(*this);
    m_server->on_ready_to_accept = [this] {
        auto socket = m_server->accept();
        if (!socket)
        {
            perror("accept");
            return;
        }

        Optional<u8> id;
        for (u8 i = 0; i < NumericLimits<u8>::max(); i++)
        {
            if (!m_clients.contains(i))
            {
                id = i;
                break;
            }
        }

        if (!id.has_value())
        {
            warnln("Out of available client IDs, not accepting client {}", socket->source_address().ipv4_address());
            return;
        }

        m_clients.set(*id, make<Client>(socket.release_nonnull(), *this, *id));
    };
}

void Server::client_did_send_message(Badge<Client>, const Client& who, const String& message)
{
    m_engine->client_did_send_message({}, who, message);
}

void Server::client_did_sync_projectile(Badge<Client>, const Client& who,
                                        const Terraria::Net::Packets::SyncProjectile& proj_sync)
{
    m_engine->client_did_sync_projectile({}, who, proj_sync);
}

void Server::client_did_connect_request(Badge<Client>, const Client& client, const String& version)
{
    m_engine->client_did_connect_request({}, client, version);
}

void Server::client_did_sync_player(Badge<Client>, const Client& who, Terraria::Net::Packets::SyncPlayer& sync_player)
{
    if (!who.has_finished_connecting())
        return;

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(sync_player);
    }
}

void Server::client_did_send_player_info(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerInfo& info)
{
    if (!who.has_finished_connecting())
        return;

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(info);
    }
}

void Server::client_did_request_world_data(Badge<Client>, Client& who)
{
    Terraria::Net::Packets::WorldData world_data;

    world_data.set_time(m_world->m_time);
    // FIXME: Is this correct?
    world_data.set_day_state(m_world->m_day_time);
    world_data.set_max_tiles_x(m_world->m_max_tiles_x);
    world_data.set_max_tiles_y(m_world->m_max_tiles_y);
    world_data.set_spawn_x(m_world->m_spawn_tile.x());
    world_data.set_spawn_y(m_world->m_spawn_tile.y());
    world_data.set_world_surface(m_world->m_surface);
    world_data.set_rock_layer(m_world->m_rock_layer);
    world_data.set_world_id(m_world->m_id);
    world_data.set_world_name(m_world->name());
    world_data.set_gamemode(m_world->m_game_mode);
    world_data.set_world_generator_version(m_world->m_generator_version);

    for (auto i = 0; i < m_world->m_tree_x.size(); i++)
        world_data.tree_x()[i] = m_world->m_tree_x[i];

    for (auto i = 0; i < m_world->m_tree_style.size(); i++)
        world_data.tree_style()[i] = m_world->m_tree_style[i];

    for (auto i = 0; i < world_data.tree_top_variations().size(); i++)
        world_data.tree_top_variations()[i] = m_world->m_tree_tops[i];

    world_data.set_copper_ore_tier(m_world->m_copper_tier);
    world_data.set_iron_ore_tier(m_world->m_iron_tier);
    world_data.set_silver_ore_tier(m_world->m_silver_tier);
    world_data.set_gold_ore_tier(m_world->m_gold_tier);
    world_data.set_mythril_ore_tier(m_world->m_mythril_tier);
    world_data.set_adamantite_ore_tier(m_world->m_adamantite_tier);
    world_data.set_intended_sandstorm_severity(m_world->m_sandstorm_intended_severity);

    world_data.set_world_flags_1(world_data.world_flags_1() | 0b0100'0000);

    who.send(world_data);
}

void Server::client_did_spawn_player(Badge<Client>, Client& client, const Terraria::Net::Packets::SpawnPlayer& spawn)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == client.id())
            continue;

        kv.value->send(spawn);
    }

    m_engine->client_did_spawn_player({}, client, spawn);
}

void Server::client_did_sync_mana(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerMana& player_mana)
{
    if (!who.has_finished_connecting())
        return;

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(player_mana);
    }
}

void Server::client_did_sync_hp(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerHP& player_hp)
{
    if (!who.has_finished_connecting())
        return;

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(player_hp);
    }
}

void Server::client_did_sync_buffs(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerBuffs& buffs)
{
    if (!who.has_finished_connecting())
        return;

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(buffs);
    }
}

void Server::client_did_sync_inventory_slot(Badge<Client>, Client& who,
                                            const Terraria::Net::Packets::SyncInventorySlot& inv_slot)
{
    if (!who.has_finished_connecting())
        return;

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(inv_slot);
    }
}

void Server::client_did_kill_projectile(Badge<Client>, const Client& who,
                                        const Terraria::Net::Packets::KillProjectile& kill_proj)
{
    m_projectiles.remove(kill_proj.projectile_id());
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(kill_proj);
    }
}

void Server::client_did_toggle_pvp(Badge<Client>, const Client& who, const Terraria::Net::Packets::TogglePvp& toggle)
{
    // Is it okay that this isn't behind a has_finished_connecting()?
    m_engine->client_did_toggle_pvp({}, who, toggle);
}

void Server::client_did_hurt_player(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerHurt& hurt)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(hurt);
    }
    m_engine->client_did_hurt_player({}, who, hurt);
}

void Server::client_did_player_death(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerDeath& death)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(death);
    }
    m_engine->client_did_player_death({}, who, death);
}

void Server::client_did_damage_npc(Badge<Client>, Client& who, const Terraria::Net::Packets::DamageNPC& damage_npc)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(damage_npc);
    }
    m_engine->client_did_damage_npc({}, who, damage_npc);
}

void Server::client_did_finish_connecting(Badge<Client>, Client& who)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        who.full_sync(*kv.value);
        kv.value->full_sync(who);
    }

    for (auto& kv : m_projectiles)
    {
        Terraria::Net::Packets::SyncProjectile sync_projectile;
        sync_projectile.projectile() = kv.value;
        who.send(sync_projectile);
    }

    m_engine->client_did_finish_connecting({}, who);
}

void Server::client_did_item_animation(Badge<Client>, Client& who,
                                       const Terraria::Net::Packets::PlayerItemAnimation& item_anim)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(item_anim);
    }
}

void Server::client_did_request_spawn_sections(Badge<Client>, Client& who, const Terraria::Net::Packets::SpawnData&)
{
    for (int i = 0; i < m_world->tile_map()->width() / 100; i++)
    {
        Terraria::Net::Packets::TileSection tile_section(tile_map(), i * 100, 0, 100, m_world->tile_map()->height());
        who.send(tile_section);
    }

    Terraria::Net::Packets::TileFrameSection frame_section;
    frame_section.set_start_x(0);
    frame_section.set_start_y(0);
    frame_section.set_end_x(tile_map().width() / 200);
    frame_section.set_end_y(tile_map().height() / 150);
    who.send(frame_section);

    Terraria::Net::Packets::SpawnPlayerSelf spawn_self;
    who.send(spawn_self);
}

void Server::client_did_modify_tile(Badge<Client>, Client& who, const Terraria::Net::Packets::ModifyTile& modify_tile)
{
    m_engine->client_did_modify_tile({}, who, modify_tile);
}

void Server::client_did_sync_tile_picking(Badge<Client>, Client& who,
                                          const Terraria::Net::Packets::SyncTilePicking& sync_tile_picking)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(sync_tile_picking);
    }
    // TODO: Should we save this in the tile? I'm not sure it really pays to save it, or if the game does at all.
}

void Server::client_did_disconnect(Badge<Client>, Client& who, Client::DisconnectReason reason)
{
    auto id = who.id();
    auto addr = who.address();
    m_engine->client_did_disconnect({}, who, reason);

    // @formatter:off
    deferred_invoke([this, id, addr](auto&) {
        outln("Client {}/{} disconnected.", id, addr);
        m_clients.remove(id);

        Terraria::Net::Packets::PlayerActive player_active;
        player_active.set_player_id(id);
        player_active.set_active(0);

        for (auto& kv : m_clients)
            kv.value->send(player_active);

        // Let's remove all of this client's projectiles when they are disconnected
        for (auto& kv : m_projectiles)
        {
            if (kv.value.owner() == id)
            {
                Terraria::Net::Packets::KillProjectile kill_projectile;
                kill_projectile.set_projectile_id(kv.key);
                kill_projectile.set_owner(id);
                for (auto& client_kv : m_clients)
                    client_kv.value->send(kill_projectile);
            }
        }
    });
    // @formatter:on
}

void Server::client_did_add_player_buff(Badge<Client>, Client& who, const Terraria::Net::Packets::AddPlayerBuff& packet)
{
    auto target = client(packet.player_id());
    if (!target)
    {
        warnln("Client {} sent add player buff for non-existent client {}", who.id(), packet.player_id());
        return;
    }

    // FIXME: this is a pretty yucky way of doing this
    // TODO: Implement duration here!
    for (auto& buff : target->player().buffs())
    {
        if (buff == 0)
        {
            buff = packet.buff();
            break;
        }
    }

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(packet);
    }
}

void Server::client_did_sync_talk_npc(Badge<Client>, Client& who, const Terraria::Net::Packets::SyncTalkNPC& packet)
{
    auto talk_npc = packet.talk_npc();
    if (talk_npc == -1)
        who.player().talk_npc() = {};
    else
        who.player().talk_npc() = talk_npc;

    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(packet);
    }
}

void Server::client_did_sync_player_team(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerTeam& packet)
{
    m_engine->client_did_sync_player_team({}, who, packet);
}

WeakPtr<Client> Server::find_owner_for_item(const Terraria::DroppedItem& item, Optional<u8> ignore_id)
{
    WeakPtr<Client> closest;
    float last_distance;

    for (auto& kv : m_clients)
    {
        if (ignore_id.has_value() && kv.key == *ignore_id)
            continue;

        auto distance = kv.value->player().position().distance_between(item.position());

        if (!closest || distance < last_distance)
        {
            closest = kv.value->make_weak_ptr();
            last_distance = distance;
        }
    }

    return closest;
}

void Server::client_did_sync_item(Badge<Client>, Client& who, Terraria::Net::Packets::SyncItem& packet)
{
    m_engine->client_did_sync_item({}, who, packet);
}

void Server::client_did_sync_item_owner(Badge<Client>, Client& who, Terraria::Net::Packets::SyncItemOwner& packet)
{
    auto item = m_dropped_items.get(packet.item_id());
    if (!item.has_value())
        return;

    // You must be the current owner to modify the owner (or there be no existing owner)
    if (item->owner().has_value() && item->owner() != who.id())
        return;

    item->owner() = packet.player_id();

    m_dropped_items.set(packet.item_id(), item.release_value());

    for (auto& kv : m_clients)
        kv.value->send(packet);

    m_engine->client_did_sync_item_owner({}, who, packet);
}

void Server::client_did_place_object(Badge<Client>, Client& who, Terraria::Net::Packets::PlaceObject& packet)
{
    auto& object = Terraria::s_tile_objects[packet.type()];
    tile_map().place_object(packet.position(), object, packet.style(), packet.alternate(), packet.random(),
                            packet.direction());
    for (auto& kv : m_clients)
    {
        if (kv.key != who.id())
            kv.value->send(packet);
    }
}

i16 Server::next_available_dropped_item_id() const
{
    for (i16 i = 0; i < s_max_dropped_items; i++)
    {
        if (!m_dropped_items.contains(i))
            return i;
    }

    // FIXME: Maybe should return Optional<i16>
    // We have run out of available item IDs.
    VERIFY_NOT_REACHED();
}

i16 Server::sync_dropped_item(i16 id, const Terraria::DroppedItem item)
{
    Terraria::Net::Packets::SyncItem sync_item;
    sync_item.set_id(id);
    sync_item.dropped_item() = item;

    for (auto& kv : m_clients)
        kv.value->send(sync_item);

    if (item.owner().has_value() && !m_dropped_items.contains(id))
    {
        Terraria::Net::Packets::SyncItemOwner sync_item_owner;
        sync_item_owner.set_item_id(id);
        sync_item_owner.set_player_id(*item.owner());
        for (auto& kv : m_clients)
            kv.value->send(sync_item_owner);
    }

    m_dropped_items.set(id, move(item));

    return id;
}

void Server::remove_dropped_item(i16 id)
{
    m_dropped_items.remove(id);

    Terraria::Net::Packets::SyncItem sync_item;
    sync_item.set_id(id);
    sync_item.dropped_item().item().set_id(Terraria::Item::Id::None);

    for (auto& kv : m_clients)
        kv.value->send(sync_item);
}

bool Server::listen(AK::IPv4Address addr, u16 port)
{
    if (!m_server->listen(addr, port))
    {
        perror("listen");
        return false;
    }

    return true;
}

int Server::exec() { return m_event_loop.exec(); }

const WeakPtr<Client> Server::client(u8 id) const
{
    auto client = m_clients.get(id);
    if (!client.has_value())
        return {};

    return (*client)->make_weak_ptr();
}

Vector<WeakPtr<Client>> Server::clients() const
{
    Vector<WeakPtr<Client>> clients;
    for (auto& kv : m_clients)
        clients.append(kv.value->make_weak_ptr());

    return clients;
}