#include "Characters/Dwarves/Dwarf.hpp"
#include "ContentUtility.hpp"
#include "Characters/DamageSounds.hpp"
#include "Characters/FootstepSounds.hpp"
#include "DamageTypes.hpp"
#include "SkeletonUtility.hpp"

#include <functional>

namespace Dwarf
{
    namespace Character
    {
        DwarfInfo::DwarfInfo()
            : Name()
            , Size(1.0f)
            , Kills(0)
            , DamageInflicted(0.0f)
            , DamageRecieved(0.0f)
        {
        }

        void WriteToXML(XML::XMLNode& node, const DwarfInfo& value)
        {
            node.AddChild("Name").SetValue(value.Name);
            node.AddChild("Size").SetValue(value.Size);
            node.AddChild("Kills").SetValue(value.Kills);
            node.AddChild("DamageInflicted").SetValue(value.DamageInflicted);
            node.AddChild("DamageRecieved").SetValue(value.DamageRecieved);
        }

        void ReadFromXML(const XML::XMLNode& node, DwarfInfo& value)
        {
            value.Name = node.GetChild("Name").GetValue<std::string>();
            value.Size = node.GetChild("Size").GetValue<float>();
            value.Kills = node.GetChild("Kills").GetValue<uint32_t>();
            value.DamageInflicted = node.GetChild("DamageInflicted").GetValue<float>();
            value.DamageRecieved = node.GetChild("DamageRecieved").GetValue<float>();
        }

        static const uint32_t DwardBeardColorCount = 7;
        static const uint32_t DwardBeardTypeCount = 10;

        static std::vector<std::string> GetDwarfBeardMaterials()
        {
            std::vector<std::string> materials;
            for (uint32_t i = 0; i < DwardBeardColorCount; i++)
            {
                for (uint32_t j = 0; j < DwardBeardTypeCount; j++)
                {
                    materials.push_back(Format("Skeletons/Characters/Dwarf2/Beards/Beards%u/beard_%u.polymatset", i, j));
                }
            }

            return materials;
        }

        static std::string GetDwarfBeardMaterial(const std::string& name)
        {
            std::hash<std::string> hasher;
            std::vector<std::string> headMaterials = GetDwarfBeardMaterials();
            return headMaterials[hasher(name) % headMaterials.size()];
        }

        static const Audio::SoundPathVector DwarfAffirmativeSounds =
        {
            { "Audio/vocalizations/Dorf/Dorf_Affirmative1.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Affirmative2_Me.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Affirmative3_Oh.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Affirmative4_HoHo.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Affirmative5_Oh.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Affirmative6_HO!.ogg", 1.0f },
        };

        static const Audio::SoundPathVector DwarfNegatorySounds =
        {
            { "Audio/vocalizations/Dorf/Dorf_Negatory1_UhnUh.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Negatory2_UhnUhAlso.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Negatory3_Meh.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Negatory4_Eaohrw.ogg", 1.0f },
        };

        static const Audio::SoundPathVector DwarfDeathSounds =
        {
            { "Audio/vocalizations/Dorf/Dorf_Downstate1.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Downstate2.ogg", 1.0f },
            { "Audio/vocalizations/Dorf/Dorf_Downstate3.ogg", 1.0f },
        };

        static const Audio::SoundPathVector DwarfEatingSounds =
        {
            { "Audio/vocalizations/Dorf/Dorf_Eating_Loop.ogg", 1.0f },
        };

        static const Audio::SoundPathVector DwarfDrinkingSounds =
        {
        };

        static const std::vector<Audio::SoundPathVector> DwarfLeapingSounds =
        {
            {
                { "Audio/vocalizations/Dorf/Dorf_Leap1_short.ogg", 1.0f },
            },
            {
                { "Audio/vocalizations/Dorf/Dorf_Leap2_medium.ogg", 1.0f },
            },
            {
                { "Audio/vocalizations/Dorf/Dorf_Leap3_longer.ogg", 1.0f },
            },
            {
                { "Audio/vocalizations/Dorf/Dorf_Leap4_longest.ogg", 1.0f },
            },
        };

        static const Audio::SoundPathVector DwarfLeapLandAlwaysSounds =
        {
            { "Audio/Items/Weapons/Ork_Sword_Swipe.wav", 1.0f },
        };

        static const std::map<Item::WeaponType, Audio::SoundPathVector> DwarfAttackWooshSounds = 
        {
            {
                Item::WeaponType_Melee_1H,
                {
                    { "Audio/Items/Weapons/Fighter_Sword_Swipe.ogg", 1.0f },
                }
            },
            {
                Item::WeaponType_Ranged_Thrown,
                {
                    { "Audio/Items/Weapons/Fighter_Sword_Swipe.ogg", 1.0f },
                }
            },
        };

        static const float DwarfSpeechGlobalVolume = 0.6f;
        static const float DwarfSpeechPositionalVolume = 1.0f;
        static const std::pair<float, float> DwarfSpeechRadius = std::make_pair(1000.0f, 3000.0f);

        static const std::pair<float, float> DwarfFootstepVolumeRange = std::make_pair(0.1f, 0.25f);
        static const std::pair<float, float> DwarfLeapFootstepVolumeRange = std::make_pair(0.8f, 1.0f);

