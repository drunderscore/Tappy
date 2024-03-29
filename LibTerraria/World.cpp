/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <LibTerraria/Net/Types.h>
#include <LibTerraria/World.h>

static constexpr u8 additional_header_bit = 0b0000'0001;
static constexpr u8 block_bit = 0b0000'0010;
static constexpr u8 wall_bit = 0b0000'0100;
static constexpr u8 color_bit = 0b0000'1000;
static constexpr u8 extended_block_id_bit = 0b0010'0000;

static constexpr u8 red_wire_bit = 0b0000'0010;
static constexpr u8 blue_wire_bit = 0b0000'0100;
static constexpr u8 green_wire_bit = 0b0000'1000;
static constexpr u8 wall_color_bit = 0b0001'0000;
static constexpr u8 liquid_bits = 0b0001'1000;
static constexpr u8 liquid_shift = 3;

static constexpr u8 rle_bits = 0b1100'0000;
static constexpr u8 rle_shift = 6;

static constexpr u8 m_actuator_bit = 0b0000'0010;
static constexpr u8 m_actuated_bit = 0b0000'0100;
static constexpr u8 yellow_wire_bit = 0b0010'0000;
static constexpr u8 extended_wall_id_bit = 0b0100'0000;

template<typename T, size_t size>
InputStream& operator>>(InputStream& stream, Array<T, size>& array)
{
    for (auto i = 0; i < size; i++)
        stream >> array[i];
    return stream;
}

