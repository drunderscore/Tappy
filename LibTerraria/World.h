/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <LibTerraria/FileMetadata.h>
#include <AK/Result.h>
#include <AK/String.h>
#include <AK/Stream.h>
#include <AK/Vector.h>
#include <AK/UUID.h>
#include <LibTerraria/GameMode.h>
#include <LibTerraria/Point.h>
#include <LibTerraria/TileMap.h>
#include <LibTerraria/Chest.h>
#include <AK/RefCounted.h>
#include <AK/HashMap.h>

namespace Terraria
{
class World : public RefCounted<World>
{
public:
    using Error = String;

    static Result<RefPtr<World>, Error> try_load_world(InputStream& stream);

    const String& name() const
    { return m_name; }

    const RefPtr<TileMap> tile_map() const
    { return m_tile_map; }

    RefPtr<TileMap> tile_map()
    { return m_tile_map; }

    const HashMap<u16, Chest>& chests() const
    { return m_chests; }

    HashMap<u16, Chest>& chests()
    { return m_chests; }

public:
    World() = default;

    i32 m_version{};
    FileMetadata m_metadata;
    String m_name;
    String m_seed;
    AK::UUID m_uuid;
    u64 m_generator_version{};
    i32 m_id{};
    i32 m_left{};
    i32 m_right{};
    i32 m_top{};
    i32 m_bottom{};
    i32 m_max_tiles_y{};
    i32 m_max_tiles_x{};
    i32 m_game_mode{};
    bool m_drunk{};
    // FIXME: What is this?
    bool m_get_good_world{};
    bool m_tenth_anniversary{};
    // FIXME: Specialize this type for proper time display
    i64 m_creation_time{};
    u8 m_moon_type{};
    AK::Array<i32, 3> m_tree_x;
    AK::Array<i32, 4> m_tree_style;
    AK::Array<i32, 3> m_cave_back_x;
    AK::Array<i32, 4> m_cave_back_style;
    i32 m_ice_back_style{};
    i32 m_jungle_back_style{};
    i32 m_hell_back_style{};
    Point<i32> m_spawn_tile{};
    double m_surface{};
    double m_rock_layer{};
    double m_time{};
    bool m_day_time{};
    i32 m_moon_phase{};
    bool m_blood_moon{};
    bool m_eclipse{};
    Point<i32> m_dungeon{};
    bool m_crimson{};
    // TODO: Magic values
    bool m_downed_boss_1{};
    bool m_downed_boss_2{};
    bool m_downed_boss_3{};
    bool m_downed_queen_bee{};
    bool m_downed_mech_boss_1{};
    bool m_downed_mech_boss_2{};
    bool m_downed_mech_boss_3{};
    bool m_downed_any_mech_boss{};
    bool m_downed_plantera{};
    bool m_downed_golem{};
    bool m_downed_king_slime{};
    bool m_saved_goblin{};
    bool m_saved_wizard{};
    bool m_saved_mech{};
    bool m_downed_goblins{};
    bool m_downed_clown{};
    bool m_downed_frost{};
    bool m_downed_pirates{};
    bool m_shadow_orb_smashed{};
    bool m_spawn_meteor{};
    u8 m_shadow_orb_count{};
    i32 m_altar_count{};
    bool m_hard_mode{};
    i32 m_invasion_delay{};
    i32 m_invasion_size{};
    i32 m_invasion_type{};
    double m_invasion_x{};
    double m_slime_rain_time{};
    u8 m_sundial_cooldown{};
    bool m_raining{};
    i32 m_rain_time{};
    float m_max_rain{};
    i32 m_cobalt_tier{};
    i32 m_mythril_tier{};
    i32 m_adamantite_tier{};
    Array<u8, 8> m_backgrounds;
    i32 m_cloud_background_active{};
    i16 m_number_of_clouds{};
    float m_wind_speed_target{};
    Vector<String> m_angler_who_finished_today;
    bool m_saved_angler{};
    i32 m_angler_quest{};
    bool m_saved_stylist{};
    bool m_saved_tax_collector{};
    bool m_saved_golfer{};
    i32 m_invasion_size_start{};
    i32 m_cultist_delay{};
    // FIXME: Magic value
    HashMap<i16, i32> m_kill_count;
    bool m_fast_foward_time{};
    bool m_downed_fishron{};
    bool m_downed_martians{};
    bool m_downed_ancient_cultists{};
    bool m_downed_moonlord{};
    bool m_downed_halloween_king{};
    bool m_downed_halloween_tree{};
    bool m_downed_christmas_ice_queen{};
    bool m_downed_christmas_santank{};
    bool m_downed_christmas_tree{};
    bool m_downed_tower_solar{};
    bool m_downed_tower_vortex{};
    bool m_downed_tower_nebula{};
    bool m_downed_tower_stardust{};
    bool m_tower_active_solar{};
    bool m_tower_active_vortex{};
    bool m_tower_active_nebula{};
    bool m_tower_active_stardust{};
    bool m_lunar_apocalypse{};
    bool m_party_manual{};
    bool m_party_genuine{};
    i32 m_party_cooldown{};
    // FIXME: Magic value
    Vector<i32> m_celebrating_npcs;
    bool m_sandstorming{};
    i32 m_sandstorm_time_left{};
    float m_sandstorm_severity{};
    float m_sandstorm_intended_severity{};
    bool m_saved_bartender{};
    bool m_downed_dd2_1{};
    bool m_downed_dd2_2{};
    bool m_downed_dd2_3{};
    Array<u8, 5> m_additional_backgrounds;
    bool m_combat_book_was_used{};
    i32 m_lantern_night_cooldown{};
    bool m_lantern_night_genuine{};
    bool m_lantern_night_manual{};
    bool m_lantern_night_next_is_genuine{};
    Vector<i32> m_tree_tops;
    bool m_force_halloween_for_today{};
    bool m_force_christmas_for_today{};
    i32 m_copper_tier{};
    i32 m_iron_tier{};
    i32 m_silver_tier{};
    i32 m_gold_tier{};
    bool m_bought_cat{};
    bool m_bought_dog{};
    bool m_bought_bunny{};
    bool m_downed_empress_of_light{};
    bool m_downed_queen_slime{};

    HashMap<u16, Chest> m_chests;

    RefPtr<MemoryTileMap> m_tile_map;
};
}