        static const std::string DwarfSkeletonPath = "Skeletons/Characters/Dwarf2/dwarf2.skel";
        static const std::string DwarfRunParticlesPath = "Particles/dust_02.partsys";
        static const std::string DwarfReviveCursorPath = "HUD/hud.cursorset";
        static const std::string DwarfDamageParticlesPath = "Particles/blood_splat.partsys";
        static const std::string DwarfHealParticlesPath = "Particles/heal_0.partsys";
        static const std::vector<std::string> DwarfEatParticlesPath = { "Particles/eat_1.partsys" };
        static const std::string DwarfIconMatsetPath = "HUD/hud.hudmatset";

        static const std::string DwarfDeathParticlesPath = "Particles/dwarf_ghost1.partsys";

        static const std::string DwarfMouthJoint = "mouth";
        static const std::string DwarfMouthDirectionJoint = "mouth_dir";

        static const std::string DwarfPipeAttachPoint = "pipe";
        static const std::string DwarfCookbookAttachPoint = "cookbook";
        static const std::string DwarfCookPotAttachPoint = "cookpot";

        static const std::string DwarfKegAttachPoint = "keg";

        static const std::string DwarfHeadLampAttachPoint = "lamp";

        static const std::string DwarfThrownItemAttachPoint = "handa_back";
        static const std::string DwarfFoodThrownItemAttachPoint = "handa_back";

        static const std::map<DrinkableType, std::pair<std::string, std::string>> DwarfDrinkingCupSkeletonMatsetPaths
        {
            {
                DrinkableType::Beer,
                {
                    "Skeletons/Items/Food/beer.skel",
                    "Skeletons/Items/Food/beer.polymatset",
                }
            },
            {
                DrinkableType::Wine,
                {
                    "Skeletons/Items/Food/wine.skel",
                    "Skeletons/Items/Food/wine.polymatset",
                }
            },
        };
        static const std::string DwarfDrinkingCupAttachJoint = "handa_back";
        static const std::string DwarfDrinkingCupAttachName = "cup";

        static CharacterParameters ModifyCharacterParameters(const CharacterParameters& parameters, const DwarfInfo& info)
        {
            CharacterParameters modifiedParameters(parameters);
            modifiedParameters.Name = info.Name;
            return modifiedParameters;
        }

        static const float DwarfDownedDeathDuration = 20.0f;

        static const float DwarfCritChance = 0.05f;
        static const float DwarfCritDamage = 2.00f;

        static const Pathfinding::EdgeType DwarfRegularWalkEdgeTypes = static_cast<Pathfinding::EdgeType>(Pathfinding::EdgeType_Walk | Pathfinding::EdgeType_Climb);
        static const Pathfinding::EdgeType DwarfIncapacitatedWalkEdgeTypes = Pathfinding::EdgeType_Walk;

        Dwarf::Dwarf(const CharacterParameters& parameters, const DwarfInfo& info, const std::string& materialSet)
            : BasicCharacter(ModifyCharacterParameters(parameters, info), DwarfSkeletonPath, materialSet)
            , _downed(false)
            , _downedDeathTimer(0.0f)
            , _standUpQueued(false)
            , _downedTransitioning(false)
            , _downedTransitionDuration(0.0f)
            , _downedTransitionTimer(0.0f)
            , _throwReleaseTag()
            , _throwAnimations()
            , _leapAnimations()
            , _standUpAnimations()

            , _eating(false)
            , _eatAnimations()
            , _incapacitatedEatAnimations()
            , _eatAnimationTags()
            , _eatParticles()

            , _info(info)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 1);
            SetItemSlotCount(Item::ItemSlot::Armor, 1);
            SetItemSlotCount(Item::ItemSlot::Trinket, 1);

            SetDropWeaponsOnDeath(false);
            SetDropArmorsOnDeath(false);
            SetDropTrinketsOnDeath(false);

            SetMoveType(MoveType_Walk);

            SetMoveSpeed(Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, 550.0f);
            SetMoveSpeed(Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Incapacitated, 80.0f);
            SetMoveSpeed(Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -PiOver2, AnimationVariant_Standard, 750.0f);
            SetMoveSpeed(Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, 0.0f,     AnimationVariant_Standard, 250.0f);
            SetMoveSpeed(Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Wood, -PiOver2, AnimationVariant_Standard, 400.0f);
            SetMoveSpeed(Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Wood, 0.0f, AnimationVariant_Standard, 500.0f);

            SetPathableEdges(DwarfRegularWalkEdgeTypes);
            SetMaxHealth(40.0f * info.Size);
            SetHealthRegeneration(0.25f);
            SetBaseCriticalChance(DwarfCritChance);
            SetBaseCriticalDamage(DwarfCritDamage);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(info.Size);
            SetSkeletonColor(Color::White);

            AddMainArmJoint("upperarma");
            AddMainArmJoint("forearma");
            AddMainArmJoint("handa");

            AddOffArmJoint("upperarmb");
            AddOffArmJoint("forearmb");
            AddOffArmJoint("handb");

