#include <LibCore/TCPServer.h>
#include <LibCore/EventLoop.h>
#include <Server/Client.h>

int main(int, char **)
{
    Core::EventLoop event_loop;

    auto server = Core::TCPServer::construct();
    HashMap<u8, NonnullRefPtr<Client>> clients;

    if (!server->listen({}, 7777))
    {
        perror("listen");
        return 1;
    }

    server->on_ready_to_accept = [&]
    {
        auto socket = server->accept();
        if (!socket)
        {
            perror("accept");
            return;
        }

        Optional<u8> id;
        for (u8 i = 0; i < NumericLimits<u8>::max(); i++)
        {
            if (!clients.contains(i))
            {
                id = i;
                break;
            }
        }

        if (!id.has_value())
        {
            warnln("Out of client IDs, ignoring newly connecting client.");
            return;
        }

        auto id_val = *id;

        auto client = adopt_ref(*new Client(socket.release_nonnull(), id_val));
        client->on_disconnect = [&, id_val](auto reason)
        {
            outln("Client {} disconnecting, removing...", id_val);
            clients.remove(id_val);
        };

        clients.set(id_val, client);
        outln("Accepted client {}", id_val);
    };

    return event_loop.exec();
}
