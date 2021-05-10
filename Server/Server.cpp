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