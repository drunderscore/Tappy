/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <LibTerraria/World.h>
#include <Server/Server.h>

static Server* s_server;

Server& server()
{
    return *s_server;
}

int main(int argc, char** argv)
{
    Core::ArgsParser args_parser;

    String world_path;

    args_parser.add_positional_argument(world_path, "Path to the world file", "world");

    if (!args_parser.parse(argc, argv))
        return 1;

    auto file_or_error = Core::File::open(world_path, Core::IODevice::OpenMode::ReadOnly);

    if (file_or_error.is_error())
    {
        warnln("Failed to open world file: {}", file_or_error.error());
        return 2;
    }

    auto file_bytes = file_or_error.value()->read_all();
    auto bytes_stream = InputMemoryStream(file_bytes);

    auto world_or_error = Terraria::World::try_load_world(bytes_stream);

    if (world_or_error.is_error())
    {
        warnln("Failed to load world file: {}", world_or_error.error());
        return 3;
    }

    auto world = world_or_error.release_value();

    s_server = new Server(world);
    if (!s_server->listen())
    {
        warnln("Server failed to listen.");
        return 4;
    }

    return s_server->exec();
}
