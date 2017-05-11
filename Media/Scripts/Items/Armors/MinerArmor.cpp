#include "Items/Armors/MinerArmor.hpp"
#include "Items/ItemSounds.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string MinerTunicArmorMaterialPath = "Skeletons/Items/Armors/empty_armor.polymatset";
        static const std::string MinerTunicArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string MinerTunicArmorSkeletonMaterialPath = "Skeletons/Items/Armors/miner_tunic_skel.polymatset";
        static const std::string MinerTunicArmorIconMatsetPath = "HUD/items.hudmatset";

        MinerTunic::MinerTunic(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_miner_tunic_armor_name", MinerTunicArmorMaterialPath, MinerTunicArmorSkeletonPath, MinerTunicArmorSkeletonMaterialPath)
        {
            SetArmorValue(0);
            SetIcon(MinerTunicArmorIconMatsetPath, "icon_minertunic");
            SetTooltipDescription("item_miner_tunic_armor_description");
            SetTooltipFlavor("item_miner_tunic_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Cloth);
            AddSelectionSounds(GetClothArmorSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::MinerTunic>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::MinerTunicArmorMaterialPath);
        preloads.insert(Item::MinerTunicArmorSkeletonPath);
        preloads.insert(Item::MinerTunicArmorSkeletonMaterialPath);
        preloads.insert(Item::MinerTunicArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetClothArmorSelectionSounds());
    }
}
