#include "Items/Weapons/GobboWeapons.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string GobWeaponSkeletonPath = "Skeletons/Items/Weapons/GobWeapons/gob_weapon.skel";
        static const std::string GobWeaponAttachPointA = "origin";
        static const std::string GobWeaponAttachPointB = "b";

        static const std::string GobBladeMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobblade.polymatset";

        static const float GobMeleeWeapDPS = 3.0f;
        static const float GobMeleeSwordDPS = 4.5f;

        GobBlade::GobBlade(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobBladeMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobDaggerMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobdagger.polymatset";

        GobDagger::GobDagger(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobDaggerMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobForkMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobfork.polymatset";

        GobFork::GobFork(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobForkMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobShankMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobshank.polymatset";

        GobShank::GobShank(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobShankMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobSwordMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobsword.polymatset";

        GobSword::GobSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobSwordMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, GobMeleeSwordDPS));
        }

        static const std::string GobTridentMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobtrident.polymatset";

        GobTrident::GobTrident(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobTridentMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobJawBladeMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobjawblade.polymatset";

        GobJawBlade::GobJawBlade(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobJawBladeMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetSplash(true);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobSkullClubMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobskullclub.polymatset";

        GobSkullClub::GobSkullClub(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobSkullClubMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetSplash(true);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobOrkAxeMatsetPath = "Skeletons/Items/Weapons/GobWeapons/goborkaxe.polymatset";

        GobOrkAxe::GobOrkAxe(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobOrkAxeMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetSplash(true);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobOrkSickleMatsetPath = "Skeletons/Items/Weapons/GobWeapons/goborksickle.polymatset";

        GobOrkSickle::GobOrkSickle(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, GobWeaponSkeletonPath, GobOrkSickleMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetSplash(true);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        static const std::string GobBowMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobbow.polymatset";

        static const std::string GobArrowSkeletonPath = "Skeletons/Items/Weapons/GobWeapons/gobarrow.skel";
        static const std::string GobArrowMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobarrow.polymatset";
        static const std::string GobArrowHitMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobarrow_hit.polymatset";

        static const float GobRangedWeapDPS = 1.5f;

        GobBow::GobBow(const ItemParameters& parameters)
            : Bow(parameters, GobWeaponSkeletonPath, GobBowMatsetPath)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, GobMeleeWeapDPS));
        }

        Character::Arrow* GobBow::SpawnArrow(const Vector2f& position, const Character::Damage& dmg)
        {
            Character::Character *owner = GetOwner();
            assert(owner);

            Character::CharacterConstructor<Character::Arrow> constructor =
                Character::BindCharacterConstructor<Character::Arrow>(GobArrowSkeletonPath, GobArrowMatsetPath, GobArrowHitMatsetPath,
                                                                      owner->GetID(), dmg);
            return owner->GetLevelLayer()->SpawnCharacter(position, "arrow", nullptr, constructor);
        }

        static const std::string GobThrowingRocksSkeletonPath = "Skeletons/Items/Weapons/GobWeapons/gob_weapon.skel";
        static const std::vector<std::string> GobThrowingRocksMatsetPaths =
        {
            "Skeletons/Items/Weapons/GobWeapons/gob_thrown_bone_0.polymatset",
            "Skeletons/Items/Weapons/GobWeapons/gob_thrown_rock_0.polymatset",
            "Skeletons/Items/Weapons/GobWeapons/gob_thrown_rock_1.polymatset",
            "Skeletons/Items/Weapons/GobWeapons/gob_thrown_wood_0.polymatset",
        };

        static const float GobThrownWeapDPS = 1.5f;
        static const float GobThrownWeapRange = 700.0f;

        static const Rotatorf GobThrowingRocksMinSpin = TwoPi * 20.0f;
        static const Rotatorf GobThrowingRocksMaxSpin = (TwoPi + Pi) * 20.0f;

        static const Audio::SoundPathVector GobThrowingRocksCollisionSounds =
        {
            { "Audio/Characters/Debris/Debris_bouncing.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_2.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_3.wav", 1.0f },
        };

        GobThrowingRocks::GobThrowingRocks(const ItemParameters& parameters)
            : ThrowingWeapon(parameters, "", GobThrowingRocksSkeletonPath, GobThrowingRocksMatsetPaths)
        {
            SetAttachPoints(GobWeaponAttachPointA, GobWeaponAttachPointB);
            SetRange(GobThrownWeapRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, GobThrownWeapDPS));

            SetSpinLimits(GobThrowingRocksMinSpin, GobThrowingRocksMaxSpin);

            SetMaterialCollisionSound(GobThrowingRocksCollisionSounds);
        }
    }

    template <>
    void EnumeratePreloads<Item::GobBlade>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobBladeMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobDagger>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobDaggerMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobFork>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobForkMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobShank>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobShankMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobSwordMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobTrident>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobTridentMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobJawBlade>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobJawBladeMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobSkullClub>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobSkullClubMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobOrkAxe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobOrkAxeMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobOrkSickle>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobOrkSickleMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::GobBow>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Bow>(preloads);

        preloads.insert(Item::GobWeaponSkeletonPath);
        preloads.insert(Item::GobBowMatsetPath);

        preloads.insert(Item::GobArrowSkeletonPath);
        preloads.insert(Item::GobArrowMatsetPath);
        preloads.insert(Item::GobArrowHitMatsetPath);

        EnumeratePreloads<Character::Arrow>(preloads);
    }

    template <>
    void EnumeratePreloads<Item::GobThrowingRocks>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);

        preloads.insert(Item::GobThrowingRocksSkeletonPath);
        preloads.insert(Item::GobThrowingRocksMatsetPaths.begin(), Item::GobThrowingRocksMatsetPaths.end());

        EnumerateSoundVectorPreloads(preloads, Item::GobThrowingRocksCollisionSounds);
    }
}