            // Standing standard idle
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "IDLE_0", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "IDLE_1", 0.01f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "IDLE_2", 0.01f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "IDLE_3", 0.01f);

            // TODO: use SIT_0 animation

            // Standing incapacitated idle
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Incapacitated, "DOWN_IDLE", 1.0f);

            // Standing injured idle
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Injured, "HPLOW_IDLE_0", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Injured, "HPLOW_IDLE_1", 1.0f);

            // Standing injured moving
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Incapacitated, "DOWN_CRAWL", 1.0f);

            // Climbing rope
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.5f,   AnimationVariant_Standard, "ROPE_CLIMB_DOWN_0", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.375f, AnimationVariant_Standard, "ROPE_CLIMB_DOWN_1", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.25f,  AnimationVariant_Standard, "ROPE_CLIMB_DOWN_2", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.125f, AnimationVariant_Standard, "ROPE_CLIMB_DOWN_3", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, 0.0f,         AnimationVariant_Standard, "ROPE_CLIMB_3", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, Pi * 0.125f,  AnimationVariant_Standard, "ROPE_CLIMB_2", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, Pi * 0.25f,   AnimationVariant_Standard, "ROPE_CLIMB_1", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, Pi * 0.375f,  AnimationVariant_Standard, "ROPE_CLIMB_0", 1.0f);

            // Idle rope
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.5f,   AnimationVariant_Standard, "ROPE_CLIMB_DOWN_0_IDLE", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.375f, AnimationVariant_Standard, "ROPE_CLIMB_DOWN_1_IDLE", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.25f,  AnimationVariant_Standard, "ROPE_CLIMB_DOWN_2_IDLE", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, -Pi * 0.125f, AnimationVariant_Standard, "ROPE_CLIMB_DOWN_3_IDLE", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, 0.0f,         AnimationVariant_Standard, "ROPE_CLIMB_3_IDLE", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, Pi * 0.125f,  AnimationVariant_Standard, "ROPE_CLIMB_2_IDLE", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, Pi * 0.25f,   AnimationVariant_Standard, "ROPE_CLIMB_1_IDLE", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, Pi * 0.375f,  AnimationVariant_Standard, "ROPE_IDLE_0", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope, Pi * 0.375f,  AnimationVariant_Standard, "ROPE_IDLE_1", 0.05f);

            // Climbing ladder
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Wood, -PiOver2, AnimationVariant_Standard, "LADDER_CLIMB", 1.0f);

            // Idle ladder
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Wood, -PiOver2, AnimationVariant_Standard, "LADDER_IDLE_0", 1.0f);

            AddInteractAnimation("INTERACT", 1.0f);

            AddWeaponAttachment("main_hand", "weapon_a_0", "weapon_a_1", { { Item::WeaponType_Melee_1H, "handa_back" },
                                                                           { Item::WeaponType_Melee_Shield, "handa_front" },
                                                                           { Item::WeaponType_Ranged_Gun_1H, "handa_back" },
                                                                           { Item::WeaponType_Ranged_Gun_2H, "handa_back" },
                                                                           { Item::WeaponType_Ranged_Thrown, "handa_back" }, });
            AddWeaponAttachment("off_hand", "weapon_b_0", "weapon_b_1", { { Item::WeaponType_Melee_1H, "handb" }, });

            AddAttackAnimation("ATTACK_1H_0", 1.0f, { { "main_hand", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, {}, { "woosh0", } } } });

            AddAttackAnimation("ATTACK_2H_0", 1.0f, { { "main_hand", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" } }, {}, {}, { "woosh0", } } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H, { { "dmg_startb", "dmg_endb" } }, {}, {}, { "woosh1", } } } });
            AddAttackAnimation("ATTACK_2H_1", 1.0f, { { "main_hand", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" } }, {}, {}, { "woosh0", } } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H, { { "dmg_startb", "dmg_endb" } }, {}, {}, { "woosh1", } } } });
            AddAttackAnimation("ATTACK_2H_2", 1.0f, { { "main_hand", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" } }, {}, {}, { "woosh0", } } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H, { { "dmg_startb", "dmg_endb" } }, {}, {}, { "woosh1", } } } });
            AddAttackAnimation("ATTACK_2H_3", 1.0f, { { "main_hand", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" } }, {}, {}, { "woosh0", } } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H, { { "dmg_startb", "dmg_endb" } }, {}, {}, { "woosh1", } } } });
            AddAttackAnimation("ATTACK_2H_4", 1.0f, { { "main_hand", { Item::WeaponType_Melee_1H, { { "dmg_startb", "dmg_endb" } }, {}, {}, { "woosh0", } } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H, { { "dmg_startb", "dmg_endb" } }, {}, {}, { "woosh1", } } } });

            AddAttackAnimation("ATTACK_SH_0", 1.0f, { { "main_hand", { Item::WeaponType_Melee_Shield, {}, {}, {} } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H,     { { "dmg_start", "dmg_end" } }, {}, {}, { "woosh0", } } } });
            AddAttackAnimation("ATTACK_SH_1", 1.0f, { { "main_hand", { Item::WeaponType_Melee_Shield, {}, {}, {} } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H,     { { "dmg_start", "dmg_end" } }, {}, {}, { "woosh0", } } } });
            AddAttackAnimation("ATTACK_SH_2", 1.0f, { { "main_hand", { Item::WeaponType_Melee_Shield, { { "dmg_start", "dmg_end" } }, {}, {}, { "woosh0", } } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H,     {}, {}, {} } } });
            AddAttackAnimation("ATTACK_SH_3", 1.0f, { { "main_hand", { Item::WeaponType_Melee_Shield, {}, {}, {} } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H,     { { "dmg_start", "dmg_end" } }, {}, {}, { "woosh0", } } } });
            AddAttackAnimation("ATTACK_SH_4", 1.0f, { { "main_hand", { Item::WeaponType_Melee_Shield, {}, {}, {} } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H,     { { "dmg_start0", "dmg_end0" }, { "dmg_start1", "dmg_end1" }, { "dmg_start2", "dmg_end2" }, }, { "dmg_reset0", "dmg_reset1", }, {}, { "woosh0", "woosh1", "woosh2", } } } });
            AddAttackAnimation("ATTACK_SH_5", 1.0f, { { "main_hand", { Item::WeaponType_Melee_Shield, {}, {}, {} } },
                                                      { "off_hand",  { Item::WeaponType_Melee_1H,     { { "dmg_start", "dmg_end" } }, {}, {}, { "woosh0", } } } });

            AddAttackAnimation("ATTACK_RIFLE_0", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Gun_2H, { { "fire", "fire" } }, {}, {}, {} } } });
            AddAttackAnimation("ATTACK_RIFLE_1", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Gun_2H, { { "fire_0", "fire_0" }, { "fire_1", "fire_1" }, { "fire_2", "fire_2" }, }, { "dmg_reset0", "dmg_reset1" }, {}, {} } } });
            AddAttackAnimation("ATTACK_RIFLE_2", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Gun_2H, { { "fire", "fire" } }, {}, {} } } });

            AddAttackAnimation("ATTACK_PISTOL_0", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Gun_1H, { { "fire", "fire" } }, {} } } });
            AddAttackAnimation("ATTACK_PISTOL_1", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Gun_1H, { { "fire_0", "fire_0" }, { "fire_1", "fire_1" } }, { "dmg_reset" }, {}, {} } } });
            AddAttackAnimation("ATTACK_PISTOL_2", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Gun_1H, { { "fire", "fire" } }, {}, {}, {} } } });

            AddAttackAnimation("ATTACK_THROW_0", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Thrown, { { "fire", "fire" } }, {}, {}, { "woosh0", } } } });
            AddAttackAnimation("ATTACK_THROW_1", 1.0f, { { "main_hand", { Item::WeaponType_Ranged_Thrown, { { "fire", "fire" } }, {}, {}, { "woosh0", } } } });

            AddAttackWooshSounds(DwarfAttackWooshSounds);

            AddFootJoint("foota", "foota");
            AddFootJoint("footb", "footb");
            SetRunParticleSystem(DwarfRunParticlesPath);
            AddRunParticleTag("DUST_0");
            AddRunParticleTag("DUST_1");

            SetAggroRange(1500.0f);

            SetHeadName("head");
            AddMaterial("mouth", GetDwarfBeardMaterial(info.Name), 1.0f);

            AddDamagedParticleSystem(DamageType_Element_Physical, DwarfDamageParticlesPath);
            AddHealingParticleSystem(DwarfHealParticlesPath);

            SetDeadIcon(DwarfIconMatsetPath, "class_dead");

            _throwReleaseTag = "fire";
            _throwAnimations.AddAnimation("ATTACK_BOMB_0", 1.0f);
            _foodThrowAnimations.AddAnimation("FOOD_THROW_0", 1.0f);

            _leapAnimations.AddAnimation("LEAP", 1.0f);
            for (const auto& leapSounds : DwarfLeapingSounds)
            {
                _leapSounds.push_back(Audio::SoundSet(leapSounds));
            }
            for (const auto& leapTerrainSounds : Audio::GetStandardTerrainLeapFootstepSoundPaths())
            {
                _leapTerrainSounds[leapTerrainSounds.first] = Audio::SoundSet(leapTerrainSounds.second);
            }
            for (const auto& leapLandTerrainSounds : Audio::GetStandardTerrainLandFootstepSoundPaths())
            {
                _leapLandTerrainSounds[leapLandTerrainSounds.first] = Audio::SoundSet(leapLandTerrainSounds.second);
            }
            _leapLandAlwaysSounds.AddSounds(DwarfLeapLandAlwaysSounds);

            _reviveAnimations.AddAnimation("REVIVE", 1.0f);
            _standUpAnimations.AddAnimation("DOWN_REVIVE", 1.0f);

            _eatAnimations.AddAnimation("EAT", 1.0f);
            _eatAnimationTags["EAT"] = { "eat0", "eat1", "eat2", "eat3", };

            _eatIncapacitatedAnimations.AddAnimation("DOWN_EAT", 1.0f);
            _eatAnimationTags["DOWN_EAT"] = { "eat0", "eat1", "eat2", };

            _drinkAnimations.AddAnimation("DRINK", 1.0f);
            _drinkingSounds.AddSounds(DwarfDrinkingSounds);

            _cookAnimations.AddAnimation("COOK", 1.0f);

            _kegThrowndownAnimations.AddAnimation("KEG_DEPLOY", 1.0f);

            AddCustomAttachPoint(DwarfThrownItemAttachPoint, "weapon_a_0", "weapon_a_1");
            AddCustomAttachPoint(DwarfFoodThrownItemAttachPoint, "weapon_a_0", "weapon_a_1");
            AddCustomAttachPoint(DwarfPipeAttachPoint, "pipe_0", "pipe_1");
            AddCustomAttachPoint(DwarfCookbookAttachPoint, "cookbook_0", "cookbook_1");
            AddCustomAttachPoint(DwarfCookPotAttachPoint, "cookpot_0", "cookpot_1");
            AddCustomAttachPoint(DwarfKegAttachPoint, "weapon_a_0", "weapon_a_1");
            AddCustomAttachPoint(DwarfHeadLampAttachPoint, "head_light_base", "head_light_line");
            AddCustomAttachPoint(DwarfDrinkingCupAttachName, "weapon_a_0", "weapon_a_1");

            SetBodyMaterial(MaterialType_Type_Flesh);
            AddDamageSounds(Audio::GetStandardOnFleshDamageSoundPaths());
            AddDamageSounds(Audio::GetStandardOnClothDamageSoundPaths());
            AddDamageSounds(Audio::GetStandardOnMetalDamageSoundPaths());
            AddHealingSounds(Audio::GetStandardHealingSoundPaths());

            AddAffirmativeSounds(DwarfAffirmativeSounds);
            AddNegatorySounds(DwarfNegatorySounds);

            _downedSounds.AddSounds(DwarfDeathSounds);
            _eatingSounds.AddSounds(DwarfEatingSounds);

            AddBaseFootstepSounds(Audio::GetStandardBasicFootstepSoundPaths());
            for (const auto& terrainFootsteps : Audio::GetStandardTerrainFootstepSoundPaths())
            {
                AddTerrainFootstepSounds(terrainFootsteps.first, terrainFootsteps.second);
            }
            SetFootstepVolumeRange(DwarfFootstepVolumeRange.first, DwarfFootstepVolumeRange.second);

            SetGlobalSpeechParameters(DwarfSpeechGlobalVolume);
            SetPositionalSpeechParameters(DwarfSpeechPositionalVolume, DwarfSpeechRadius.first, DwarfSpeechRadius.second);
        }

        const DwarfInfo& Dwarf::GetInfo() const
        {
            return _info;
        }

        const App::Cursor* Dwarf::GetInteractCursor() const
        {
            return _reviveCursor;
        }

        void Dwarf::PlayThrowAnimation()
        {
            PlayAnimationSet(_throwAnimations, false, 0.1f, 0.0f);
        }

        std::string Dwarf::GetThrownItemAttachPointName() const
        {
            return DwarfThrownItemAttachPoint;
        }

        bool Dwarf::HasThrowReleasePassed() const
        {
            return HasAnimationTagPassed(_throwReleaseTag);
        }

        bool Dwarf::HasThrowAnimationFinished() const
        {
            return IsAnimationFinished();
        }

        float Dwarf::playEatAnimation()
        {
            _eating = true;
            _curEatingSound = Speak(_eatingSounds, true, true);
            SetWeaponAlpha(0.0f, 0.25f);
            if (IsIncapacitated())
            {
                return PlayAnimationSet(_eatIncapacitatedAnimations, true, 0.1f, 0.0f);
            }
            else
            {
                return PlayAnimationSet(_eatAnimations, true, 0.1f, 0.0f);
            }
        }

        float Dwarf::playReiveAnimation()
        {
            return PlayAnimationSet(_reviveAnimations, false, 0.1f, 0.0f);
        }

        float Dwarf::playDrinkAnimation(DrinkableType type)
        {
            _drinking = true;
            _drinkingType = type;
            assert(_drinkingCups.find(type) != _drinkingCups.end());
            Speak(_drinkingSounds, false, true);
            updateCupPositions();
            AttachDrawable(DwarfDrinkingCupAttachJoint, _drinkingCups.at(type));
            SetWeaponAlpha(0.0f, 0.25f);
            return PlayAnimationSet(_drinkAnimations, false, 0.1f, 0.0f);
        }

        void Dwarf::updateCupPositions()
        {
            auto drinkingCup = _drinkingCups.at(_drinkingType);
            Rayf attachRay = GetAttachPoint(DwarfDrinkingCupAttachName);
            Animation::AttachmentInfo attachInfo = Animation::GetSkeletonAttachmentInfo(
                Animation::IsCharacterInvertedX(this), Animation::IsCharacterInvertedY(this), true, attachRay,
                drinkingCup, "a", "b");
            drinkingCup->SetInvertedX(attachInfo.InvertX);
            drinkingCup->SetInvertedY(attachInfo.InvertY);
            drinkingCup->SetPosition(attachInfo.Position);
            drinkingCup->SetScale(attachInfo.Scale);
            drinkingCup->SetRotation(attachInfo.Rotation);
        }

        void Dwarf::OnPostEat(Character* food)
        {
            if (_downed)
            {
                revive(food);
            }
        }

        float Dwarf::OnPreLeap(Pathfinding::TerrainType terrainType)
        {
            // No wind up time
            return 0;
        }

        void Dwarf::OnLeap(Pathfinding::TerrainType terrainType, float airTime, float maxAirTime)
        {
            assert(_leapSounds.size() > 0);
            float distPerc = Clamp(airTime / maxAirTime, 0.0f, 1.0f);
            uint32_t soundIdx = Min(static_cast<uint32_t>(distPerc * _leapSounds.size()), _leapSounds.size() - 1);
            Speak(_leapSounds[soundIdx], false, true);

            auto footSoundIter = _leapTerrainSounds.find(terrainType);
            if (footSoundIter != _leapTerrainSounds.end())
            {
                auto soundManager = GetLevel()->GetSoundManager();
                soundManager->PlaySinglePositionalSound(footSoundIter->second.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(), 
                                                        DwarfSpeechRadius.first, DwarfSpeechRadius.second,
                                                        Random::RandomBetween(DwarfLeapFootstepVolumeRange.first, DwarfLeapFootstepVolumeRange.second));
            }

            PlayAnimationSet(_leapAnimations, true, 0.1f, 0.0f);
        }

        float Dwarf::OnPostLeap(Pathfinding::TerrainType terrainType)
        {
            auto soundManager = GetLevel()->GetSoundManager();
            auto footSoundIter = _leapLandTerrainSounds.find(terrainType);
            if (footSoundIter != _leapLandTerrainSounds.end())
            {
                soundManager->PlaySinglePositionalSound(footSoundIter->second.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                                                        DwarfSpeechRadius.first, DwarfSpeechRadius.second,
                                                        Random::RandomBetween(DwarfLeapFootstepVolumeRange.first, DwarfLeapFootstepVolumeRange.second));
            }
            soundManager->PlaySinglePositionalSound(_leapLandAlwaysSounds.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                                                    DwarfSpeechRadius.first, DwarfSpeechRadius.second,
                                                    Random::RandomBetween(DwarfLeapFootstepVolumeRange.first, DwarfLeapFootstepVolumeRange.second));

            return 0.0f;
        }

        std::string Dwarf::GetPipeAttachPointName() const
        {
            return DwarfPipeAttachPoint;
        }

        void Dwarf::PlayCookAnimation()
        {
            // TODO: show cookpot
            PlayAnimationSet(_cookAnimations, true, 0.1f, 0.0f);
        }

        std::string Dwarf::GetCookbookAttachPointName() const
        {
            return DwarfCookbookAttachPoint;
        }

        std::string Dwarf::GetCookPotAttachPointName() const
        {
            return DwarfCookPotAttachPoint;
        }

        void Dwarf::PlayFoodThrowAnimation()
        {
            PlayAnimationSet(_foodThrowAnimations, false, 0.1f, 0.0f);
        }

        std::string Dwarf::GetFoodThrownItemAttachPointName() const
        {
            return DwarfFoodThrownItemAttachPoint;
        }

        bool Dwarf::HasFoodThrowReleasePassed() const
        {
            return HasAnimationTagPassed(_throwReleaseTag);
        }

        bool Dwarf::HasFoodThrowAnimationFinished() const
        {
            return IsAnimationFinished();
        }

        Brewer::Info Dwarf::PlayKegThrowdownAnimation()
        {
            std::string anim = _kegThrowndownAnimations.PeekNextAnimation();
            PlayAnimationSet(_kegThrowndownAnimations, false, 0.1f, 0.0f);

            const auto& skeleton = GetSkeleton();

            Brewer::Info info;
            info.TotalDuration = skeleton->GetAnimationLength(anim);
            info.KegSpawnTime = skeleton->GetAnimationTagTime(anim, "keg_spawn");
            info.KegDropTime = skeleton->GetAnimationTagTime(anim, "keg_drop");

            return info;
        }

        std::string Dwarf::GetKegAttachPointName() const
        {
            return DwarfKegAttachPoint;
        }

        std::string Dwarf::GetLampAttachPoint() const
        {
            return DwarfHeadLampAttachPoint;
        }

        bool Dwarf::IsDowned() const
        {
            return _downed;
        }

        float Dwarf::Revive(Character* reviver)
        {
            if (_downed)
            {
                return revive(reviver);
            }

            return -1.0f;
        }

        DwarfInfo Dwarf::CreateDefaultDwarfInfo(const CharacterParameters& parameters)
        {
            DwarfInfo info;
            info.Name = parameters.Name;
            return info;
        }

        void Dwarf::OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& inputDamage)
        {
            BasicCharacter::OnPostRecieveDamage(source, position, inputDamage);

            _info.DamageRecieved += inputDamage.Amount;

            // Damage was lethal
            if (GetHealth().GetCurrent() <= 0.0f && !_downed)
            {
                down();
            }
        }

        void Dwarf::OnPostInflictDamage(Character* target, const Vector2f& position, const Damage& damage)
        {
            BasicCharacter::OnPostInflictDamage(target, position, damage);

            _info.DamageInflicted += damage.Amount;
        }

        void Dwarf::OnTargetKilled(Character* target)
        {
            BasicCharacter::OnTargetKilled(target);

            _info.Kills++;
        }

        float Dwarf::OnPreInteractWithCharacter(Character* target)
        {
            if (IsA<Edible>(target))
            {
                return playEatAnimation();
            }
            else if (IsA<Drinkable>(target))
            {
                return playDrinkAnimation(AsA<Drinkable>(target)->GetDrinkableType());
            }
            else if (IsA<Reviveable>(target) && AsA<Reviveable>(target)->IsDowned())
            {
                return Max(playReiveAnimation(), AsA<Reviveable>(target)->Revive(this));
            }
            else
            {
                return BasicCharacter::OnPreInteractWithCharacter(target);
            }
        }

        bool Dwarf::CanBeInteractedWith(const Character* interactor) const
        {
            return _downed && IsA<Dwarf>(interactor);
        }

        Vector2f Dwarf::GetDeathImpulse(const Physics::Collision* collision) const
        {
            if (!IsAlive())
            {
                return Vector2f::Zero;
            }
            else
            {
                return BasicCharacter::GetDeathImpulse(collision);
            }
        }

        void Dwarf::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicCharacter::OnLoadContent(contentManager);

            const auto& skeleton = GetSkeleton();
            for (const auto& eatParticles : DwarfEatParticlesPath)
            {
                const auto& particles = Content::CreateParticleSystemInstance(contentManager, eatParticles);
                skeleton->AddChildDrawable(DwarfMouthJoint, particles);
                _eatParticles.push_back(particles);
            }

            _downedSounds.LoadContent(contentManager);
            _eatingSounds.LoadContent(contentManager);
            _drinkingSounds.LoadContent(contentManager);

            for (const auto& drinkableType : DwarfDrinkingCupSkeletonMatsetPaths)
            {
                _drinkingCups[drinkableType.first] =
                    Content::CreateSkeletonInstance(contentManager, drinkableType.second.first, drinkableType.second.second);
            }

            for (auto& leapSounds : _leapSounds)
            {
                leapSounds.LoadContent(contentManager);
            }
            for (auto& leapTerrainType : _leapTerrainSounds)
            {
                leapTerrainType.second.LoadContent(contentManager);
            }
            for (auto& leapTerrainType : _leapLandTerrainSounds)
            {
                leapTerrainType.second.LoadContent(contentManager);
            }
            _leapLandAlwaysSounds.LoadContent(contentManager);

            _deathParticles = Content::CreateParticleSystemInstance(contentManager, DwarfDeathParticlesPath);
            _reviveCursor = Content::GetCursor(contentManager, DwarfReviveCursorPath, "cursor_revive");
        }

        void Dwarf::OnUnloadContent()
        {
            BasicCharacter::OnUnloadContent();

            SafeReleaseAndClearContainer(_eatParticles);

            _downedSounds.UnloadContent();
            _eatingSounds.UnloadContent();
            _drinkingSounds.UnloadContent();

            for (auto drinkingCup : _drinkingCups)
            {
                SafeRelease(drinkingCup.second);
            }
            _drinkingCups.clear();

            for (auto& leapSounds : _leapSounds)
            {
                leapSounds.UnloadContent();
            }
            for (auto& leapTerrainType : _leapTerrainSounds)
            {
                leapTerrainType.second.UnloadContent();
            }
            for (auto& leapTerrainType : _leapLandTerrainSounds)
            {
                leapTerrainType.second.UnloadContent();
            }
            _leapLandAlwaysSounds.UnloadContent();

            SafeRelease(_deathParticles);
            SafeRelease(_reviveCursor);
        }

        void Dwarf::OnUpdate(double totalTime, float dt)
        {
            BasicCharacter::OnUpdate(totalTime, dt);

            if (_standUpQueued)
            {
                _standUpQueued = false;
                PlayAnimation(_standUpAnimations.PopNextAnimation(), false, 0.15f, 0.0f);
            }

            if (_downedTransitioning)
            {
                if (!_downed && GetState() != CharacterState_Idle)
                {
                    down();
                }

                _downedTransitionTimer -= dt;

                float perc = _downedTransitionDuration > 0.0f ? 1.0f - Clamp(_downedTransitionTimer / _downedTransitionDuration, 0.0f, 1.0f) : 1.0f;
                SetWeaponAlpha(_downed ? 1.0f - perc : perc, 0.25f);

                if (_downedTransitionTimer <= 0.0f)
                {
                    _downedTransitioning = false;

                    if (!_downed)
                    {
                        const float reviveHealPerc = 0.1f;
                        Heal(nullptr, -Min(GetHealth().GetCurrent(), 0.0f) + (GetHealth().GetMaximum() * reviveHealPerc));

                        SetIncapacitated(false);
                        SetInvulnerable(false);
                        SetAttackable(true);
                        SetEntityMask(GetEntityMask() & ~CharacterMask_Usable);
                        SetCanMoveWhileIncapacitated(false);
                        SetCanInteractWhileIncapacitated(false);
                        SetPathableEdges(DwarfRegularWalkEdgeTypes);
                    }
                }
            }

            if (_eating)
            {
                if (!IsPlayingAnimation(_eatAnimations) && !IsPlayingAnimation(_eatIncapacitatedAnimations))
                {
                    _eating = false;
                    if (_curEatingSound)
                    {
                        _curEatingSound->Stop(0.25f);
                        _curEatingSound = nullptr;
                    }
                    if (!_downed)
                    {
                        SetWeaponAlpha(1.0f, 0.25f);
                    }
                }

                const auto& tags = _eatAnimationTags[GetCurrentAnimation()];
                const auto& skeleton = GetSkeleton();
                for (const auto& tag : tags)
                {
                    if (skeleton->HasAnimationTagJustPassed(tag))
                    {
                        auto& particles = Random::RandomItem(_eatParticles);

                        const Vector2f& mouthPos = skeleton->GetJointPosition(DwarfMouthJoint);
                        Vector2f mouthDir = skeleton->GetJointPosition(DwarfMouthDirectionJoint) - mouthPos;

                        particles->SetPointSpawner(mouthPos);
                        particles->SetRotation(Rotatorf(mouthDir));
                        particles->Burst();
                    }
                }
            }

            if (_drinking)
            {
                updateCupPositions();
                if (!IsPlayingAnimation(_drinkAnimations))
                {
                    _drinking = false;
                    SetWeaponAlpha(1.0f, 0.25f);
                    DetachDrawable(DwarfDrinkingCupAttachJoint, _drinkingCups.at(_drinkingType));
                }
            }
            for (auto drinkingCup : _drinkingCups)
            {
                drinkingCup.second->Update(totalTime, dt);
            }

            for (auto& eatParticles : _eatParticles)
            {
                eatParticles->SetColor(GetColor());
                eatParticles->SetScale(GetScale());
                eatParticles->Update(totalTime, dt);
            }

            if (_downed)
            {
                _downedDeathTimer -= dt;
                if (_downedDeathTimer <= 0.0f)
                {
                    // Time... to die
                    SetInvulnerable(false);
                    Kill(nullptr);
                    _deathParticles->Burst();
                    _downed = false;
                }
            }

            _deathParticles->SetScale(GetScale());
            _deathParticles->SetInvertedX(Animation::IsCharacterInvertedX(this));
            _deathParticles->SetInvertedY(Animation::IsCharacterInvertedY(this));
            _deathParticles->SetPointSpawner(GetBounds().Middle());
            _deathParticles->Update(totalTime, dt);
        }

        void Dwarf::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            levelRenderer->AddDrawable(_deathParticles, false, false);

            BasicCharacter::OnDraw(levelRenderer);
        }

        void Dwarf::down()
        {
            if (_downed)
            {
                return;
            }
            _downed = true;

            SetInvulnerable(true);
            SetAttackable(false);
            SetIncapacitated(true);
            SetEntityMask(GetEntityMask() | CharacterMask_Usable);
            SetCanMoveWhileIncapacitated(true);
            SetCanInteractWhileIncapacitated(true);
            SetPathableEdges(DwarfIncapacitatedWalkEdgeTypes);
            _downedTransitioning = true;
            _downedTransitionDuration = 1.0f;
            _downedTransitionTimer = _downedTransitionDuration;
            DoDeathRagdoll(false);
            PlayAnimation("DOWN_IDLE", false, 0.0f, 0.0f);
            Speak(_downedSounds, false, true);
            SetWeaponAlpha(0.0f, 0.25f);

            _downedTransitioning = false;
            _downedTransitionDuration = 0.0f;
            _downedTransitionTimer = 0.0f;
            _standUpQueued = false;
            _downedDeathTimer = DwarfDownedDeathDuration;
        }

        float Dwarf::revive(Character* reviver)
        {
            if (!_downed)
            {
                return -1.0f;
            }
            _downed = false;

            StopMoving();

            _downedTransitioning = true;
            _downedTransitionDuration = GetSkeleton()->GetAnimationLength(_standUpAnimations.PeekNextAnimation());
            _downedTransitionTimer = _downedTransitionDuration;
            _standUpQueued = true;

            return _downedTransitionDuration;
        }

        const std::string Dwarf::NormalRunAnimation = "RUN_0";
        const std::string Dwarf::ForwardRunAnimation = "RUN_2";
        const std::string Dwarf::BackwardRunAnimation = "RUN_1";
    }

    template <>
    void EnumeratePreloads<Character::Dwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);
        preloads.insert(Character::DwarfSkeletonPath);

        std::vector<std::string> headMaterials = Character::GetDwarfBeardMaterials();
        preloads.insert(headMaterials.begin(), headMaterials.end());

        preloads.insert(Character::DwarfReviveCursorPath);
        preloads.insert(Character::DwarfRunParticlesPath);
        preloads.insert(Character::DwarfDamageParticlesPath);
        preloads.insert(Character::DwarfHealParticlesPath);
        preloads.insert(Character::DwarfEatParticlesPath.begin(), Character::DwarfEatParticlesPath.end());

        preloads.insert(Character::DwarfIconMatsetPath);

        for (const auto& drinkableType : Character::DwarfDrinkingCupSkeletonMatsetPaths)
        {
            preloads.insert(drinkableType.second.first);
            preloads.insert(drinkableType.second.second);
        }

        EnumerateSoundVectorPreloads(preloads, Character::DwarfAffirmativeSounds);
        EnumerateSoundVectorPreloads(preloads, Character::DwarfNegatorySounds);
        EnumerateSoundVectorPreloads(preloads, Character::DwarfDeathSounds);
        EnumerateSoundVectorPreloads(preloads, Character::DwarfEatingSounds);
        EnumerateSoundVectorPreloads(preloads, Character::DwarfDrinkingSounds);
        for (const auto& leapSounds : Character::DwarfLeapingSounds)
        {
            EnumerateSoundVectorPreloads(preloads, leapSounds);
        }

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnFleshDamageSoundPaths());
        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnClothDamageSoundPaths());
        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnMetalDamageSoundPaths());
        EnumerateSoundVectorPreloads(preloads, Audio::GetStandardHealingSoundPaths());

        EnumerateSoundVectorPreloads(preloads, Audio::GetStandardBasicFootstepSoundPaths());
        for (const auto& terrainFootsteps : Audio::GetStandardTerrainFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainFootsteps.second);
        }
        for (const auto& terrainLeapFootsteps : Audio::GetStandardTerrainLeapFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainLeapFootsteps.second);
        }
        for (const auto& terrainLeapLandFootsteps : Audio::GetStandardTerrainLandFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainLeapLandFootsteps.second);
        }
        EnumerateSoundVectorPreloads(preloads, Character::DwarfLeapLandAlwaysSounds);

        for (const auto& wooshSound : Character::DwarfAttackWooshSounds)
        {
            EnumerateSoundVectorPreloads(preloads, wooshSound.second);
        }

        preloads.insert(Character::DwarfDeathParticlesPath);
    }
}
