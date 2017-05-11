#include "Items/Weapons/Shield.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string ShieldIconMatsetPath = "HUD/items.hudmatset";

        Shield::Shield(const ItemParameters& parameters, const std::string& nameCode, WeaponType type,
                       const std::string& skeletonPath, const std::string& matsetPath)
            : BasicWeapon(parameters, nameCode, type, skeletonPath, matsetPath)
        {
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Element_Physical, 0.0f));
        }

        static const std::string BucklerSkeletonPath = "Skeletons/Items/Weapons/Shields/shield.skel";
        static const std::string BucklerMatsetPath = "Skeletons/Items/Weapons/Shields/shield_woodenbasic.polymatset";

        WoodenBasicShield::WoodenBasicShield(const ItemParameters& parameters)
            : Shield(parameters, "item_woodenbasicshield_name", WeaponType_Melee_Shield, BucklerSkeletonPath, BucklerMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_woodenbasicshield_description");
            SetTooltipFlavor("item_woodenbasicshield_flavor");
            SetIcon(ShieldIconMatsetPath, "icon_woodenbasicshield");

            SetArmorAmmount(2.0f);
            SetBlockChance(Character::DamageType_Element_Physical, 0.3f, 1.0f);

            AddSelectionSounds(GetWoodWeaponSelectionSounds());
        }

        static const std::string WoodenHeaterSkeletonPath = "Skeletons/Items/Weapons/Shields/shield.skel";
        static const std::string WoodenHeaterMatsetPath = "Skeletons/Items/Weapons/Shields/shield_heater.polymatset";

        WoodenHeater::WoodenHeater(const ItemParameters& parameters)
            : Shield(parameters, "item_woodenheater_name", WeaponType_Melee_Shield, WoodenHeaterSkeletonPath, WoodenHeaterMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_woodenheater_description");
            SetTooltipFlavor("item_woodenheater_flavor");
            SetIcon(ShieldIconMatsetPath, "icon_woodenheater");

            SetArmorAmmount(2.0f);
            SetBlockChance(Character::DamageType_Element_Physical, 0.3f, 2.0f);

            AddSelectionSounds(GetWoodWeaponSelectionSounds());
        }

        static const std::string BracedWoodenHeaterSkeletonPath = "Skeletons/Items/Weapons/Shields/shield.skel";
        static const std::string BracedWoodenHeaterMatsetPath = "Skeletons/Items/Weapons/Shields/shield_heaterbrace.polymatset";

        BracedWoodenHeater::BracedWoodenHeater(const ItemParameters& parameters)
            : Shield(parameters, "item_bracedwoodenheater_name", WeaponType_Melee_Shield, BracedWoodenHeaterSkeletonPath, BracedWoodenHeaterMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_bracedwoodenheater_description");
            SetTooltipFlavor("item_bracedwoodenheater_flavor");
            SetIcon(ShieldIconMatsetPath, "icon_bracedwoodenheater");

            SetArmorAmmount(2.5f);

            AddSelectionSounds(GetWoodWeaponSelectionSounds());
        }

        static const std::string IronHeaterSkeletonPath = "Skeletons/Items/Weapons/Shields/shield.skel";
        static const std::string IronHeaterMatsetPath = "Skeletons/Items/Weapons/Shields/shield_heateriron.polymatset";

        IronHeater::IronHeater(const ItemParameters& parameters)
            : Shield(parameters, "item_ironheater_name", WeaponType_Melee_Shield, IronHeaterSkeletonPath, IronHeaterMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_ironheater_description");
            SetTooltipFlavor("item_ironheater_flavor");
            SetIcon(ShieldIconMatsetPath, "icon_ironheater");

            SetArmorAmmount(3.0f);
            SetBlockChance(Character::DamageType_Element_Physical, 0.4f, 3.0f);
            SetBlockChance(Character::DamageType_Element_Fire, 0.5f, 5.0f);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string WoodenRoundShieldSkeletonPath = "Skeletons/Items/Weapons/Shields/shield.skel";
        static const std::string WoodenRoundShieldMatsetPath = "Skeletons/Items/Weapons/Shields/shield_gap.polymatset";

        WoodenRoundShield::WoodenRoundShield(const ItemParameters& parameters)
            : Shield(parameters, "item_woodenroundshield_name", WeaponType_Melee_Shield, WoodenRoundShieldSkeletonPath, WoodenRoundShieldMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_woodenroundshield_description");
            SetTooltipFlavor("item_woodenroundshield_flavor");
            SetIcon(ShieldIconMatsetPath, "icon_woodenroundshield");

            SetArmorAmmount(3.0f);

            AddSelectionSounds(GetWoodWeaponSelectionSounds());
        }

        static const std::string TowerShieldSkeletonPath = "Skeletons/Items/Weapons/Shields/shield.skel";
        static const std::string TowerShieldMatsetPath = "Skeletons/Items/Weapons/Shields/shield_orkboss.polymatset";

        TowerShield::TowerShield(const ItemParameters& parameters)
            : Shield(parameters, "item_towershield_name", WeaponType_Melee_Shield, TowerShieldSkeletonPath, TowerShieldMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_towershield_description");
            SetTooltipFlavor("item_towershield_flavor");
            SetIcon(ShieldIconMatsetPath, "icon_towershield");

            SetArmorAmmount(5.0f);
            SetBlockChance(Character::DamageType_Element_Physical, 0.3f, 10.0f);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Shield>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicWeapon>(preloads);
    }

    template <>
    void EnumeratePreloads<Item::WoodenBasicShield>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Shield>(preloads);

        preloads.insert(Item::BucklerSkeletonPath);
        preloads.insert(Item::BucklerMatsetPath);

        preloads.insert(Item::ShieldIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::WoodenHeater>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Shield>(preloads);

        preloads.insert(Item::WoodenHeaterSkeletonPath);
        preloads.insert(Item::WoodenHeaterMatsetPath);

        preloads.insert(Item::ShieldIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BracedWoodenHeater>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Shield>(preloads);

        preloads.insert(Item::BracedWoodenHeaterSkeletonPath);
        preloads.insert(Item::BracedWoodenHeaterMatsetPath);

        preloads.insert(Item::ShieldIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::IronHeater>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Shield>(preloads);

        preloads.insert(Item::IronHeaterSkeletonPath);
        preloads.insert(Item::IronHeaterMatsetPath);

        preloads.insert(Item::ShieldIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::WoodenRoundShield>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Shield>(preloads);

        preloads.insert(Item::WoodenRoundShieldSkeletonPath);
        preloads.insert(Item::WoodenRoundShieldMatsetPath);

        preloads.insert(Item::ShieldIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::TowerShield>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Shield>(preloads);

        preloads.insert(Item::TowerShieldSkeletonPath);
        preloads.insert(Item::TowerShieldMatsetPath);

        preloads.insert(Item::ShieldIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }
}
