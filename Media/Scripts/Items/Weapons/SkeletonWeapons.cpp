#include "Items/Weapons/SkeletonWeapons.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string SkeletonWeaponSkeletonPath = "Skeletons/Items/Weapons/BoneWeapons/skel_weapon.skel";
        static const std::string SkeletonWeaponAttachPointA = "origin";
        static const std::string SkeletonWeaponAttachPointB = "b";

        static const float SkeletonMeleeWeapDPS = 1.0f;
        static const float SkeletonRangedWeapDPS = 1.0f;

        static const std::string SkeletonAxeMatsetPath = "Skeletons/Items/Weapons/BoneWeapons/skelaxe.polymatset";

        SkelAxe::SkelAxe(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, SkeletonWeaponSkeletonPath, SkeletonAxeMatsetPath)
        {
            SetAttachPoints(SkeletonWeaponAttachPointA, SkeletonWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, SkeletonMeleeWeapDPS));
        }

        static const std::string SkeletonDaggerMatsetPath = "Skeletons/Items/Weapons/BoneWeapons/skeldagger.polymatset";

        SkelDagger::SkelDagger(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, SkeletonWeaponSkeletonPath, SkeletonDaggerMatsetPath)
        {
            SetAttachPoints(SkeletonWeaponAttachPointA, SkeletonWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, SkeletonMeleeWeapDPS));
        }

        static const std::string SkeletonHalberdMatsetPath = "Skeletons/Items/Weapons/BoneWeapons/skelhalberd.polymatset";

        SkelHalberd::SkelHalberd(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, SkeletonWeaponSkeletonPath, SkeletonHalberdMatsetPath)
        {
            SetAttachPoints(SkeletonWeaponAttachPointA, SkeletonWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, SkeletonMeleeWeapDPS));
        }

        static const std::string SkeletonScimitarMatsetPath = "Skeletons/Items/Weapons/BoneWeapons/skelscimitar.polymatset";

        SkelScimitar::SkelScimitar(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, SkeletonWeaponSkeletonPath, SkeletonScimitarMatsetPath)
        {
            SetAttachPoints(SkeletonWeaponAttachPointA, SkeletonWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, SkeletonMeleeWeapDPS));
        }

        static const std::string SkelBowSkeletonPath = "Skeletons/Items/Weapons/BoneWeapons/skel_weapon.skel";
        static const std::string SkelBowMatsetPath = "Skeletons/Items/Weapons/BoneWeapons/skelbow.polymatset";

        static const std::string SkelArrowSkeletonPath = "Skeletons/Items/Weapons/BoneWeapons/skelarrow.skel";
        static const std::string SkelArrowMatsetPath = "Skeletons/Items/Weapons/BoneWeapons/skelarrow.polymatset";
        static const std::string SkelArrowHitMatsetPath = "Skeletons/Items/Weapons/BoneWeapons/skelarrow_hit.polymatset";

        SkelBow::SkelBow(const ItemParameters& parameters)
            : Bow(parameters, SkelBowSkeletonPath, SkelBowMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, SkeletonRangedWeapDPS));
        }

        Character::Arrow* SkelBow::SpawnArrow(const Vector2f& position, const Character::Damage& dmg)
        {
            Character::Character *owner = GetOwner();
            assert(owner);

            Character::CharacterConstructor<Character::Arrow> constructor =
                Character::BindCharacterConstructor<Character::Arrow>(SkelArrowSkeletonPath, SkelArrowMatsetPath, SkelArrowHitMatsetPath,
                                                                      owner->GetID(), dmg);
            return owner->GetLevelLayer()->SpawnCharacter(position, "arrow", NULL, constructor);
        }
    }

    template <>
    void EnumeratePreloads<Item::SkelAxe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::SkeletonWeaponSkeletonPath);
        preloads.insert(Item::SkeletonAxeMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::SkelDagger>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::SkeletonWeaponSkeletonPath);
        preloads.insert(Item::SkeletonDaggerMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::SkelHalberd>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::SkeletonWeaponSkeletonPath);
        preloads.insert(Item::SkeletonHalberdMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::SkelScimitar>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::SkeletonWeaponSkeletonPath);
        preloads.insert(Item::SkeletonScimitarMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::SkelBow>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Bow>(preloads);

        preloads.insert(Item::SkelBowSkeletonPath);
        preloads.insert(Item::SkelBowMatsetPath);

        preloads.insert(Item::SkelArrowSkeletonPath);
        preloads.insert(Item::SkelArrowMatsetPath);
        preloads.insert(Item::SkelArrowHitMatsetPath);

        EnumeratePreloads<Character::Arrow>(preloads);
    }
}
