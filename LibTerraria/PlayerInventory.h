/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Types.h>
#include <AK/Stream.h>
#include <AK/HashMap.h>
#include <LibTerraria/Item.h>
#include <AK/Function.h>

namespace Terraria
{
class PlayerInventory
{
public:
    PlayerInventory() = default;

    enum class [[maybe_unused]] Slot : i16
    {
        Hotbar0,
        Hotbar1,
        Hotbar2,
        Hotbar3,
        Hotbar4,
        Hotbar5,
        Hotbar6,
        Hotbar7,
        Hotbar8,
        Hotbar9,
        Primary10,
        Primary11,
        Primary12,
        Primary13,
        Primary14,
        Primary15,
        Primary16,
        Primary17,
        Primary18,
        Primary19,
        Primary20,
        Primary21,
        Primary22,
        Primary23,
        Primary24,
        Primary25,
        Primary26,
        Primary27,
        Primary28,
        Primary29,
        Primary30,
        Primary31,
        Primary32,
        Primary33,
        Primary34,
        Primary35,
        Primary36,
        Primary37,
        Primary38,
        Primary39,
        Primary40,
        Primary41,
        Primary42,
        Primary43,
        Primary44,
        Primary45,
        Primary46,
        Primary47,
        Primary48,
        Primary49,
        Coin1,
        Coin2,
        Coin3,
        Coin4,
        Ammo1,
        Ammo2,
        Ammo3,
        Ammo4,
        Cursor,
        Accessory1,
        Accessory2,
        Accessory3,
        Accessory4,
        Accessory5,
        Accessory6,
        Accessory7,
        Accessory8,
        Accessory9,
        Accessory10,
        Vanity1,
        Vanity2,
        Vanity3,
        Vanity4,
        Vanity5,
        Vanity6,
        Vanity7,
        Vanity8,
        Vanity9,
        Vanity10,
        Dye1,
        Dye2,
        Dye3,
        Dye4,
        Dye5,
        Dye6,
        Dye7,
        Dye8,
        Dye9,
        Dye10,
        Pet,
        LightPet,
        Minecart,
        Mount,
        GrapplingHook,
        PetDye,
        LightPetDye,
        MinecartDye,
        MountDye,
        GrapplingHookDye,
        Piggy0,
        Piggy1,
        Piggy2,
        Piggy3,
        Piggy4,
        Piggy5,
        Piggy6,
        Piggy7,
        Piggy8,
        Piggy9,
        Piggy10,
        Piggy11,
        Piggy12,
        Piggy13,
        Piggy14,
        Piggy15,
        Piggy16,
        Piggy17,
        Piggy18,
        Piggy19,
        Piggy20,
        Piggy21,
        Piggy22,
        Piggy23,
        Piggy24,
        Piggy25,
        Piggy26,
        Piggy27,
        Piggy28,
        Piggy29,
        Piggy30,
        Piggy31,
        Piggy32,
        Piggy33,
        Piggy34,
        Piggy35,
        Piggy36,
        Piggy37,
        Piggy38,
        Piggy39,
        Bank0,
        Bank1,
        Bank2,
        Bank3,
        Bank4,
        Bank5,
        Bank6,
        Bank7,
        Bank8,
        Bank9,
        Bank10,
        Bank11,
        Bank12,
        Bank13,
        Bank14,
        Bank15,
        Bank16,
        Bank17,
        Bank18,
        Bank19,
        Bank20,
        Bank21,
        Bank22,
        Bank23,
        Bank24,
        Bank25,
        Bank26,
        Bank27,
        Bank28,
        Bank29,
        Bank30,
        Bank31,
        Bank32,
        Bank33,
        Bank34,
        Bank35,
        Bank36,
        Bank37,
        Bank38,
        Bank39,
        Trash,
        Void0,
        Void1,
        Void2,
        Void3,
        Void4,
        Void5,
        Void6,
        Void7,
        Void8,
        Void9,
        Void10,
        Void11,
        Void12,
        Void13,
        Void14,
        Void15,
        Void16,
        Void17,
        Void18,
        Void19,
        Void20,
        Void21,
        Void22,
        Void23,
        Void24,
        Void25,
        Void26,
        Void27,
        Void28,
        Void29,
        Void30,
        Void31,
        Void32,
        Void33,
        Void34,
        Void35,
        Void36,
        Void37,
        Void38,
        Void39,
        Forge0,
        Forge1,
        Forge2,
        Forge3,
        Forge4,
        Forge5,
        Forge6,
        Forge7,
        Forge8,
        Forge9,
        Forge10,
        Forge11,
        Forge12,
        Forge13,
        Forge14,
        Forge15,
        Forge16,
        Forge17,
        Forge18,
        Forge19,
        Forge20,
        Forge21,
        Forge22,
        Forge23,
        Forge24,
        Forge25,
        Forge26,
        Forge27,
        Forge28,
        Forge29,
        Forge30,
        Forge31,
        Forge32,
        Forge33,
        Forge34,
        Forge35,
        Forge36,
        Forge37,
        Forge38,
        Forge39,
        _Count
    };

    void set_item(Slot slot, Optional<Item> item)
    {
        if (!item.has_value())
            m_items.remove(slot);
        else
            m_items.set(slot, *item);
    }

    Optional<Item> get(Slot slot) const
    {
        return m_items.get(slot);
    }

    template<typename Callback>
    void for_each(Callback cb)
    {
        for (auto& kv : m_items)
        {
            cb(kv.key, kv.value);
        }
    }

    void set_selected_slot(Slot value)
    {
        if (m_selected_slot == value)
            return;

        if (on_selected_slot_change)
            on_selected_slot_change(m_selected_slot, value);
        m_selected_slot = value;
    }

    Slot selected_slot() const
    { return m_selected_slot; }

    Optional<Item> selected_item() const
    { return get(m_selected_slot); }

    // (slot_from, slot_to)
    Function<void(Slot&, Slot&)> on_selected_slot_change;
    Function<void(Slot&, Item&)> on_insert_item;

private:
    AK::HashMap<Slot, Item> m_items;
    Slot m_selected_slot{};
};
}

template<>
struct AK::Formatter<Terraria::PlayerInventory::Slot> : AK::Formatter<String>
{
    void format(FormatBuilder& builder, Terraria::PlayerInventory::Slot slot)
    {
        builder.put_i64(static_cast<i16>(slot));
    }
};

template<>
struct AK::Traits<Terraria::PlayerInventory::Slot> : public GenericTraits<Terraria::PlayerInventory::Slot>
{
    static constexpr unsigned hash(Terraria::PlayerInventory::Slot slot)
    { return int_hash(static_cast<i16>(slot)); }
};

InputStream& operator>>(InputStream& stream, Terraria::PlayerInventory::Slot& value);

OutputStream& operator<<(OutputStream& stream, Terraria::PlayerInventory::Slot value);