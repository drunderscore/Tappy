/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <LibMain/Main.h>
#include <LibTerraria/World.h>
#include <Server/Server.h>

static Server* s_server;

Server& server() { return *s_server; }

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    Core::ArgsParser args_parser;

    String world_path;

    args_parser.add_positional_argument(world_path, "Path to the world file", "world");

    if (!args_parser.parse(arguments))
        return 1;

    auto file = TRY(Core::File::open(world_path, Core::OpenMode::ReadOnly));

    auto file_bytes = file->read_all();
    InputMemoryStream bytes_stream(file_bytes);

    auto world = TRY(Terraria::World::try_load_world(bytes_stream));

    s_server = new Server(world);
    if (!s_server->listen())
    {
        warnln("Server failed to listen.");
        return 4;
    }

    return s_server->exec();
}
