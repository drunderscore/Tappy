/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Optional.h>
#include <AK/Stream.h>
#include <AK/String.h>
#include <LibTerraria/Item.h>

namespace Terraria
{
class PlayerDeathReason
{
public:
    const Optional<u8>& player() const { return m_player; }

    const Optional<i16>& npc() const { return m_npc; }

    const Optional<i16>& projectile() const { return m_projectile; }

    const Optional<u8>& other() const { return m_other; }

    const Optional<String>& custom_reason() const { return m_custom_reason; }

    const Optional<i16>& projectile_type() const { return m_projectile_type; }

    const Optional<Item::Id>& item_id() const { return m_item_id; }

    const Optional<Item::Prefix>& item_prefix() const { return m_item_prefix; }

    Optional<u8>& player() { return m_player; }

    Optional<i16>& npc() { return m_npc; }

    Optional<i16>& projectile() { return m_projectile; }

    Optional<u8>& other() { return m_other; }

    Optional<String>& custom_reason() { return m_custom_reason; }

    Optional<i16>& projectile_type() { return m_projectile_type; }

    Optional<Item::Id>& item_id() { return m_item_id; }

    Optional<Item::Prefix>& item_prefix() { return m_item_prefix; }

private:
    Optional<u8> m_player;
    Optional<i16> m_npc;
    Optional<i16> m_projectile;
    Optional<u8> m_other;
    Optional<String> m_custom_reason;
    Optional<i16> m_projectile_type;
    Optional<Item::Id> m_item_id;
    Optional<Item::Prefix> m_item_prefix;
};
}

InputStream& operator>>(InputStream& stream, Terraria::PlayerDeathReason& value);

OutputStream& operator<<(OutputStream& stream, const Terraria::PlayerDeathReason& value);