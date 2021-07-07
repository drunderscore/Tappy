/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Client.h>
#include <Server/Server.h>
#include <LibTerraria/Net/NetworkText.h>
#include <LibTerraria/Net/Packets/SetUserSlot.h>
#include <LibTerraria/Net/Packets/PlayerInfo.h>
#include <LibTerraria/Net/Packets/SyncInventorySlot.h>
#include <LibTerraria/Net/Packets/ClientUUID.h>
#include <LibTerraria/Net/Packets/PlayerHP.h>
#include <LibTerraria/Net/Packets/PlayerMana.h>
#include <LibTerraria/Net/Packets/WorldData.h>
#include <LibTerraria/Net/Packets/PlayerBuffs.h>
#include <LibTerraria/Net/Packets/SpawnData.h>
#include <LibTerraria/Net/Packets/TileSection.h>
#include <LibTerraria/Net/Packets/SpawnPlayer.h>
#include <LibTerraria/Net/Packets/ConnectRequest.h>
#include <LibTerraria/Net/Packets/SpawnPlayerSelf.h>
#include <LibTerraria/Net/Packets/ConnectFinished.h>
#include <LibTerraria/Net/Packets/TileFrameSection.h>
#include <LibTerraria/Net/Packets/SyncPlayer.h>
#include <LibTerraria/Net/Packets/SyncProjectile.h>
#include <LibTerraria/Net/Packets/SyncNPC.h>
#include <LibTerraria/Net/Packets/ReleaseNPC.h>
#include <LibTerraria/Net/Packets/KillProjectile.h>
#include <LibTerraria/Net/Packets/Modules/Text.h>
#include <LibTerraria/Net/Packets/Disconnect.h>
#include <math.h>
#include <LibTerraria/Net/Packets/PlayerActive.h>
#include <LibTerraria/Net/Packets/PlayerDead.h>

#define USE_BOGUS_KEEP_ALIVE_PACKET 0

Client::Client(NonnullRefPtr<Core::TCPSocket> socket, Server& server, u8 id) :
        m_socket(move(socket)),
        m_id(id),
        m_input_stream(m_socket),
        m_output_stream(m_socket),
        m_server(server)
{
    m_socket->on_ready_to_read = [this]()
    {
        on_ready_to_read();
    };

    if constexpr(USE_BOGUS_KEEP_ALIVE_PACKET)
    {
        m_keep_alive_timer = Core::Timer::create_repeating(5000, [this]
        {
            send_keep_alive();
        });
        m_keep_alive_timer->start();
    }
}

void Client::send(const Terraria::Net::Packet& packet)
{
    auto bytes = packet.to_bytes();
    m_output_stream << static_cast<u16>(bytes.size() + 2);
    m_output_stream << bytes;
    if (m_output_stream.handle_any_error())
    {
        m_in_process_of_disconnecting = true;
        warnln("Stream errored trying to send data");
        m_server.client_did_disconnect({}, *this, DisconnectReason::StreamErrored);
    }
}

void Client::disconnect(const Terraria::Net::NetworkText& reason)
{
    m_in_process_of_disconnecting = true;
    Terraria::Net::Packets::Disconnect disconnect;
    disconnect.set_reason(reason);
    send(disconnect);
    m_server.client_did_disconnect({}, *this, DisconnectReason::DisconnectedByServer);
}

