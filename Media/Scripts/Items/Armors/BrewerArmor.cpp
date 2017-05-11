#include "Items/Armors/BrewerArmor.hpp"
#include "Items/ItemSounds.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string BrewerTunicArmorMaterialPath = "Skeletons/Items/Armors/empty_armor.polymatset";
        static const std::string BrewerTunicArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string BrewerTunicArmorSkeletonMaterialPath = "Skeletons/Items/Armors/brewer_tunic_skel.polymatset";
        static const std::string BrewerTunicArmorIconMatsetPath = "HUD/items.hudmatset";

        BrewerTunic::BrewerTunic(const ItemParameters& parameters) : BasicArmor(parameters, "item_brew_tunic_armor_name", BrewerTunicArmorMaterialPath, BrewerTunicArmorSkeletonPath, BrewerTunicArmorSkeletonMaterialPath)
        {
            SetArmorValue(0);
            SetIcon(BrewerTunicArmorIconMatsetPath, "icon_tunic");
            SetTooltipDescription("item_brew_tunic_armor_description");
            SetTooltipFlavor("item_brew_tunic_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Cloth);
            AddSelectionSounds(GetClothArmorSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::BrewerTunic>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::BrewerTunicArmorMaterialPath);
        preloads.insert(Item::BrewerTunicArmorSkeletonPath);
        preloads.insert(Item::BrewerTunicArmorSkeletonMaterialPath);
        preloads.insert(Item::BrewerTunicArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetClothArmorSelectionSounds());
    }
}
