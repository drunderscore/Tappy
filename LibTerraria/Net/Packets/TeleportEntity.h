/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Optional.h>
#include <LibTerraria/Net/Packet.h>
#include <LibTerraria/Point.h>

namespace Terraria::Net::Packets
{
class TeleportEntity : public Terraria::Net::Packet
{
public:
    enum class TeleportType : u8
    {
        PlayerToPosition,
        NPCToPosition,
        PlayerToPlayer,
    };

    TeleportEntity() = default;

    const char* packet_name() const override { return "TeleportEntity"; }

    static Optional<TeleportEntity> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const override;

    i16 target() const { return m_target; }

    bool get_position_from_target() const { m_get_position_from_target; }

    TeleportType type() const { return m_type; }

    const EntityPoint& position() const { return m_position; }

    u8 style() const { return m_style; }

    const Optional<i32>& extra_info() const { return m_extra_info; }

    void set_target(i16 value) { m_target = value; }

    void set_get_position_from_target(bool value) { m_get_position_from_target = value; }

    void set_type(TeleportType value) { m_type = value; }

    void set_style(u8 value) { m_style = value; }

    EntityPoint& position() { return m_position; }

    Optional<i32>& extra_info() { return m_extra_info; }

private:
    i16 m_target{};
    bool m_get_position_from_target{};
    TeleportType m_type{};
    EntityPoint m_position;
    u8 m_style{};
    Optional<i32> m_extra_info;

    static constexpr u8 m_teleport_npc_bit = 0b0000'0001;
    static constexpr u8 m_teleport_player_to_player_bit = 0b0000'0010;
    static constexpr u8 m_get_position_from_target_bit = 0b0000'0100;
    static constexpr u8 m_extra_info_bit = 0b0000'1000;
};
}