void Client::full_sync(Client& to)
{
    Terraria::Net::Packets::PlayerActive player_active;
    player_active.set_player_id(m_id);
    player_active.set_active(1);
    to.send(player_active);

    Terraria::Net::Packets::PlayerInfo player_info;
    player_info.set_player_id(m_id);
    player_info.set_character(m_player.character());
    to.send(player_info);

    Terraria::Net::Packets::SyncPlayer sync_player;
    sync_player.set_player_id(m_id);
    sync_player.set_control_bits(m_player.control_bits());
    sync_player.set_bits_2(m_player.bits_2());
    sync_player.set_bits_3(m_player.bits_3());
    sync_player.set_bits_4(m_player.bits_4());
    sync_player.set_selected_item(static_cast<u8>(m_player.inventory().selected_slot()));
    sync_player.position() = m_player.position();
    sync_player.velocity() = m_player.velocity();
    // TODO: Potion of return information
    to.send(sync_player);

    if (m_player.hp() <= 0)
    {
        Terraria::Net::Packets::PlayerDead player_dead;
        player_dead.set_player_id(m_id);
        to.send(player_dead);
    }

    Terraria::Net::Packets::PlayerHP player_hp;
    player_hp.set_player_id(m_id);
    player_hp.set_hp(m_player.hp());
    player_hp.set_max_hp(m_player.max_hp());
    to.send(player_hp);

    Terraria::Net::Packets::TogglePvp toggle_pvp;
    toggle_pvp.set_player_id(m_id);
    toggle_pvp.set_pvp(m_player.pvp());
    send(toggle_pvp);

    // TODO: Player teams

    Terraria::Net::Packets::PlayerMana player_mana;
    player_mana.set_player_id(m_id);
    player_mana.set_mana(m_player.mana());
    player_mana.set_max_mana(m_player.max_mana());
    to.send(player_mana);

    Terraria::Net::Packets::PlayerBuffs player_buffs;
    player_buffs.set_player_id(m_id);
    player_buffs.buffs() = m_player.buffs();
    to.send(player_buffs);

    // TODO: Player chest index (the currently open chest)

    Terraria::Net::Packets::SyncInventorySlot sync_inventory_slot;
    sync_inventory_slot.set_player_id(m_id);
    // @formatter:off
    m_player.inventory().for_each([&](auto slot, auto item)
    {
        sync_inventory_slot.set_slot(slot);
        sync_inventory_slot.item() = item;
        to.send(sync_inventory_slot);
    });
    // @formatter:on
}

