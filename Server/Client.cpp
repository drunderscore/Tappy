#include <Server/Client.h>
#include <LibCore/FileStream.h>
#include <LibTerraria/Net/NetworkText.h>
#include <LibTerraria/Net/Packets/Disconnect.h>
#include <LibTerraria/Net/Packets/SetUserSlot.h>
#include <LibTerraria/Net/Packets/PlayerInfo.h>

Client::Client(NonnullRefPtr<Core::TCPSocket> socket, u8 id) : m_socket(move(socket)), m_id(id)
{
    m_socket->on_ready_to_read = [this]()
    {
        on_ready_to_read();
    };
}

void Client::on_ready_to_read()
{
    if (m_socket->eof())
    {
        if (on_disconnect)
            on_disconnect(DisconnectReason::EofReached);
        return;
    }

    Core::InputFileStream stream(m_socket);
    u16 packet_size;
    stream >> packet_size;

    if (stream.has_any_error())
    {
        dbgln("Stream errored trying to read packet size");
        stream.handle_any_error();
        return;
    }

    u8 packet_id;
    stream >> packet_id;
    if (stream.has_any_error())
    {
        dbgln("Stream errored trying to read packet id");
        stream.handle_any_error();
        return;
    }

    // The packet size (2 bytes) and id (1 byte) are part of this.
    auto bytes = m_socket->read(packet_size - 3);
    outln("Received packet {}, size {}, with {} data bytes", packet_id, packet_size, bytes.size());

    // Connection request, let's send a user slot
    if (packet_id == 1)
    {
        Terraria::Net::Packets::SetUserSlot set_user_slot;
        set_user_slot.set_player_id(m_id);
        auto bytes = set_user_slot.to_bytes();
        auto bytes_length = static_cast<u16>(bytes.size());
        m_socket->send({&bytes_length, sizeof(bytes_length)});
        m_socket->send(bytes);
    }
    else if (packet_id == 4)
    {
        InputMemoryStream packet_bytes_stream(bytes);

        auto player_info = Terraria::Net::Packets::PlayerInfo::from_bytes(packet_bytes_stream);
        outln("Player id is {}", player_info->player_id());
        outln("Player name is _{}_", player_info->name());
    }
}