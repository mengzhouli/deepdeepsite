#include "Items/Armors/FighterArmor.hpp"
#include "DamageTypes.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string FighterArmorIconMatsetPath = "HUD/items.hudmatset";

        static const std::string ChainmailArmorMaterialPath = "Skeletons/Items/Armors/empty_armor.polymatset";
        static const std::string ChainmailArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string ChainmailArmorSkeletonMaterialPath = "Skeletons/Items/Armors/chainmail_armor_skel.polymatset";

        Chainmail::Chainmail(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_chainmail_armor_name", ChainmailArmorMaterialPath, ChainmailArmorSkeletonPath, ChainmailArmorSkeletonMaterialPath)
        {
            SetArmorValue(3);
            SetIcon(FighterArmorIconMatsetPath, "icon_chainmail");
            SetTooltipDescription("item_chainmail_armor_description");
            SetTooltipFlavor("item_chainmail_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Metal);
            AddSelectionSounds(GetMetalArmorSelectionSounds());
        }

        static const std::string BristleNogginArmorMaterialPath = "Skeletons/Items/Armors/bristlenoggin_armor.polymatset";
        static const std::string BristleNogginArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string BristleNogginArmorSkeletonMaterialPath = "Skeletons/Items/Armors/bristlenoggin_armor_skel.polymatset";

        BristleNogginArmor::BristleNogginArmor(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_bristlenoggin_armor_name", BristleNogginArmorMaterialPath, BristleNogginArmorSkeletonPath, BristleNogginArmorSkeletonMaterialPath)
        {
            SetArmorValue(4);
            SetIcon(FighterArmorIconMatsetPath, "icon_bristlenogginarmor");
            SetTooltipDescription("item_bristlenoggin_armor_description");
            SetTooltipFlavor("item_bristlenoggin_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Metal);
            AddSelectionSounds(GetMetalArmorSelectionSounds());
        }

        static const std::string BronzeArmorMaterialPath = "Skeletons/Items/Armors/bronze_armor.polymatset";
        static const std::string BronzeArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string BronzeArmorSkeletonMaterialPath = "Skeletons/Items/Armors/bronze_armor_skel.polymatset";

        BronzeArmor::BronzeArmor(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_bronze_armor_name", BronzeArmorMaterialPath, BronzeArmorSkeletonPath, BronzeArmorSkeletonMaterialPath)
        {
            SetArmorValue(4);
            SetIcon(FighterArmorIconMatsetPath, "icon_bronzearmor");
            SetTooltipDescription("item_bronze_armor_description");
            SetTooltipFlavor("item_bronze_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Metal);
            AddSelectionSounds(GetMetalArmorSelectionSounds());
        }

        static const std::string TwinFangArmorMaterialPath = "Skeletons/Items/Armors/twinfang_armor.polymatset";
        static const std::string TwinFangArmorSkeletonPath = "Skeletons/Items/Armors/basic_armor.skel";
        static const std::string TwinFangArmorSkeletonMaterialPath = "Skeletons/Items/Armors/twinfang_armor_skel.polymatset";

        TwinFangArmor::TwinFangArmor(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_twinfang_armor_name", TwinFangArmorMaterialPath, TwinFangArmorSkeletonPath, TwinFangArmorSkeletonMaterialPath)
        {
            SetArmorValue(4);
            SetIcon(FighterArmorIconMatsetPath, "icon_twinfangarmor");
            SetTooltipDescription("item_twinfang_armor_description");
            SetTooltipFlavor("item_twinfang_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Metal);
            AddSelectionSounds(GetMetalArmorSelectionSounds());
        }

        static const std::string BladeArmorMaterialPath = "Skeletons/Items/Armors/blade_armor.polymatset";
        static const std::string BladeArmorSkeletonPath = "";
        static const std::string BladeArmorSkeletonMaterialPath = "";

        BladeArmor::BladeArmor(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_blade_armor_name", BladeArmorMaterialPath, BladeArmorSkeletonPath, BladeArmorSkeletonMaterialPath)
        {
            SetArmorValue(4);
            SetIcon(FighterArmorIconMatsetPath, "icon_bladearmor");
            SetTooltipDescription("item_blade_armor_description");
            SetTooltipFlavor("item_blade_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Metal);
            AddSelectionSounds(GetMetalArmorSelectionSounds());
        }


        static const std::string TeapotArmorMaterialPath = "Skeletons/Items/Armors/teapot_armor.polymatset";
        static const std::string TeapotArmorSkeletonPath = "";
        static const std::string TeapotArmorSkeletonMaterialPath = "";

        TeapotArmor::TeapotArmor(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_teapot_armor_name", TeapotArmorMaterialPath, TeapotArmorSkeletonPath, TeapotArmorSkeletonMaterialPath)
        {
            SetArmorValue(5);
            SetIcon(FighterArmorIconMatsetPath, "icon_teapotarmor");
            SetTooltipDescription("item_teapot_armor_description");
            SetTooltipFlavor("item_teapot_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Metal);
            AddSelectionSounds(GetMetalArmorSelectionSounds());
        }


        static const std::string WokArmorMaterialPath = "Skeletons/Items/Armors/wok_armor.polymatset";
        static const std::string WokArmorSkeletonPath = "";
        static const std::string WokArmorSkeletonMaterialPath = "";

        WokArmor::WokArmor(const ItemParameters& parameters)
            : BasicArmor(parameters, "item_wok_armor_name", WokArmorMaterialPath, WokArmorSkeletonPath, WokArmorSkeletonMaterialPath)
        {
            SetArmorValue(6);
            SetIcon(FighterArmorIconMatsetPath, "icon_wokarmor");
            SetTooltipDescription("item_wok_armor_description");
            SetTooltipFlavor("item_wok_armor_flavor");
            SetMaterial(Character::MaterialType_Type_Metal);
            AddSelectionSounds(GetMetalArmorSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Chainmail>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::ChainmailArmorMaterialPath);
        preloads.insert(Item::ChainmailArmorSkeletonPath);
        preloads.insert(Item::ChainmailArmorSkeletonMaterialPath);
        preloads.insert(Item::FighterArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalArmorSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BristleNogginArmor>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::BristleNogginArmorMaterialPath);
        preloads.insert(Item::BristleNogginArmorSkeletonPath);
        preloads.insert(Item::BristleNogginArmorSkeletonMaterialPath);
        preloads.insert(Item::FighterArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalArmorSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BronzeArmor>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::BronzeArmorMaterialPath);
        preloads.insert(Item::BronzeArmorSkeletonPath);
        preloads.insert(Item::BronzeArmorSkeletonMaterialPath);
        preloads.insert(Item::FighterArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalArmorSelectionSounds());
    }
    
    template <>
    void EnumeratePreloads<Item::TwinFangArmor>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::TwinFangArmorMaterialPath);
        preloads.insert(Item::TwinFangArmorSkeletonPath);
        preloads.insert(Item::TwinFangArmorSkeletonMaterialPath);
        preloads.insert(Item::FighterArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalArmorSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BladeArmor>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::BladeArmorMaterialPath);
        preloads.insert(Item::BladeArmorSkeletonPath);
        preloads.insert(Item::BladeArmorSkeletonMaterialPath);
        preloads.insert(Item::FighterArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalArmorSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::TeapotArmor>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::TeapotArmorMaterialPath);
        preloads.insert(Item::TeapotArmorSkeletonPath);
        preloads.insert(Item::TeapotArmorSkeletonMaterialPath);
        preloads.insert(Item::FighterArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalArmorSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::WokArmor>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicArmor>(preloads);
        preloads.insert(Item::WokArmorMaterialPath);
        preloads.insert(Item::WokArmorSkeletonPath);
        preloads.insert(Item::WokArmorSkeletonMaterialPath);
        preloads.insert(Item::FighterArmorIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalArmorSelectionSounds());
    }
}
