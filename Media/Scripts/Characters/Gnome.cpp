#include "Characters/Gnome.hpp"

#include "DamageTypes.hpp"
#include "Characters/DamageSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string GnomeWrenchSkeletonPath = "Skeletons/Items/Weapons/GnomeWeapons/gnome_wrench.skel";
        static const std::string GnomeWrenchMatsetPath = "Skeletons/Items/Weapons/GnomeWeapons/wrench.polymatset";

        GnomeWrench::GnomeWrench(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_2H, GnomeWrenchSkeletonPath, GnomeWrenchMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 1.0f));
        }
    }

    namespace Character
    {
        static const std::string GnomeSkeletonPath = "Skeletons/Characters/gnome/gnome.skel";
        static const std::string GnomeMatsetPath = "Skeletons/Characters/gnome/gnome.polymatset";

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> GnomeInterchangeMatsetPaths =
            std::map<std::string, std::vector<std::pair<std::string, float>>>();

        static const std::string GnomeDamageParticlePath = "Particles/blood_splat.partsys";
        static const std::string GnomeRunParticlesPath = "Particles/dust_02.partsys";

        Gnome::Gnome(const CharacterParameters& parameters)
            : BasicCharacter(parameters, GnomeSkeletonPath, GnomeMatsetPath)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            float size = Random::RandomBetween(0.9f, 1.1f);

            SetMoveType(MoveType_Walk);
            SetMoveSpeed(400.0f);
            SetMaxHealth(6.0f * size);
            SetHealthRegeneration(0.0f);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(size);
            SetSkeletonColor(Color::White);

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle0", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle1", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle2", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle3", 1.0f);

            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run1", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run2", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run3", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run4", 1.0f);

            AddWeaponAttachment("handa", "weapona_0", "weapona_1", { { Item::WeaponType_Melee_2H, "handa" } });
            //AddWeaponAttachment("handb", "weaponb_0", "weaponb_1", { });

            AddAttackAnimation("attack0", 1.0f, { { "handa", { Item::WeaponType_Melee_2H, {}, {}, {} } } });
            AddAttackAnimation("attack1", 1.0f, { { "handa", { Item::WeaponType_Melee_2H, {}, {}, {} } } });

            AddFootJoint("foota_front", "foota_back");
            AddFootJoint("footb_front", "footb_back");
            SetRunParticleSystem(GnomeRunParticlesPath);
            AddRunParticleTag("dust_0");
            AddRunParticleTag("dust_1");

            for (auto interchangeMat : GnomeInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }

            SetAggroRange(1000.0f);

            AddDamagedParticleSystem(DamageType_Element_Physical, GnomeDamageParticlePath);

            SetBodyMaterial(MaterialType_Type_Cloth);
            AddDamageSounds(Audio::GetStandardOnClothDamageSoundPaths());
        }
    }

    template <>
    void EnumeratePreloads<Item::GnomeWrench>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GnomeWrenchSkeletonPath);
        preloads.insert(Item::GnomeWrenchMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::Gnome>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert(Character::GnomeSkeletonPath);
        preloads.insert(Character::GnomeMatsetPath);

        for (auto interchangeMat : Character::GnomeInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }

        preloads.insert(Character::GnomeDamageParticlePath);
        preloads.insert(Character::GnomeRunParticlesPath);

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnClothDamageSoundPaths());
    }
}
