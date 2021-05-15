/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Server.h>
#include <LibTerraria/Net/Packets/Modules/Text.h>

Server::Server() : m_server(Core::TCPServer::construct())
{
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
    outln("\u001b[33m{}/{}: {}\u001b[0m", who.player()->character().name(), who.address(), message);
    Terraria::Net::Packets::Modules::Text text;
    text.set_text(message);
    text.set_color({255, 255, 255});
    text.set_author(who.id());

    for (auto& client : m_clients)
        client.value->send(text);
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
        info.set_skin_variant(kv.value->player().character().skin_variant());
        info.set_hair(kv.value->player().character().hair());
        info.set_name(kv.value->player().character().name().isolated_copy());
        info.set_hair_dye(kv.value->player().character().hair_dye());
        info.set_hide_visuals(kv.value->player().character().hide_visuals());
        info.set_hide_visuals2(kv.value->player().character().hide_visuals2());
        info.set_hide_misc(kv.value->player().character().hide_misc());
        info.set_hair_color(kv.value->player().character().hair_color());
        info.set_skin_color(kv.value->player().character().skin_color());
        info.set_eye_color(kv.value->player().character().eye_color());
        info.set_shirt_color(kv.value->player().character().shirt_color());
        info.set_undershirt_color(kv.value->player().character().undershirt_color());
        info.set_pants_color(kv.value->player().character().pants_color());
        info.set_shoe_color(kv.value->player().character().shoe_color());
        info.set_difficulty_flags(kv.value->player().character().difficulty_flags());
        info.set_torch_flags(kv.value->player().character().torch_flags());
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
            inv_slot.set_id(item.id());
            inv_slot.set_stack(item.stack());
            inv_slot.set_prefix(item.prefix());
            who.send(inv_slot);
        });
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