#include "Items/Armors/CookArmor.hpp"
#include "Items/ItemSounds.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string ApronArmorMaterialPath = "Skeletons/Items/Armors/empty_armor.polymatset";
        static const std::string ApronArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string ApronArmorSkeletonMaterialPath = "Skeletons/Items/Armors/apron_skel.polymatset";
        static const std::string ApronArmorIconMatsetPath = "HUD/items.hudmatset";

        Apron::Apron(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_apron_armor_name", ApronArmorMaterialPath, ApronArmorSkeletonPath, ApronArmorSkeletonMaterialPath)
        {
            SetArmorValue(1);
            SetIcon(ApronArmorIconMatsetPath, "icon_apron");
            SetTooltipDescription("item_apron_armor_description");
            SetTooltipFlavor("item_apron_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Cloth);
            AddSelectionSounds(GetClothArmorSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Apron>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::ApronArmorMaterialPath);
        preloads.insert(Item::ApronArmorSkeletonPath);
        preloads.insert(Item::ApronArmorSkeletonMaterialPath);
        preloads.insert(Item::ApronArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetClothArmorSelectionSounds());
    }
}
