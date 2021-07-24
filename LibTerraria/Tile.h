/*
 * Copyright (c) 2021, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <AK/Optional.h>

namespace Terraria
{
// This class should contain light and simple members, we're going to have a lot of these.
class [[gnu::packed]] Tile
{
public:
    Tile() = default;

    struct PackedFrames
    {
        i16 x{};
        i16 y{};
    };
    enum class WallId : u16
    {
        // TODO: Wall IDs
    };

    class Block
    {
    public:
        enum class Id : u16
        {
            Dirt = 0,
            Stone = 1,
            Grass = 2,
            Plants = 3,
            Torches = 4,
            Trees = 5,
            Iron = 6,
            Copper = 7,
            Gold = 8,
            Silver = 9,
            ClosedDoor = 10,
            OpenDoor = 11,
            Heart = 12,
            Bottles = 13,
            Tables = 14,
            Chairs = 15,
            Anvils = 16,
            Furnaces = 17,
            WorkBenches = 18,
            Platforms = 19,
            Saplings = 20,
            Containers = 21,
            Demonite = 22,
            CorruptGrass = 23,
            CorruptPlants = 24,
            Ebonstone = 25,
            DemonAltar = 26,
            Sunflower = 27,
            Pots = 28,
            PiggyBank = 29,
            WoodBlock = 30,
            ShadowOrbs = 31,
            CorruptThorns = 32,
            Candles = 33,
            Chandeliers = 34,
            Jackolanterns = 35,
            Presents = 36,
            Meteorite = 37,
            GrayBrick = 38,
            RedBrick = 39,
            ClayBlock = 40,
            BlueDungeonBrick = 41,
            HangingLanterns = 42,
            GreenDungeonBrick = 43,
            PinkDungeonBrick = 44,
            GoldBrick = 45,
            SilverBrick = 46,
            CopperBrick = 47,
            Spikes = 48,
            WaterCandle = 49,
            Books = 50,
            Cobweb = 51,
            Vines = 52,
            Sand = 53,
            Glass = 54,
            Signs = 55,
            Obsidian = 56,
            Ash = 57,
            Hellstone = 58,
            Mud = 59,
            JungleGrass = 60,
            JunglePlants = 61,
            JungleVines = 62,
            Sapphire = 63,
            Ruby = 64,
            Emerald = 65,
            Topaz = 66,
            Amethyst = 67,
            Diamond = 68,
            JungleThorns = 69,
            MushroomGrass = 70,
            MushroomPlants = 71,
            MushroomTrees = 72,
            Plants2 = 73,
            JunglePlants2 = 74,
            ObsidianBrick = 75,
            HellstoneBrick = 76,
            Hellforge = 77,
            ClayPot = 78,
            Beds = 79,
            Cactus = 80,
            Coral = 81,
            ImmatureHerbs = 82,
            MatureHerbs = 83,
            BloomingHerbs = 84,
            Tombstones = 85,
            Loom = 86,
            Pianos = 87,
            Dressers = 88,
            Benches = 89,
            Bathtubs = 90,
            Banners = 91,
            Lampposts = 92,
            Lamps = 93,
            Kegs = 94,
            ChineseLanterns = 95,
            CookingPots = 96,
            Safes = 97,
            SkullLanterns = 98,
            TrashCan = 99,
            Candelabras = 100,
            Bookcases = 101,
            Thrones = 102,
            Bowls = 103,
            GrandfatherClocks = 104,
            Statues = 105,
            Sawmill = 106,
            Cobalt = 107,
            Mythril = 108,
            HallowedGrass = 109,
            HallowedPlants = 110,
            Adamantite = 111,
            Ebonsand = 112,
            HallowedPlants2 = 113,
            TinkerersWorkbench = 114,
            HallowedVines = 115,
            Pearlsand = 116,
            Pearlstone = 117,
            PearlstoneBrick = 118,
            IridescentBrick = 119,
            Mudstone = 120,
            CobaltBrick = 121,
            MythrilBrick = 122,
            Silt = 123,
            WoodenBeam = 124,
            CrystalBall = 125,
            DiscoBall = 126,
            MagicalIceBlock = 127,
            Mannequin = 128,
            Crystals = 129,
            ActiveStoneBlock = 130,
            InactiveStoneBlock = 131,
            Lever = 132,
            AdamantiteForge = 133,
            MythrilAnvil = 134,
            PressurePlates = 135,
            Switches = 136,
            Traps = 137,
            Boulder = 138,
            MusicBoxes = 139,
            DemoniteBrick = 140,
            Explosives = 141,
            InletPump = 142,
            OutletPump = 143,
            Timers = 144,
            CandyCaneBlock = 145,
            GreenCandyCaneBlock = 146,
            SnowBlock = 147,
            SnowBrick = 148,
            HolidayLights = 149,
            AdamantiteBeam = 150,
            SandstoneBrick = 151,
            EbonstoneBrick = 152,
            RedStucco = 153,
            YellowStucco = 154,
            GreenStucco = 155,
            GrayStucco = 156,
            Ebonwood = 157,
            RichMahogany = 158,
            Pearlwood = 159,
            RainbowBrick = 160,
            IceBlock = 161,
            BreakableIce = 162,
            CorruptIce = 163,
            HallowedIce = 164,
            Stalactite = 165,
            Tin = 166,
            Lead = 167,
            Tungsten = 168,
            Platinum = 169,
            PineTree = 170,
            ChristmasTree = 171,
            Sinks = 172,
            PlatinumCandelabra = 173,
            PlatinumCandle = 174,
            TinBrick = 175,
            TungstenBrick = 176,
            PlatinumBrick = 177,
            ExposedGems = 178,
            GreenMoss = 179,
            BrownMoss = 180,
            RedMoss = 181,
            BlueMoss = 182,
            PurpleMoss = 183,
            LongMoss = 184,
            SmallPiles = 185,
            LargePiles = 186,
            LargePiles2 = 187,
            CactusBlock = 188,
            Cloud = 189,
            MushroomBlock = 190,
            LivingWood = 191,
            LeafBlock = 192,
            SlimeBlock = 193,
            BoneBlock = 194,
            FleshBlock = 195,
            RainCloud = 196,
            FrozenSlimeBlock = 197,
            Asphalt = 198,
            CrimsonGrass = 199,
            FleshIce = 200,
            CrimsonPlants = 201,
            Sunplate = 202,
            Crimstone = 203,
            Crimtane = 204,
            CrimsonVines = 205,
            IceBrick = 206,
            WaterFountain = 207,
            Shadewood = 208,
            Cannon = 209,
            LandMine = 210,
            Chlorophyte = 211,
            SnowballLauncher = 212,
            Rope = 213,
            Chain = 214,
            Campfire = 215,
            Firework = 216,
            Blendomatic = 217,
            MeatGrinder = 218,
            Extractinator = 219,
            Solidifier = 220,
            Palladium = 221,
            Orichalcum = 222,
            Titanium = 223,
            Slush = 224,
            Hive = 225,
            LihzahrdBrick = 226,
            DyePlants = 227,
            DyeVat = 228,
            HoneyBlock = 229,
            CrispyHoneyBlock = 230,
            Larva = 231,
            WoodenSpikes = 232,
            PlantDetritus = 233,
            Crimsand = 234,
            Teleporter = 235,
            LifeFruit = 236,
            LihzahrdAltar = 237,
            PlanteraBulb = 238,
            MetalBars = 239,
            Painting3X3 = 240,
            Painting4X3 = 241,
            Painting6X4 = 242,
            ImbuingStation = 243,
            BubbleMachine = 244,
            Painting2X3 = 245,
            Painting3X2 = 246,
            Autohammer = 247,
            PalladiumColumn = 248,
            BubblegumBlock = 249,
            Titanstone = 250,
            PumpkinBlock = 251,
            HayBlock = 252,
            SpookyWood = 253,
            Pumpkins = 254,
            AmethystGemsparkOff = 255,
            TopazGemsparkOff = 256,
            SapphireGemsparkOff = 257,
            EmeraldGemsparkOff = 258,
            RubyGemsparkOff = 259,
            DiamondGemsparkOff = 260,
            AmberGemsparkOff = 261,
            AmethystGemspark = 262,
            TopazGemspark = 263,
            SapphireGemspark = 264,
            EmeraldGemspark = 265,
            RubyGemspark = 266,
            DiamondGemspark = 267,
            AmberGemspark = 268,
            Womannequin = 269,
            FireflyinaBottle = 270,
            LightningBuginaBottle = 271,
            Cog = 272,
            StoneSlab = 273,
            SandStoneSlab = 274,
            BunnyCage = 275,
            SquirrelCage = 276,
            MallardDuckCage = 277,
            DuckCage = 278,
            BirdCage = 279,
            BlueJay = 280,
            CardinalCage = 281,
            FishBowl = 282,
            HeavyWorkBench = 283,
            CopperPlating = 284,
            SnailCage = 285,
            GlowingSnailCage = 286,
            AmmoBox = 287,
            MonarchButterflyJar = 288,
            PurpleEmperorButterflyJar = 289,
            RedAdmiralButterflyJar = 290,
            UlyssesButterflyJar = 291,
            SulphurButterflyJar = 292,
            TreeNymphButterflyJar = 293,
            ZebraSwallowtailButterflyJar = 294,
            JuliaButterflyJar = 295,
            ScorpionCage = 296,
            BlackScorpionCage = 297,
            FrogCage = 298,
            MouseCage = 299,
            BoneWelder = 300,
            FleshCloningVat = 301,
            GlassKiln = 302,
            LihzahrdFurnace = 303,
            LivingLoom = 304,
            SkyMill = 305,
            IceMachine = 306,
            SteampunkBoiler = 307,
            HoneyDispenser = 308,
            PenguinCage = 309,
            WormCage = 310,
            DynastyWood = 311,
            RedDynastyShingles = 312,
            BlueDynastyShingles = 313,
            MinecartTrack = 314,
            Coralstone = 315,
            BlueJellyfishBowl = 316,
            GreenJellyfishBowl = 317,
            PinkJellyfishBowl = 318,
            ShipInABottle = 319,
            SeaweedPlanter = 320,
            BorealWood = 321,
            PalmWood = 322,
            PalmTree = 323,
            BeachPiles = 324,
            TinPlating = 325,
            Waterfall = 326,
            Lavafall = 327,
            Confetti = 328,
            ConfettiBlack = 329,
            CopperCoinPile = 330,
            SilverCoinPile = 331,
            GoldCoinPile = 332,
            PlatinumCoinPile = 333,
            WeaponsRack = 334,
            FireworksBox = 335,
            LivingFire = 336,
            AlphabetStatues = 337,
            FireworkFountain = 338,
            GrasshopperCage = 339,
            LivingCursedFire = 340,
            LivingDemonFire = 341,
            LivingFrostFire = 342,
            LivingIchor = 343,
            LivingUltrabrightFire = 344,
            Honeyfall = 345,
            ChlorophyteBrick = 346,
            CrimtaneBrick = 347,
            ShroomitePlating = 348,
            MushroomStatue = 349,
            MartianConduitPlating = 350,
            ChimneySmoke = 351,
            CrimsonThorns = 352,
            VineRope = 353,
            BewitchingTable = 354,
            AlchemyTable = 355,
            Sundial = 356,
            MarbleBlock = 357,
            GoldBirdCage = 358,
            GoldBunnyCage = 359,
            GoldButterflyCage = 360,
            GoldFrogCage = 361,
            GoldGrasshopperCage = 362,
            GoldMouseCage = 363,
            GoldWormCage = 364,
            SilkRope = 365,
            WebRope = 366,
            Marble = 367,
            Granite = 368,
            GraniteBlock = 369,
            MeteoriteBrick = 370,
            PinkSlimeBlock = 371,
            PeaceCandle = 372,
            WaterDrip = 373,
            LavaDrip = 374,
            HoneyDrip = 375,
            FishingCrate = 376,
            SharpeningStation = 377,
            TargetDummy = 378,
            Bubble = 379,
            PlanterBox = 380,
            LavaMoss = 381,
            VineFlowers = 382,
            LivingMahogany = 383,
            LivingMahoganyLeaves = 384,
            CrystalBlock = 385,
            TrapdoorOpen = 386,
            TrapdoorClosed = 387,
            TallGateClosed = 388,
            TallGateOpen = 389,
            LavaLamp = 390,
            CageEnchantedNightcrawler = 391,
            CageBuggy = 392,
            CageGrubby = 393,
            CageSluggy = 394,
            ItemFrame = 395,
            Sandstone = 396,
            HardenedSand = 397,
            CorruptHardenedSand = 398,
            CrimsonHardenedSand = 399,
            CorruptSandstone = 400,
            CrimsonSandstone = 401,
            HallowHardenedSand = 402,
            HallowSandstone = 403,
            DesertFossil = 404,
            Fireplace = 405,
            Chimney = 406,
            FossilOre = 407,
            LunarOre = 408,
            LunarBrick = 409,
            LunarMonolith = 410,
            Detonator = 411,
            LunarCraftingStation = 412,
            SquirrelOrangeCage = 413,
            SquirrelGoldCage = 414,
            LunarBlockSolar = 415,
            LunarBlockVortex = 416,
            LunarBlockNebula = 417,
            LunarBlockStardust = 418,
            LogicGateLamp = 419,
            LogicGate = 420,
            ConveyorBeltLeft = 421,
            ConveyorBeltRight = 422,
            LogicSensor = 423,
            WirePipe = 424,
            AnnouncementBox = 425,
            TeamBlockRed = 426,
            TeamBlockRedPlatform = 427,
            WeightedPressurePlate = 428,
            WireBulb = 429,
            TeamBlockGreen = 430,
            TeamBlockBlue = 431,
            TeamBlockYellow = 432,
            TeamBlockPink = 433,
            TeamBlockWhite = 434,
            TeamBlockGreenPlatform = 435,
            TeamBlockBluePlatform = 436,
            TeamBlockYellowPlatform = 437,
            TeamBlockPinkPlatform = 438,
            TeamBlockWhitePlatform = 439,
            GemLocks = 440,
            FakeContainers = 441,
            ProjectilePressurePad = 442,
            GeyserTrap = 443,
            BeeHive = 444,
            PixelBox = 445,
            SillyBalloonPink = 446,
            SillyBalloonPurple = 447,
            SillyBalloonGreen = 448,
            SillyStreamerBlue = 449,
            SillyStreamerGreen = 450,
            SillyStreamerPink = 451,
            SillyBalloonMachine = 452,
            SillyBalloonTile = 453,
            Pigronata = 454,
            PartyMonolith = 455,
            PartyBundleOfBalloonTile = 456,
            PartyPresent = 457,
            SandFallBlock = 458,
            SnowFallBlock = 459,
            SnowCloud = 460,
            SandDrip = 461,
            DjinnLamp = 462,
            DefendersForge = 463,
            WarTable = 464,
            WarTableBanner = 465,
            ElderCrystalStand = 466,
            Containers2 = 467,
            FakeContainers2 = 468,
            Tables2 = 469,
            DisplayDoll = 470,
            WeaponsRack2 = 471,
            IronBrick = 472,
            LeadBrick = 473,
            LesionBlock = 474,
            HatRack = 475,
            GolfHole = 476,
            GolfGrass = 477,
            CrimstoneBrick = 478,
            SmoothSandstone = 479,
            BloodMoonMonolith = 480,
            CrackedBlueDungeonBrick = 481,
            CrackedGreenDungeonBrick = 482,
            CrackedPinkDungeonBrick = 483,
            RollingCactus = 484,
            AntlionLarva = 485,
            DrumSet = 486,
            PicnicTable = 487,
            FallenLog = 488,
            PinWheel = 489,
            WeatherVane = 490,
            VoidVault = 491,
            GolfGrassHallowed = 492,
            GolfCupFlag = 493,
            GolfTee = 494,
            ShellPile = 495,
            AntiPortalBlock = 496,
            Toilets = 497,
            Spider = 498,
            LesionStation = 499,
            SolarBrick = 500,
            VortexBrick = 501,
            NebulaBrick = 502,
            StardustBrick = 503,
            MysticSnakeRope = 504,
            GoldGoldfishBowl = 505,
            CatBast = 506,
            GoldStarryGlassBlock = 507,
            BlueStarryGlassBlock = 508,
            VoidMonolith = 509,
            ArrowSign = 510,
            PaintedArrowSign = 511,
            GreenMossBrick = 512,
            BrownMossBrick = 513,
            RedMossBrick = 514,
            BlueMossBrick = 515,
            PurpleMossBrick = 516,
            LavaMossBrick = 517,
            LilyPad = 518,
            Cattail = 519,
            FoodPlatter = 520,
            BlackDragonflyJar = 521,
            BlueDragonflyJar = 522,
            GreenDragonflyJar = 523,
            OrangeDragonflyJar = 524,
            RedDragonflyJar = 525,
            YellowDragonflyJar = 526,
            GoldDragonflyJar = 527,
            MushroomVines = 528,
            SeaOats = 529,
            OasisPlants = 530,
            BoulderStatue = 531,
            MaggotCage = 532,
            RatCage = 533,
            KryptonMoss = 534,
            KryptonMossBrick = 535,
            XenonMoss = 536,
            XenonMossBrick = 537,
            LadybugCage = 538,
            ArgonMoss = 539,
            ArgonMossBrick = 540,
            EchoBlock = 541,
            OwlCage = 542,
            PupfishBowl = 543,
            GoldLadybugCage = 544,
            LawnFlamingo = 545,
            Grate = 546,
            PottedPlants1 = 547,
            PottedPlants2 = 548,
            Seaweed = 549,
            TurtleCage = 550,
            TurtleJungleCage = 551,
            Sandcastles = 552,
            GrebeCage = 553,
            SeagullCage = 554,
            WaterStriderCage = 555,
            GoldWaterStriderCage = 556,
            GrateClosed = 557,
            SeahorseCage = 558,
            GoldSeahorseCage = 559,
            GolfTrophies = 560,
            MarbleColumn = 561,
            BambooBlock = 562,
            LargeBambooBlock = 563,
            PlasmaLamp = 564,
            FogMachine = 565,
            AmberStoneBlock = 566,
            GardenGnome = 567,
            PinkFairyJar = 568,
            GreenFairyJar = 569,
            BlueFairyJar = 570,
            Bamboo = 571,
            SoulBottles = 572,
            TatteredWoodSign = 573,
            BorealBeam = 574,
            RichMahoganyBeam = 575,
            GraniteColumn = 576,
            SandstoneColumn = 577,
            MushroomBeam = 578,
            RockGolemHead = 579,
            HellButterflyJar = 580,
            LavaflyinaBottle = 581,
            MagmaSnailCage = 582,
            TreeTopaz = 583,
            TreeAmethyst = 584,
            TreeSapphire = 585,
            TreeEmerald = 586,
            TreeRuby = 587,
            TreeDiamond = 588,
            TreeAmber = 589,
            GemSaplings = 590,
            PotsSuspended = 591,
            BrazierSuspended = 592,
            VolcanoSmall = 593,
            VolcanoLarge = 594,
            VanityTreeSakuraSaplings = 595,
            VanityTreeSakura = 596,
            TeleportationPylon = 597,
            LavafishBowl = 598,
            AmethystBunnyCage = 599,
            TopazBunnyCage = 600,
            SapphireBunnyCage = 601,
            EmeraldBunnyCage = 602,
            RubyBunnyCage = 603,
            DiamondBunnyCage = 604,
            AmberBunnyCage = 605,
            AmethystSquirrelCage = 606,
            TopazSquirrelCage = 607,
            SapphireSquirrelCage = 608,
            EmeraldSquirrelCage = 609,
            RubySquirrelCage = 610,
            DiamondSquirrelCage = 611,
            AmberSquirrelCage = 612,
            PottedLavaPlants = 613,
            PottedLavaPlantTendrils = 614,
            VanityTreeWillowSaplings = 615,
            VanityTreeYellowWillow = 616,
            MasterTrophyBase = 617,
            AccentSlab = 618,
            TruffleWormCage = 619,
            EmpressButterflyJar = 620,
            SliceOfCake = 621,
            TeaKettle = 622,
            PottedCrystalPlants = 623,
        };

        explicit Block(Id id) : m_id(id)
        {}

        Block(Id id, i16 frame_x, i16 frame_y) : m_id(id), m_frame_x(frame_x), m_frame_y(frame_y)
        {}

        Id id() const
        { return m_id; }

        void set_id(Id value)
        { m_id = value; }

        const Optional<i16>& frame_x() const
        { return m_frame_x; }

        const Optional<i16>& frame_y() const
        { return m_frame_y; }

        Optional<i16>& frame_x()
        { return m_frame_x; }

        Optional<i16>& frame_y()
        { return m_frame_y; }

        u8 shape() const
        { return m_shape; }

        void set_shape(u8 value)
        { m_shape = value; }

        /* ALWAYS_INLINE */ static Optional<PackedFrames>
        frame_for_block(const Tile& the_tile, const Tile& top, const Tile& bottom, const Tile& left, const Tile& right);

    private:
        Id m_id;
        Optional<i16> m_frame_x;
        Optional<i16> m_frame_y;
        u8 m_shape{};
    };

    Tile(Block block) : m_block(move(block))
    {}

    const Optional<Block>& block() const
    { return m_block; }

    Optional<Block>& block()
    { return m_block; }

    const Optional<WallId>& wall_id() const
    { return m_wall_id; }

    Optional<WallId>& wall_id()
    { return m_wall_id; }

    bool has_red_wire() const
    { return (m_flags & m_red_wire_bit) == m_red_wire_bit; }

    bool has_blue_wire() const
    { return (m_flags & m_blue_wire_bit) == m_blue_wire_bit; }

    bool has_green_wire() const
    { return (m_flags & m_green_wire_bit) == m_green_wire_bit; }

    bool has_yellow_wire() const
    { return (m_flags & m_yellow_wire_bit) == m_yellow_wire_bit; }

    bool has_actuator() const
    { return (m_flags & m_actuator_bit) == m_actuator_bit; }

    bool is_actuated() const
    { return (m_flags & m_actuated_bit) == m_actuated_bit; }

    void set_red_wire(bool value)
    {
        if (value)
            m_flags |= m_red_wire_bit;
        else
            m_flags &= ~m_red_wire_bit;
    }

    void set_blue_wire(bool value)
    {
        if (value)
            m_flags |= m_blue_wire_bit;
        else
            m_flags &= ~m_blue_wire_bit;
    }

    void set_green_wire(bool value)
    {
        if (value)
            m_flags |= m_green_wire_bit;
        else
            m_flags &= ~m_green_wire_bit;
    }

    void set_yellow_wire(bool value)
    {
        if (value)
            m_flags |= m_yellow_wire_bit;
        else
            m_flags &= ~m_yellow_wire_bit;
    }

    void set_has_actuator(bool value)
    {
        if (value)
            m_flags |= m_actuator_bit;
        else
            m_flags &= ~m_actuator_bit;
    }

    void set_is_actuated(bool value)
    {
        if (value)
            m_flags |= m_actuated_bit;
        else
            m_flags &= ~m_actuated_bit;
    }

    // FIXME: Magic value
    // _technically_ don't need the mask as it's at the end and all the other bytes fall off after shifting...
    u8 liquid() const
    { return (m_flags & m_liquid_bits) >> m_liquid_shift; }

    void set_liquid(u8 value)
    {
        // Remove the previous bits
        m_flags &= ~m_liquid_bits;
        m_flags |= ((value << m_liquid_shift) & m_liquid_bits);
    }

    u8 liquid_amount() const
    { return m_liquid_amount; }

    void set_liquid_amount(u8 value)
    { m_liquid_amount = value; }

    /* ALWAYS_INLINE */ static PackedFrames frames_for_wire(bool top, bool bottom, bool left, bool right);

    static constexpr i16 frame_x_for_style(i16 style)
    {
        return style * 18;
    }

    static constexpr i16 frame_y_for_style(i16 style)
    {
        return style * 22;
    }

private:
    u8 m_flags{};
    u8 m_liquid_amount{};
    Optional<Block> m_block;
    Optional<WallId> m_wall_id;
    // TODO: Liquid

    static constexpr u8 m_red_wire_bit = 0b0000'0001;
    static constexpr u8 m_blue_wire_bit = 0b0000'0010;
    static constexpr u8 m_green_wire_bit = 0b0000'0100;
    static constexpr u8 m_yellow_wire_bit = 0b0000'1000;
    static constexpr u8 m_actuator_bit = 0b0001'0000;
    static constexpr u8 m_actuated_bit = 0b0010'0000;
    static constexpr u8 m_liquid_bits = 0b1100'0000;
    static constexpr u8 m_liquid_shift = 6;
};
}
