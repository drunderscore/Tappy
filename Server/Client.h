#pragma once

#include <AK/RefCounted.h>
#include <LibCore/Timer.h>
#include <LibCore/TCPSocket.h>

class Client : public RefCounted<Client>
{
public:
    enum class DisconnectReason
    {
        EofReached,                 // The TCP socket reached EOF
        TookTooLongToConnect        // The client took too long to connect
    };

    Client(NonnullRefPtr<Core::TCPSocket> socket, u8 id);

    Function<void(DisconnectReason)> on_disconnect;

    u8 id() const
    { return m_id; }

private:
    void on_ready_to_read();

    NonnullRefPtr<Core::TCPSocket> m_socket;
    RefPtr<Core::Timer> m_connection_timeout_timer;
    u8 m_id;
};