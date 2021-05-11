/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <LibTerraria/Net/Packet.h>
#include <AK/RefCounted.h>
#include <AK/WeakPtr.h>
#include <AK/UUID.h>
#include <LibCore/Timer.h>
#include <LibCore/TCPSocket.h>
#include <LibCore/FileStream.h>
#include <LibTerraria/Player.h>

class Server;

class Client : public Weakable<Client>
{
public:
    enum class DisconnectReason
    {
        EofReached,                 // The TCP socket reached EOF
        TookTooLongToConnect        // The client took too long to connect
    };

    Client(NonnullRefPtr<Core::TCPSocket> socket, Server& server, u8 id);

    Function<void(DisconnectReason)> on_disconnect;

    u8 id() const
    { return m_id; }

    const WeakPtr<Terraria::Player> player() const
    {
        if (!m_player)
            return {};
        return m_player->make_weak_ptr();
    }

    WeakPtr<Terraria::Player> player()
    {
        if (!m_player)
            return {};
        return m_player->make_weak_ptr();
    }

    IPv4Address address() const
    {
        return m_socket->source_address().ipv4_address();
    }

    void send(const Terraria::Net::Packet& packet);

private:

    void on_ready_to_read();

    Server& m_server;
    NonnullRefPtr<Core::TCPSocket> m_socket;
    Core::InputFileStream m_input_stream;
    Core::OutputFileStream m_output_stream;
    OwnPtr<Terraria::Player> m_player;
    Optional<UUID> m_uuid;
    u8 m_id;
};