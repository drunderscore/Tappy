/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Server.h>
#include <LibTerraria/Net/Packets/Modules/Text.h>
#include <Server/Scripting/Engine.h>

Server::Server() : m_server(Core::TCPServer::construct())
{
    m_engine = make<Scripting::Engine>(*this);
    m_server->on_ready_to_accept = [this]
    {
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

        auto id_val = *id;
        auto client = make<Client>(socket.release_nonnull(), *this, id_val);

        client->on_disconnect = [this, id_val](auto reason)
        {
            outln("Client {} has disconnected.", id_val);
            m_clients.remove(id_val);
        };
        m_clients.set(id_val, move(client));
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
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(sync_player);
    }
}

void Server::client_did_send_player_info(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerInfo& info)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(info);
    }
}

void Server::client_did_request_world_data(Badge<Client>, Client& who)
{
    // FIXME: When should we send player info of existing players?
    // When a client joins, we have to tell them about all existing players info.
    // When should we do this? Is this the right place?
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;
        Terraria::Net::Packets::PlayerInfo info;
        info.set_player_id(kv.value->id());
        info.set_character(kv.value->player().character());
        who.send(info);

        Terraria::Net::Packets::SpawnPlayer spawn;
        spawn.set_player_id(kv.key);
        spawn.set_spawn_x(kv.value->player().position().x);
        spawn.set_spawn_x(kv.value->player().position().y);
        spawn.set_remaining_respawn_time(0);
        spawn.set_context(1);
        who.send(spawn);

        kv.value->player().inventory().for_each([&](auto& slot, auto& item)
        {
            Terraria::Net::Packets::SyncInventorySlot inv_slot;
            inv_slot.set_player_id(kv.key);
            inv_slot.set_slot(slot);
            inv_slot.item() = item;
            who.send(inv_slot);
        });

        Terraria::Net::Packets::TogglePvp toggle_pvp;
        toggle_pvp.set_player_id(kv.key);
        toggle_pvp.set_pvp(kv.value->player().pvp());
        who.send(toggle_pvp);
    }

    for (auto& kv : m_projectiles)
    {
        Terraria::Net::Packets::SyncProjectile sync_proj;
        sync_proj.projectile() = kv.value;
        who.send(sync_proj);
    }
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
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(player_mana);
    }
}

void Server::client_did_sync_hp(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerHP& player_hp)
{
    for (auto& kv : m_clients)
    {
        if (kv.key == who.id())
            continue;

        kv.value->send(player_hp);
    }
}

void Server::client_did_sync_buffs(Badge<Client>, Client& who, const Terraria::Net::Packets::PlayerBuffs& buffs)
{
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

bool Server::listen(AK::IPv4Address addr, u16 port)
{
    if (!m_server->listen(addr, port))
    {
        perror("listen");
        return false;
    }

    return true;
}

int Server::exec()
{
    return m_event_loop.exec();
}

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