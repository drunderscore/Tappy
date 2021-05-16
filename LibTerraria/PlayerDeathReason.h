/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Optional.h>
#include <AK/Stream.h>
#include <AK/ByteBuffer.h>
#include <LibTerraria/Item.h>
#include <AK/String.h>

namespace Terraria
{
class PlayerDeathReason
{
public:
    static Optional<PlayerDeathReason> from_bytes(InputStream& stream);

    ByteBuffer to_bytes() const;

    const Optional<u8>& player() const
    { return m_player; }

    const Optional<i16>& npc() const
    { return m_npc; }

    const Optional<i16>& projectile() const
    { return m_projectile; }

    const Optional<u8>& other() const
    { return m_other; }

    const Optional<String>& custom_reason() const
    { return m_custom_reason; }

    const Optional<i16>& projectile_type() const
    { return m_projectile_type; }

    const Optional<Item::Id>& item_id() const
    { return m_item_id; }

    const Optional<Item::Prefix>& item_prefix() const
    { return m_item_prefix; }

    Optional<u8>& player()
    { return m_player; }

    Optional<i16>& npc()
    { return m_npc; }

    Optional<i16>& projectile()
    { return m_projectile; }

    Optional<u8>& other()
    { return m_other; }

    Optional<String>& custom_reason()
    { return m_custom_reason; }

    Optional<i16>& projectile_type()
    { return m_projectile_type; }

    Optional<Item::Id>& item_id()
    { return m_item_id; }

    Optional<Item::Prefix>& item_prefix()
    { return m_item_prefix; }

private:
    Optional<u8> m_player;
    Optional<i16> m_npc;
    Optional<i16> m_projectile;
    Optional<u8> m_other;
    Optional<String> m_custom_reason;
    Optional<i16> m_projectile_type;
    Optional<Item::Id> m_item_id;
    Optional<Item::Prefix> m_item_prefix;

    static constexpr u8 m_player_bit = 0b0000'0001;
    static constexpr u8 m_npc_bit = 0b0000'0010;
    static constexpr u8 m_projectile_bit = 0b0000'0100;
    static constexpr u8 m_other_bit = 0b0000'1000;
    static constexpr u8 m_projectile_type_bit = 0b0001'0000;
    static constexpr u8 m_item_type_bit = 0b0010'0000;
    static constexpr u8 m_item_prefix_bit = 0b0100'0000;
    static constexpr u8 m_custom_reason_bit = 0b1000'0000;
};
}