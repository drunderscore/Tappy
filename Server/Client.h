/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/RefCounted.h>
#include <AK/UUID.h>
#include <AK/WeakPtr.h>
#include <LibCore/TCPSocket.h>
#include <LibCore/Timer.h>
#include <LibTerraria/Net/NetworkText.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Player.h>
#include <Server/SocketStream.h>

class Server;

class Client : public Weakable<Client>
{
public:
    enum class DisconnectReason
    {
        EofReached, // The TCP socket reached EOF
        DisconnectedByServer,
        StreamErrored
    };

    Client(NonnullRefPtr<Core::TCPSocket> socket, Server& server, u8 id);

    u8 id() const { return m_id; }

    const Terraria::Player& player() const { return m_player; }

    Terraria::Player& player() { return m_player; }

    IPv4Address address() const { return m_socket->source_address().ipv4_address(); }

    void send(const Terraria::Net::Packet&);

    void disconnect(const Terraria::Net::NetworkText&);

    bool has_finished_connecting() const { return m_has_finished_connecting; }

    bool in_process_of_disconnecting() const { return m_in_process_of_disconnecting; }

    void full_sync(Client& to);

    const Optional<UUID>& uuid() const { return m_uuid; }

private:
    void on_ready_to_read();

    void send_keep_alive();

    Server& m_server;
    NonnullRefPtr<Core::TCPSocket> m_socket;
    SocketStream m_stream;
    Terraria::Player m_player;
    Optional<UUID> m_uuid;
    u8 m_id;
    bool m_has_finished_connecting{};
    bool m_in_process_of_disconnecting{};
    RefPtr<Core::Timer> m_keep_alive_timer;
};