namespace Terraria
{
World::World(NonnullRefPtr<TileMap> tile_map) : m_tile_map(move(tile_map)) {}

ErrorOr<NonnullRefPtr<World>> World::try_load_world(InputStream& stream)
{
    i32 version;
    FileMetadata metadata;
    Header header;

    stream >> version;

    if (version != world_version_capable_of_loading)
        return Error::from_string_literal("Unable to load this world version");

    stream >> metadata;

    if (stream.handle_recoverable_error())
        return Error::from_string_literal("Unable to read world metadata");

    if (metadata.type() != FileMetadata::FileType::World)
        return Error::from_string_literal("File is not a world file");

    u16 temporary_16;
    u32 temporary_32;

    // TODO: Verify stream against these pointers
    stream >> temporary_16;
    Vector<i32> file_pointers;
    file_pointers.resize(temporary_16);
    for (auto i = 0; i < temporary_16; i++)
        stream >> file_pointers[i];

    stream >> temporary_16;
    Vector<bool> importance;
    importance.resize(temporary_16);
    // TODO: Understand what the game is doing here.
    {
        u8 b = 0;
        u8 b2 = 128;
        for (auto i = 0; i < temporary_16; i++)
        {
            if (b2 == 128)
            {
                stream >> b;
                b2 = 1;
            }
            else
            {
                b2 <<= 1;
            }

            if ((b & b2) == b2)
                importance[i] = true;
        }
    }

    Net::Types::read_string(stream, header.name);
    Net::Types::read_string(stream, header.seed);
    stream >> header.generator_version;

    Array<u8, 16> uuid;
    stream >> uuid;
    header.uuid = uuid;

    stream >> header.id;
    stream >> header.left;
    stream >> header.right;
    stream >> header.top;
    stream >> header.bottom;
    stream >> header.max_tiles_y;
    stream >> header.max_tiles_x;
    stream >> header.game_mode;
    stream >> header.drunk;
    stream >> header.get_good_world;
    stream >> header.tenth_anniversary;
    stream >> header.tenth_anniversary;
    stream >> header.dont_starve;
    stream >> header.creation_time;
    stream >> header.moon_type;
    stream >> header.tree_x;
    stream >> header.tree_style;
    stream >> header.cave_back_x;
    stream >> header.cave_back_style;
    stream >> header.ice_back_style;
    stream >> header.jungle_back_style;
    stream >> header.hell_back_style;
    stream >> header.spawn_tile;
    stream >> header.surface;
    stream >> header.rock_layer;
    stream >> header.time;
    stream >> header.day_time;
    stream >> header.moon_phase;
    stream >> header.blood_moon;
    stream >> header.eclipse;
    stream >> header.dungeon;
    stream >> header.crimson;
    stream >> header.downed_boss_1;
    stream >> header.downed_boss_2;
    stream >> header.downed_boss_3;
    stream >> header.downed_queen_bee;
    stream >> header.downed_mech_boss_1;
    stream >> header.downed_mech_boss_2;
    stream >> header.downed_mech_boss_3;
    stream >> header.downed_any_mech_boss;
    stream >> header.downed_plantera;
    stream >> header.downed_golem;
    stream >> header.downed_king_slime;
    stream >> header.saved_goblin;
    stream >> header.saved_wizard;
    stream >> header.saved_mech;
    stream >> header.downed_goblins;
    stream >> header.downed_clown;
    stream >> header.downed_frost;
    stream >> header.downed_pirates;
    stream >> header.shadow_orb_smashed;
    stream >> header.spawn_meteor;
    stream >> header.shadow_orb_count;
    stream >> header.altar_count;
    stream >> header.hard_mode;
    stream >> header.invasion_delay;
    stream >> header.invasion_size;
    stream >> header.invasion_type;
    stream >> header.invasion_x;
    stream >> header.slime_rain_time;
    stream >> header.sundial_cooldown;
    stream >> header.raining;
    stream >> header.rain_time;
    stream >> header.max_rain;
    stream >> header.cobalt_tier;
    stream >> header.mythril_tier;
    stream >> header.adamantite_tier;
    stream >> header.backgrounds;
    stream >> header.cloud_background_active;
    stream >> header.number_of_clouds;
    stream >> header.wind_speed_target;

    stream >> temporary_32;
    for (int i = 0; i < temporary_32; i++)
    {
        String value;
        Net::Types::read_string(stream, value);
        header.angler_who_finished_today.append(move(value));
    }

    stream >> header.saved_angler;
    stream >> header.angler_quest;
    stream >> header.saved_stylist;
    stream >> header.saved_tax_collector;
    stream >> header.saved_golfer;
    stream >> header.invasion_size_start;
    stream >> header.cultist_delay;

    stream >> temporary_16;
    for (int i = 0; i < temporary_16; i++)
    {
        i32 value;
        stream >> value;
        if (value != 0)
            header.kill_count.set(i, value);
    }

    stream >> header.fast_foward_time;
    stream >> header.downed_fishron;
    stream >> header.downed_martians;
    stream >> header.downed_ancient_cultists;
    stream >> header.downed_moonlord;
    stream >> header.downed_halloween_king;
    stream >> header.downed_halloween_tree;
    stream >> header.downed_christmas_ice_queen;
    stream >> header.downed_christmas_santank;
    stream >> header.downed_christmas_tree;
    stream >> header.downed_tower_solar;
    stream >> header.downed_tower_vortex;
    stream >> header.downed_tower_nebula;
    stream >> header.downed_tower_stardust;
    stream >> header.tower_active_solar;
    stream >> header.tower_active_vortex;
    stream >> header.tower_active_nebula;
    stream >> header.tower_active_stardust;
    stream >> header.lunar_apocalypse;
    stream >> header.party_manual;
    stream >> header.party_genuine;
    stream >> header.party_cooldown;

    stream >> temporary_32;
    for (int i = 0; i < temporary_32; i++)
    {
        i32 value;
        stream >> value;
        header.celebrating_npcs.append(value);
    }

    stream >> header.sandstorming;
    stream >> header.sandstorm_time_left;
    stream >> header.sandstorm_severity;
    stream >> header.sandstorm_intended_severity;
    stream >> header.saved_bartender;
    stream >> header.downed_dd2_1;
    stream >> header.downed_dd2_2;
    stream >> header.downed_dd2_3;
    stream >> header.additional_backgrounds;
    stream >> header.combat_book_was_used;
    stream >> header.lantern_night_cooldown;
    stream >> header.lantern_night_genuine;
    stream >> header.lantern_night_manual;
    stream >> header.lantern_night_next_is_genuine;

    stream >> temporary_32;
    for (int i = 0; i < temporary_32; i++)
    {
        i32 value;
        stream >> value;
        header.tree_tops.append(value);
    }

    stream >> header.force_halloween_for_today;
    stream >> header.force_christmas_for_today;
    stream >> header.copper_tier;
    stream >> header.iron_tier;
    stream >> header.silver_tier;
    stream >> header.gold_tier;
    stream >> header.bought_cat;
    stream >> header.bought_dog;
    stream >> header.bought_bunny;
    stream >> header.downed_empress_of_light;
    stream >> header.downed_queen_slime;

    auto tile_map = adopt_ref(*new MemoryTileMap(header.max_tiles_x, header.max_tiles_y));
    auto world = adopt_ref(*new World(move(tile_map)));
    world->m_header = move(header);

    u8 temporary_8;

    i16 rle = 0;
    Tile* tile_to_rle;
    for (u16 x = 0; x < world->m_header.max_tiles_x; x++)
    {
        for (u16 y = 0; y < world->m_header.max_tiles_y; y++)
        {
            auto& tile = world->m_tile_map->at(x, y);

            if (rle > 0)
            {
                tile = *tile_to_rle;

                rle--;
                continue;
            }

            Optional<u8> lower_wall_id;

            u8 header1;
            u8 header2 = 0;
            u8 header3 = 0;
            stream >> header1;

            if ((header1 & additional_header_bit) == additional_header_bit)
            {
                stream >> header2;
                if ((header2 & additional_header_bit) == additional_header_bit)
                    stream >> header3;
            }

            if ((header1 & block_bit) == block_bit)
            {
                if ((header1 & extended_block_id_bit))
                {
                    stream >> temporary_16;
                    tile.block() = Tile::Block(static_cast<Tile::Block::Id>(temporary_16));
                }
                else
                {
                    stream >> temporary_8;
                    tile.block() = Tile::Block(static_cast<Tile::Block::Id>(temporary_8));
                }

                if (importance[static_cast<i16>(tile.block()->id())])
                {
                    stream >> tile.block()->frame_x();
                    stream >> tile.block()->frame_y();

                    if (tile.block()->id() == Tile::Block::Id::Timers)
                        tile.block()->frame_y() = 0;
                }
            }

            if ((header3 & color_bit) == color_bit)
            {
                // TODO: Do something with this block color
                stream >> temporary_8;
            }

            if ((header1 & wall_bit) == wall_bit)
            {
                stream >> lower_wall_id;

                // TODO: Do something with this wall color
                if ((header3 & wall_color_bit) == wall_color_bit)
                    stream >> temporary_8;
            }

            temporary_8 = ((header1 & liquid_bits) >> liquid_shift);
            if (temporary_8 != 0)
            {
                tile.set_liquid(temporary_8);
                stream >> temporary_8;
                tile.set_liquid_amount(temporary_8);
            }

            if (lower_wall_id.has_value())
            {
                if ((header3 & extended_wall_id_bit) == extended_wall_id_bit)
                {
                    stream >> temporary_8;
                    tile.wall_id() = static_cast<Tile::WallId>((temporary_8 << 8) | *lower_wall_id);
                }
                else
                {
                    tile.wall_id() = static_cast<Tile::WallId>(*lower_wall_id);
                }
            }

            if ((header2 & red_wire_bit) == red_wire_bit)
                tile.set_red_wire(true);

            if ((header2 & blue_wire_bit) == blue_wire_bit)
                tile.set_blue_wire(true);

            if ((header2 & green_wire_bit) == green_wire_bit)
                tile.set_green_wire(true);

            if ((header3 & yellow_wire_bit) == yellow_wire_bit)
                tile.set_yellow_wire(true);

            if ((header3 & m_actuator_bit) == m_actuator_bit)
                tile.set_has_actuator(true);

            if ((header3 & m_actuated_bit) == m_actuated_bit)
                tile.set_is_actuated(true);

            u8 rle_type = (header1 & rle_bits) >> rle_shift;
            if (rle_type == 1)
            {
                stream >> temporary_8;
                rle = temporary_8;
                tile_to_rle = &tile;
            }
            else if (rle_type == 2)
            {
                stream >> rle;
                tile_to_rle = &tile;
            }
            else if (rle != 0)
            {
                VERIFY_NOT_REACHED();
            }
        }
    }

    // TODO: some other stuff like pointer validation? idk

    u16 total_chests;
    stream >> total_chests;
    u16 items_slots_in_chests;
    stream >> items_slots_in_chests;

    for (auto i = 0; i < total_chests; i++)
    {
        Chest chest;
        stream >> temporary_32;
        chest.position().set_x(temporary_32);

        stream >> temporary_32;
        chest.position().set_y(temporary_32);

        String name;
        Net::Types::read_string(stream, name);
        chest.set_name(move(name));

        for (auto j = 0; j < items_slots_in_chests; j++)
        {
            stream >> temporary_16;
            if (temporary_16 == 0)
                continue;

            Item item;
            item.set_stack(temporary_16);

            int item_id;
            stream >> item_id;
            item.set_id(static_cast<Item::Id>(item_id));

            stream >> temporary_8;
            item.set_prefix(static_cast<Item::Prefix>(temporary_8));

            chest.contents().set(j, move(item));
        }

        world->m_chests.set(i, move(chest));
    }

    u16 total_signs;
    stream >> total_signs;

    for (auto i = 0; i < total_signs; i++)
    {
        Sign sign;
        String text;
        Net::Types::read_string(stream, text);
        sign.set_text(move(text));

        stream >> temporary_32;
        sign.position().set_x(temporary_32);
        stream >> temporary_32;
        sign.position().set_y(temporary_32);

        world->m_signs.set(i, move(sign));
    }

    return {world};
}
}