#include "Characters/MrBones.hpp"

#include "DamageTypes.hpp"
#include "Characters/DamageSounds.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string MrBonesRunParticlesPath = "Particles/dust_02.partsys";

        MrBones::MrBones(const CharacterParameters& parameters)
            : BasicCharacter(parameters, "Skeletons/Characters/MrBones/mr_bones.skel",
                             "Skeletons/Characters/MrBones/mr_bones.polymatset")
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            SetMoveType(MoveType_Walk);
            SetMoveSpeed(300.0f);
            SetMaxHealth(30.0f);
            SetHealthRegeneration(0.0f);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle1", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle2", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle3", 0.1f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle4", 0.1f);

            AddWeaponAttachment("handa", "weapona1", "weapona2", { { Item::WeaponType_Melee_1H, "handa" }, });
            AddWeaponAttachment("handb", "weaponb1", "weaponb2", { { Item::WeaponType_Melee_1H, "handb" },
                                                                   { Item::WeaponType_Ranged_Bow, "handb"} });

            AddAttackAnimation("attack1", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, {} } } });
            AddAttackAnimation("attack2", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, {} } } });
            AddAttackAnimation("attack4", 0.5f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, {} } } });
            
            AddAttackAnimation("attack3", 0.5f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, {} } },
                                                  { "handb", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, {} } } });

            AddAttackAnimation("range_attack0", 1.0f, { { "handb", { Item::WeaponType_Ranged_Bow, { { "fire_arrow", "fire_arrow" } }, {} } } });

            AddFootJoint("foota", "foota");
            AddFootJoint("footb", "footb");
            SetRunParticleSystem(MrBonesRunParticlesPath);
            AddRunParticleTag("dust_0");
            AddRunParticleTag("dust_1");

            SetAggroRange(1000.0f);

            SetHeadName("sight_line");

            AddMaterial("legs", "Skeletons/Characters/MrBones/skellegs_0.polymatset", 1.0f);
            AddMaterial("legs", "Skeletons/Characters/MrBones/skellegs_1.polymatset", 1.0f);
            AddMaterial("legs", "Skeletons/Characters/MrBones/skellegs_2.polymatset", 1.0f);
            AddMaterial("legs", "Skeletons/Characters/MrBones/skellegs_3.polymatset", 1.0f);

            AddDamagedParticleSystem(DamageType_Element_Physical, "Particles/bone_splat.partsys");

            SetBodyMaterial(MaterialType_Type_Bone);
            AddDamageSounds(Audio::GetStandardOnBoneDamageSoundPaths());
        }

        void MrBones::OnStateChange(CharacterState newState)
        {
            if (newState == CharacterState_Dead)
            {
                SetSkeletonJointStrength(0);
            }

            BasicCharacter::OnStateChange(newState);
        }


        MrBonesMelee::MrBonesMelee(const CharacterParameters& parameters)
            : MrBones(parameters)
        {
            AddMaterial("arms", "Skeletons/Characters/MrBones/skelarm_0.polymatset", 1.0f);
            AddMaterial("arms", "Skeletons/Characters/MrBones/skelarm_1.polymatset", 1.0f);

            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_0.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_1.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_2.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_3.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_4.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_5.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_6.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_7.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_8.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_9.polymatset", 1.0f);
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_10.polymatset", 1.0f);

            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_0.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_1.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_2.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_3.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_4.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_5.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_6.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_7.polymatset", 1.0f);
        }

        MrBonesArcher::MrBonesArcher(const CharacterParameters& parameters)
            : MrBones(parameters)
        {
            AddMaterial("head", "Skeletons/Characters/MrBones/skelhead_archer.polymatset", 1.0f);
            AddMaterial("body", "Skeletons/Characters/MrBones/skelbody_archer.polymatset", 1.0f);
            AddMaterial("arms", "Skeletons/Characters/MrBones/skelarm_archer.polymatset", 1.0f);
        }
    }

    template <>
    void EnumeratePreloads<Character::MrBones>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert("Skeletons/Characters/MrBones/mr_bones.skel");
        preloads.insert("Skeletons/Characters/MrBones/mr_bones.polymatset");

        preloads.insert("Skeletons/Characters/MrBones/skelarm_0.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelarm_1.polymatset");

        preloads.insert("Skeletons/Characters/MrBones/skellegs_0.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skellegs_1.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skellegs_2.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skellegs_3.polymatset");

        preloads.insert("Particles/bone_splat.partsys");
        preloads.insert(Character::MrBonesRunParticlesPath);

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnBoneDamageSoundPaths());
    }

    template <>
    void EnumeratePreloads<Character::MrBonesMelee>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::MrBones>(preloads);

        preloads.insert("Skeletons/Characters/MrBones/skelhead_0.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_1.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_2.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_3.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_4.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_5.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_6.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_7.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_8.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_9.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelhead_10.polymatset");

        preloads.insert("Skeletons/Characters/MrBones/skelbody_0.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_1.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_2.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_3.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_4.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_5.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_6.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_7.polymatset");
    }

    template <>
    void EnumeratePreloads<Character::MrBonesArcher>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::MrBones>(preloads);

        preloads.insert("Skeletons/Characters/MrBones/skelhead_archer.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelbody_archer.polymatset");
        preloads.insert("Skeletons/Characters/MrBones/skelarm_archer.polymatset");
    }
}
