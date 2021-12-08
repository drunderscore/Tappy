/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/HashMap.h>
#include <AK/RefCounted.h>
#include <AK/Result.h>
#include <AK/Stream.h>
#include <AK/String.h>
#include <AK/UUID.h>
#include <AK/Vector.h>
#include <LibTerraria/Chest.h>
#include <LibTerraria/FileMetadata.h>
#include <LibTerraria/GameMode.h>
#include <LibTerraria/Point.h>
#include <LibTerraria/Sign.h>
#include <LibTerraria/TileMap.h>

namespace Terraria
{
class World : public RefCounted<World>
{
public:
    struct Header
    {
        String name;
        String seed;
        UUID uuid;
        u64 generator_version{};
        i32 id{};
        i32 left{};
        i32 right{};
        i32 top{};
        i32 bottom{};
        i32 max_tiles_y{};
        i32 max_tiles_x{};
        i32 game_mode{};
        bool drunk{};
        // FIXME: What is this?
        bool get_good_world{};
        bool tenth_anniversary{};
        bool dont_starve{};
        bool not_the_bees{};
        // FIXME: Specialize this type for proper time display
        i64 creation_time{};
        u8 moon_type{};
        Array<i32, 3> tree_x;
        Array<i32, 4> tree_style;
        Array<i32, 3> cave_back_x;
        Array<i32, 4> cave_back_style;
        i32 ice_back_style{};
        i32 jungle_back_style{};
        i32 hell_back_style{};
        Point<i32> spawn_tile{};
        double surface{};
        double rock_layer{};
        double time{};
        bool day_time{};
        i32 moon_phase{};
        bool blood_moon{};
        bool eclipse{};
        Point<i32> dungeon{};
        bool crimson{};
        // TODO: Magic values
        bool downed_boss_1{};
        bool downed_boss_2{};
        bool downed_boss_3{};
        bool downed_queen_bee{};
        bool downed_mech_boss_1{};
        bool downed_mech_boss_2{};
        bool downed_mech_boss_3{};
        bool downed_any_mech_boss{};
        bool downed_plantera{};
        bool downed_golem{};
        bool downed_king_slime{};
        bool saved_goblin{};
        bool saved_wizard{};
        bool saved_mech{};
        bool downed_goblins{};
        bool downed_clown{};
        bool downed_frost{};
        bool downed_pirates{};
        bool shadow_orb_smashed{};
        bool spawn_meteor{};
        u8 shadow_orb_count{};
        i32 altar_count{};
        bool hard_mode{};
        i32 invasion_delay{};
        i32 invasion_size{};
        i32 invasion_type{};
        double invasion_x{};
        double slime_rain_time{};
        u8 sundial_cooldown{};
        bool raining{};
        i32 rain_time{};
        float max_rain{};
        i32 cobalt_tier{};
        i32 mythril_tier{};
        i32 adamantite_tier{};
        Array<u8, 8> backgrounds;
        i32 cloud_background_active{};
        i16 number_of_clouds{};
        float wind_speed_target{};
        Vector<String> angler_who_finished_today;
        bool saved_angler{};
        i32 angler_quest{};
        bool saved_stylist{};
        bool saved_tax_collector{};
        bool saved_golfer{};
        i32 invasion_size_start{};
        i32 cultist_delay{};
        // FIXME: Magic value
        HashMap<i16, i32> kill_count;
        bool fast_foward_time{};
        bool downed_fishron{};
        bool downed_martians{};
        bool downed_ancient_cultists{};
        bool downed_moonlord{};
        bool downed_halloween_king{};
        bool downed_halloween_tree{};
        bool downed_christmas_ice_queen{};
        bool downed_christmas_santank{};
        bool downed_christmas_tree{};
        bool downed_tower_solar{};
        bool downed_tower_vortex{};
        bool downed_tower_nebula{};
        bool downed_tower_stardust{};
        bool tower_active_solar{};
        bool tower_active_vortex{};
        bool tower_active_nebula{};
        bool tower_active_stardust{};
        bool lunar_apocalypse{};
        bool party_manual{};
        bool party_genuine{};
        i32 party_cooldown{};
        // FIXME: Magic value
        Vector<i32> celebrating_npcs;
        bool sandstorming{};
        i32 sandstorm_time_left{};
        float sandstorm_severity{};
        float sandstorm_intended_severity{};
        bool saved_bartender{};
        bool downed_dd2_1{};
        bool downed_dd2_2{};
        bool downed_dd2_3{};
        Array<u8, 5> additional_backgrounds;
        bool combat_book_was_used{};
        i32 lantern_night_cooldown{};
        bool lantern_night_genuine{};
        bool lantern_night_manual{};
        bool lantern_night_next_is_genuine{};
        Vector<i32> tree_tops;
        bool force_halloween_for_today{};
        bool force_christmas_for_today{};
        i32 copper_tier{};
        i32 iron_tier{};
        i32 silver_tier{};
        i32 gold_tier{};
        bool bought_cat{};
        bool bought_dog{};
        bool bought_bunny{};
        bool downed_empress_of_light{};
        bool downed_queen_slime{};
    };

    static constexpr i32 world_version_capable_of_loading = 244;

    World(NonnullRefPtr<TileMap>);

    static Result<NonnullRefPtr<World>, String> try_load_world(InputStream& stream);

    Header& header() { return m_header; }

    const Header& header() const { return m_header; }

    const NonnullRefPtr<TileMap> tile_map() const { return m_tile_map; }

    NonnullRefPtr<TileMap> tile_map() { return m_tile_map; }

    const HashMap<u16, Chest>& chests() const { return m_chests; }

    HashMap<u16, Chest>& chests() { return m_chests; }

    const HashMap<u16, Sign>& signs() const { return m_signs; }

    HashMap<u16, Sign>& signs() { return m_signs; }

private:
    i32 m_version{};
    FileMetadata m_metadata;
    Header m_header;
    HashMap<u16, Chest> m_chests;
    HashMap<u16, Sign> m_signs;
    NonnullRefPtr<TileMap> m_tile_map;
};
}