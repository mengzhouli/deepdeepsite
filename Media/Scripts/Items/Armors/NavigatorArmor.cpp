#include "Items/Armors/NavigatorArmor.hpp"
#include "DamageTypes.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string NavigatorTunicArmorMaterialPath = "Skeletons/Items/Armors/empty_armor.polymatset";
        static const std::string NavigatorTunicArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string NavigatorTunicArmorSkeletonMaterialPath = "Skeletons/Items/Armors/navigator_tunic_skel.polymatset";
        static const std::string NavigatorTunicArmorIconMatsetPath = "HUD/items.hudmatset";

        NavigatorTunic::NavigatorTunic(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_nav_tunic_armor_name", NavigatorTunicArmorMaterialPath, NavigatorTunicArmorSkeletonPath, NavigatorTunicArmorSkeletonMaterialPath)
        {
            SetArmorValue(0);
            SetIcon(NavigatorTunicArmorIconMatsetPath, "icon_navtunic");
            SetTooltipDescription("item_nav_tunic_armor_description");
            SetTooltipFlavor("item_nav_tunic_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Cloth);
            AddSelectionSounds(GetClothArmorSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::NavigatorTunic>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::NavigatorTunicArmorMaterialPath);
        preloads.insert(Item::NavigatorTunicArmorSkeletonPath);
        preloads.insert(Item::NavigatorTunicArmorSkeletonMaterialPath);
        preloads.insert(Item::NavigatorTunicArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetClothArmorSelectionSounds());
    }
}
