/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <AK/NonnullRefPtr.h>
#include <LibCore/Socket.h>

class SocketStream : public InputStream, public OutputStream
{
public:
    explicit SocketStream(NonnullRefPtr<Core::Socket> socket) : m_socket(move(socket)) {}

    size_t read(Bytes bytes) override { return m_socket->read(bytes.data(), bytes.size()); }

    bool unreliable_eof() const override { return eof(); }

    bool eof() const { return m_socket->eof(); }

    bool read_or_error(Bytes bytes) override
    {
        if (read(bytes) < bytes.size())
        {
            set_fatal_error();
            return false;
        }

        return true;
    }

    bool discard_or_error(size_t count) override
    {
        // TODO: Don't use a VLA
        u8 discarded_data[count];

        return read(Bytes{discarded_data, count});
    }

    size_t write(ReadonlyBytes bytes) override
    {
        m_socket->send(bytes);

        return bytes.size();
    }

    bool write_or_error(ReadonlyBytes bytes) override
    {
        if (write(bytes) < bytes.size())
        {
            set_fatal_error();
            return false;
        }

        return true;
    }

private:
    NonnullRefPtr<Core::Socket> m_socket;
};