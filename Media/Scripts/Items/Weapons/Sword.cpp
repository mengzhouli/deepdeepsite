#include "Items/Weapons/Sword.hpp"
#include "Items/ItemSounds.hpp"

#include "Random.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string WoodenShortSworSkeletonPath = "Skeletons/Items/Weapons/Swords/sword.skel";
        static const std::string WoodenShortSwordMatsetPath = "Skeletons/Items/Weapons/Swords/sword_wood.polymatset";
        static const std::string SwordIconMatsetPath = "HUD/items.hudmatset";

        WoodenShortSword::WoodenShortSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "item_woodenshortsword_name", WeaponType_Melee_1H, WoodenShortSworSkeletonPath, WoodenShortSwordMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_woodenshortsword_description");
            SetTooltipFlavor("item_woodenshortsword_flavor");
            SetIcon(SwordIconMatsetPath, "icon_woodenshortsword");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 2.5f));
            AddSelectionSounds(GetWoodWeaponSelectionSounds());
        }

        static const std::string RustyIronShortSwordSkeletonPath = "Skeletons/Items/Weapons/Swords/sword.skel";
        static const std::string RustyIronShortSwordMatsetPath = "Skeletons/Items/Weapons/Swords/sword_rust.polymatset";

        RustyIronShortSword::RustyIronShortSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "item_rustyironshortsword_name", WeaponType_Melee_1H, RustyIronShortSwordSkeletonPath, RustyIronShortSwordMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_rustyironshortsword_description");
            SetTooltipFlavor("item_rustyironshortsword_flavor");
            SetIcon(SwordIconMatsetPath, "icon_rustyironshortsword");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, 3.0f));
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string IronShortSwordSkeletonPath = "Skeletons/Items/Weapons/Swords/sword.skel";
        static const std::string IronShortSwordMatsetPath = "Skeletons/Items/Weapons/Swords/sword_basic.polymatset";

        IronShortSword::IronShortSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "item_ironshortsword_name", WeaponType_Melee_1H, IronShortSwordSkeletonPath, IronShortSwordMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_ironshortsword_description");
            SetTooltipFlavor("item_ironshortsword_flavor");
            SetIcon(SwordIconMatsetPath, "icon_ironshortsword");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, 4.0f));
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string IgnitionPinSwordSkeletonPath = "Skeletons/Items/Weapons/Swords/sword.skel";
        static const std::string IgnitionPinSwordMatsetPath = "Skeletons/Items/Weapons/Swords/sword_ignitionpin.polymatset";

        IgnitionPinSword::IgnitionPinSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "item_ignitionpinsword_name", WeaponType_Melee_1H, IgnitionPinSwordSkeletonPath, IgnitionPinSwordMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_ignitionpinsword_description");
            SetTooltipFlavor("item_ignitionpinsword_flavor");
            SetIcon(SwordIconMatsetPath, "icon_ignitionpinsword");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, 4.0f));
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string LettersBaneSwordSkeletonPath = "Skeletons/Items/Weapons/Swords/sword.skel";
        static const std::string LettersBaneSwordMatsetPath = "Skeletons/Items/Weapons/Swords/sword_lettersbane.polymatset";

        LettersbaneSword::LettersbaneSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "item_lettersbanesword_name", WeaponType_Melee_1H, LettersBaneSwordSkeletonPath, LettersBaneSwordMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_lettersbanesword_description");
            SetTooltipFlavor("item_lettersbanesword_flavor");
            SetIcon(SwordIconMatsetPath, "icon_lettersbanesword");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, 4.0f));
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string UncalculatedDivisionSwordSkeletonPath = "Skeletons/Items/Weapons/Swords/sword.skel";
        static const std::string UncalculatedDivisionSwordMatsetPath = "Skeletons/Items/Weapons/Swords/sword_uncalculateddivision.polymatset";

        UncalculatedDivisionSword::UncalculatedDivisionSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "item_uncalculateddivisionsword_name", WeaponType_Melee_1H, UncalculatedDivisionSwordSkeletonPath, UncalculatedDivisionSwordMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_uncalculateddivisionsword_description");
            SetTooltipFlavor("item_uncalculateddivisionsword_flavor");
            SetIcon(SwordIconMatsetPath, "icon_uncalculateddivisionsword");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, 4.0f));
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::WoodenShortSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::WoodenShortSworSkeletonPath);
        preloads.insert(Item::WoodenShortSwordMatsetPath);
        preloads.insert(Item::SwordIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::RustyIronShortSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::RustyIronShortSwordSkeletonPath);
        preloads.insert(Item::RustyIronShortSwordMatsetPath);
        preloads.insert(Item::SwordIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::IronShortSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::IronShortSwordSkeletonPath);
        preloads.insert(Item::IronShortSwordMatsetPath);
        preloads.insert(Item::SwordIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::IgnitionPinSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::IgnitionPinSwordSkeletonPath);
        preloads.insert(Item::IgnitionPinSwordMatsetPath);
        preloads.insert(Item::SwordIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::LettersbaneSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::LettersBaneSwordSkeletonPath);
        preloads.insert(Item::LettersBaneSwordMatsetPath);
        preloads.insert(Item::SwordIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::UncalculatedDivisionSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::UncalculatedDivisionSwordSkeletonPath);
        preloads.insert(Item::UncalculatedDivisionSwordMatsetPath);
        preloads.insert(Item::SwordIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }
}
