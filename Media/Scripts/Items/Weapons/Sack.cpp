#include "Items/Weapons/Sack.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string SackSkeletonPath = "Skeletons/Items/Tools/Sack/sack.skel";
        static const std::string SackMatsetPath = "Skeletons/Items/Tools/Sack/sack.polymatset";
        static const std::string SackIconMatsetPath = "HUD/items.hudmatset";

        Sack::Sack(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, "item_sack_name", WeaponType_Special, SackSkeletonPath, SackMatsetPath)
        {
            SetAttachPoints("origin", "b");
            SetTooltipDescription("item_sack_description");
            SetTooltipFlavor("item_sack_flavor");
            SetIcon(SackIconMatsetPath, "icon_sack");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 0.0f));
            SetArmorAmmount(1.5f);
            AddSelectionSounds(GetClothArmorSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Sack>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::SackSkeletonPath);
        preloads.insert(Item::SackMatsetPath);
        preloads.insert(Item::SackIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetClothArmorSelectionSounds());
    }
}
