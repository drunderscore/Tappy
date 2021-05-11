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

private:
    NonnullRefPtr<Core::TCPServer> m_server;
    HashMap<u8, NonnullOwnPtr<Client>> m_clients;
    Core::EventLoop m_event_loop;
};