void Client::on_ready_to_read()
{
    if (m_socket->eof())
    {
        m_in_process_of_disconnecting = true;
        m_server.client_did_disconnect({}, *this, DisconnectReason::EofReached);
        return;
    }

    u16 packet_size;
    m_input_stream >> packet_size;

    if (m_input_stream.handle_any_error())
    {
        m_in_process_of_disconnecting = true;
        warnln("Stream errored trying to read packet size");
        m_server.client_did_disconnect({}, *this, DisconnectReason::StreamErrored);
        return;
    }

    Terraria::Net::Packet::Id packet_id;
    m_input_stream >> packet_id;
    if (m_input_stream.handle_any_error())
    {
        m_in_process_of_disconnecting = true;
        warnln("Stream errored trying to read packet id");
        m_server.client_did_disconnect({}, *this, DisconnectReason::StreamErrored);
        return;
    }

    // The packet size (2 bytes) and id (1 byte) are part of this.
    auto bytes = m_socket->read(packet_size - 3);
    InputMemoryStream packet_bytes_stream(bytes);

    // TODO: Some of these packets contain the player id, but we ignore that and assume it's the player id we assigned to this socket.

    // Connection request, let's send a user slot
    if (packet_id == Terraria::Net::Packet::Id::ConnectRequest)
    {
        auto request = Terraria::Net::Packets::ConnectRequest::from_bytes(packet_bytes_stream);

        m_server.client_did_connect_request({}, *this, request->version());

        Terraria::Net::Packets::SetUserSlot set_user_slot;
        set_user_slot.set_player_id(m_id);
        send(set_user_slot);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerInfo)
    {
        auto player_info = Terraria::Net::Packets::PlayerInfo::from_bytes(packet_bytes_stream);
        m_player.character() = player_info->character();
        outln("Got character, created player for {}", m_player.character().name());
        m_server.client_did_send_player_info({}, *this, *player_info);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncInventorySlot)
    {
        auto inv_slot = Terraria::Net::Packets::SyncInventorySlot::from_bytes(packet_bytes_stream);
        if (inv_slot->item().id() == Terraria::Item::Id::None)
            m_player.inventory().set_item(inv_slot->slot(), {});
        else
            m_player.inventory().set_item(inv_slot->slot(), inv_slot->item());
        m_server.client_did_sync_inventory_slot({}, *this, *inv_slot);
    }
    else if (packet_id == Terraria::Net::Packet::Id::RequestWorldData)
    {
        m_server.client_did_request_world_data({}, *this);
    }
    else if (packet_id == Terraria::Net::Packet::Id::ClientUUID)
    {
        auto client_uuid = Terraria::Net::Packets::ClientUUID::from_bytes(packet_bytes_stream);
        if (client_uuid->uuid().length() != 36)
            warnln("Client sent UUID that isn't 36 characters.");
        else
            m_uuid = UUID(client_uuid->uuid().view());
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerHP)
    {
        auto player_hp = Terraria::Net::Packets::PlayerHP::from_bytes(packet_bytes_stream);
        m_player.set_hp(player_hp->hp());
        m_player.set_max_hp(player_hp->max_hp());
        m_server.client_did_sync_hp({}, *this, *player_hp);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerBuffs)
    {
        auto player_buffs = Terraria::Net::Packets::PlayerBuffs::from_bytes(packet_bytes_stream);
        player_buffs->buffs().span().copy_to(m_player.buffs().span());
        m_server.client_did_sync_buffs({}, *this, *player_buffs);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerMana)
    {
        auto player_mana = Terraria::Net::Packets::PlayerMana::from_bytes(packet_bytes_stream);
        m_player.set_mana(player_mana->mana());
        m_player.set_max_mana(player_mana->max_mana());
        m_server.client_did_sync_mana({}, *this, *player_mana);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SpawnData)
    {
        auto spawn_data = Terraria::Net::Packets::SpawnData::from_bytes(packet_bytes_stream);
        m_server.client_did_request_spawn_sections({}, *this, *spawn_data);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SpawnPlayer)
    {
        auto spawn_player = Terraria::Net::Packets::SpawnPlayer::from_bytes(packet_bytes_stream);
        outln("Wants to spawn player, probably themselves. Fuck that, let's just tell them to finish.");
        if (!m_has_finished_connecting)
        {
            m_server.client_did_finish_connecting({}, *this);
            Terraria::Net::Packets::ConnectFinished connect_finished;
            send(connect_finished);
            m_has_finished_connecting = true;
        }
        m_server.client_did_spawn_player({}, *this, *spawn_player);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncPlayer)
    {
        auto sync_player = Terraria::Net::Packets::SyncPlayer::from_bytes(packet_bytes_stream);
        m_player.set_control_bits(sync_player->control_bits());
        m_player.set_bits_2(sync_player->bits_2());
        m_player.set_bits_3(sync_player->bits_3());
        m_player.set_bits_4(sync_player->bits_4());
        m_player.inventory().set_selected_slot(
                static_cast<Terraria::PlayerInventory::Slot>(sync_player->selected_item()));
        m_player.position() = sync_player->position();
        if (sync_player->velocity().has_value())
            m_player.velocity() = *sync_player->velocity();
        // TODO: Do something with potion of return use and home position
        m_server.client_did_sync_player({}, *this, *sync_player);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncProjectile)
    {
        auto proj = Terraria::Net::Packets::SyncProjectile::from_bytes(packet_bytes_stream);
        m_server.client_did_sync_projectile({}, *this, *proj);
    }
    else if (packet_id == Terraria::Net::Packet::Id::NetModules)
    {
        Terraria::Net::Packet::ModuleId module;
        packet_bytes_stream >> module;
        if (module == Terraria::Net::Packet::ModuleId::Text)
        {
            auto text = Terraria::Net::Packets::Modules::Text::from_bytes(packet_bytes_stream);
            if (text->command_name() == "Say")
                m_server.client_did_send_message({}, *this, text->message());
        }
    }
    else if (packet_id == Terraria::Net::Packet::Id::KillProjectile)
    {
        auto kill_proj = Terraria::Net::Packets::KillProjectile::from_bytes(packet_bytes_stream);
        m_server.client_did_kill_projectile({}, *this, *kill_proj);
    }
    else if (packet_id == Terraria::Net::Packet::Id::TogglePvp)
    {
        auto toggle_pvp = Terraria::Net::Packets::TogglePvp::from_bytes(packet_bytes_stream);
        m_server.client_did_toggle_pvp({}, *this, *toggle_pvp);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerHurt)
    {
        auto player_hurt = Terraria::Net::Packets::PlayerHurt::from_bytes(packet_bytes_stream);
        m_server.client_did_hurt_player({}, *this, *player_hurt);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerDeath)
    {
        auto player_death = Terraria::Net::Packets::PlayerDeath::from_bytes(packet_bytes_stream);
        m_server.client_did_player_death({}, *this, *player_death);
    }
    else if (packet_id == Terraria::Net::Packet::Id::DamageNPC)
    {
        auto damage_npc = Terraria::Net::Packets::DamageNPC::from_bytes(packet_bytes_stream);
        m_server.client_did_damage_npc({}, *this, *damage_npc);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerItemAnimation)
    {
        auto item_anim = Terraria::Net::Packets::PlayerItemAnimation::from_bytes(packet_bytes_stream);
        m_server.client_did_item_animation({}, *this, *item_anim);
    }
    else if (packet_id == Terraria::Net::Packet::Id::ModifyTile)
    {
        auto modify_tile = Terraria::Net::Packets::ModifyTile::from_bytes(packet_bytes_stream);
        m_server.client_did_modify_tile({}, *this, *modify_tile);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncTilePicking)
    {
        auto sync_tile_picking = Terraria::Net::Packets::SyncTilePicking::from_bytes(packet_bytes_stream);
        m_server.client_did_sync_tile_picking({}, *this, *sync_tile_picking);
    }
    else if (packet_id == Terraria::Net::Packet::Id::AddPlayerBuff)
    {
        auto add_player_buff = Terraria::Net::Packets::AddPlayerBuff::from_bytes(packet_bytes_stream);
        m_server.client_did_add_player_buff({}, *this, *add_player_buff);
    }
    else if (packet_id == Terraria::Net::Packet::Id::SyncTalkNPC)
    {
        auto sync_talk_npc = Terraria::Net::Packets::SyncTalkNPC::from_bytes(packet_bytes_stream);
        m_server.client_did_sync_talk_npc({}, *this, *sync_talk_npc);
    }
    else if (packet_id == Terraria::Net::Packet::Id::PlayerTeam)
    {
        auto player_team = Terraria::Net::Packets::PlayerTeam::from_bytes(packet_bytes_stream);
        m_server.client_did_sync_player_team({}, *this, *player_team);
    }
    else if (packet_id == Terraria::Net::Packet::Id::ClientSyncedInventory)
    {
        // This packet has no data, and is completely useless.
    }
    else
    {
        warnln("Unhandled packet {}", packet_id);
    }

    if (packet_bytes_stream.handle_any_error())
    {
        m_in_process_of_disconnecting = true;
        warnln("Stream errored trying to read packet data");
        m_server.client_did_disconnect({}, *this, DisconnectReason::StreamErrored);
        return;
    }
}

void Client::send_keep_alive()
{
    // This is our way of keeping the client connection alive
    // The client will think the server has disconnected after 7200 ticks (120 seconds) of no data received.
    // Packet ID 0 is unused, so we just send it as bogus, and it knows we're still here.
    m_output_stream << static_cast<u16>(3);
    m_output_stream << static_cast<u8>(0);
}