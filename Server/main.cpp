/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <Server/Server.h>

static Server* s_server;

Server& server()
{
    return *s_server;
}

int main(int, char**)
{
    s_server = new Server;
    if (!s_server->listen())
    {
        warnln("Server failed to listen.");
        return 1;
    }

    return s_server->exec();
}
