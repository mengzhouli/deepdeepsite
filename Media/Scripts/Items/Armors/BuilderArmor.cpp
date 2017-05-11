#include "Items/Armors/BuilderArmor.hpp"
#include "Items/ItemSounds.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string BuilderTunicArmorMaterialPath = "Skeletons/Items/Armors/empty_armor.polymatset";
        static const std::string BuilderTunicArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string BuilderTunicArmorSkeletonMaterialPath = "Skeletons/Items/Armors/builder_tunic_skel.polymatset";
        static const std::string BuilderTunicArmorIconMatsetPath = "HUD/items.hudmatset";

        BuilderTunic::BuilderTunic(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_builder_tunic_armor_name", BuilderTunicArmorMaterialPath, BuilderTunicArmorSkeletonPath, BuilderTunicArmorSkeletonMaterialPath)
        {
            SetArmorValue(0);
            SetIcon(BuilderTunicArmorIconMatsetPath, "icon_buildertunic");
            SetTooltipDescription("item_builder_tunic_armor_description");
            SetTooltipFlavor("item_builder_tunic_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Cloth);
            AddSelectionSounds(GetClothArmorSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::BuilderTunic>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::BuilderTunicArmorMaterialPath);
        preloads.insert(Item::BuilderTunicArmorSkeletonPath);
        preloads.insert(Item::BuilderTunicArmorSkeletonMaterialPath);
        preloads.insert(Item::BuilderTunicArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetClothArmorSelectionSounds());
    }
}
