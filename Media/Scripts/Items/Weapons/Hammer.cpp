#include "Items/Weapons/Hammer.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string HammerSkeletonPath = "Skeletons/Items/Tools/Hammer/hammer.skel";
        static const std::string HammerIconMatsetPath = "HUD/items.hudmatset";

        Hammer::Hammer(const ItemParameters& parameters, const std::string& name, float buildMult, const std::string& matsetPath)
            : BasicMeleeWeapon(parameters, name, WeaponType_Melee_1H, HammerSkeletonPath, matsetPath)
            , _buildRateMult(buildMult)
        {
            SetAttachPoints("origin", "b");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 2.0f));
        }

        float Hammer::GetBuildRateMultiplier() const
        {
            return _buildRateMult;
        }

        static const std::string WoodHammerMatsetPath = "Skeletons/Items/Tools/Hammer/hammer_wood.polymatset";

        WoodHammer::WoodHammer(const ItemParameters& parameters)
            : Hammer(parameters, "item_woodhammer_name", 1.0f, WoodHammerMatsetPath)
        {
            SetTooltipDescription("item_woodhammer_description");
            SetTooltipFlavor("item_woodhammer_flavor");
            SetIcon(HammerIconMatsetPath, "icon_woodhammer");
            AddSelectionSounds(GetWoodWeaponSelectionSounds());
        }

        static const std::string IronHammerMatsetPath = "Skeletons/Items/Tools/Hammer/hammer_iron.polymatset";

        IronHammer::IronHammer(const ItemParameters& parameters)
            : Hammer(parameters, "item_ironhammer_name", 1.1f, IronHammerMatsetPath)
        {
            SetTooltipDescription("item_ironhammer_description");
            SetTooltipFlavor("item_ironhammer_flavor");
            SetIcon(HammerIconMatsetPath, "icon_ironhammer");
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string GoldHammerMatsetPath = "Skeletons/Items/Tools/Hammer/hammer_gold.polymatset";

        GoldHammer::GoldHammer(const ItemParameters& parameters)
            : Hammer(parameters, "item_goldhammer_name", 1.2f, GoldHammerMatsetPath)
        {
            SetTooltipDescription("item_goldhammer_description");
            SetTooltipFlavor("item_goldhammer_flavor");
            SetIcon(HammerIconMatsetPath, "icon_goldhammer");
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Hammer>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::HammerSkeletonPath);
    }

    template <>
    void EnumeratePreloads<Item::WoodHammer>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Hammer>(preloads);

        preloads.insert(Item::WoodHammerMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::IronHammer>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Hammer>(preloads);

        preloads.insert(Item::IronHammerMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::GoldHammer>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Hammer>(preloads);

        preloads.insert(Item::GoldHammerMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }
}
