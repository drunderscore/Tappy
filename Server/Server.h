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

class Server
{
public:
    Server();

    bool listen(AK::IPv4Address addr = {}, u16 port = 7777);

    int exec();

    void client_did_send_message(Badge<Client>, const Client&, const String&);

private:
    NonnullRefPtr<Core::TCPServer> m_server;
    HashMap<u8, NonnullOwnPtr<Client>> m_clients;
    Core::EventLoop m_event_loop;
};