#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {

        static const Audio::SoundPathVector WoodArmorSelectionSounds =
        {
            // TODO
        };

        static const Audio::SoundPathVector MetalArmorSelectionSounds =
        {
            { "Audio/Items/UI/item_Armor_1.wav" , 1.0f },
            { "Audio/Items/UI/item_Armor_2.wav" , 1.0f },
            { "Audio/Items/UI/item_Armor_3.wav" , 1.0f },
        };

        static const Audio::SoundPathVector ClothArmorSelectionSounds =
        {
            { "Audio/Items/UI/item_cloth_1.wav" , 1.0f },
            { "Audio/Items/UI/item_cloth_2.wav" , 1.0f },
            { "Audio/Items/UI/item_cloth_3.wav" , 1.0f },
        };

        static const Audio::SoundPathVector MetalTrinketSelectionSounds =
        {
            { "Audio/Items/UI/item_trinket_1.wav" , 1.0f },
            { "Audio/Items/UI/item_trinket_2.wav" , 1.0f },
        };

        static const Audio::SoundPathVector WoodTrinketSelectionSounds =
        {
            // TODO
            { "Audio/Items/UI/item_trinket_1.wav" , 1.0f },
            { "Audio/Items/UI/item_trinket_2.wav" , 1.0f },
        };

        static const Audio::SoundPathVector BookTrinketSelectionSounds =
        {
            // TODO
            { "Audio/Items/UI/item_trinket_1.wav" , 1.0f },
            { "Audio/Items/UI/item_trinket_2.wav" , 1.0f },
        };

        static const Audio::SoundPathVector WoodWeaponSelectionSounds =
        {
            // TODO
        };

        static const Audio::SoundPathVector MetalWeaponSelectionSounds =
        {
            { "Audio/Items/UI/item_Armor_1.wav" , 1.0f },
            { "Audio/Items/UI/item_Armor_2.wav" , 1.0f },
            { "Audio/Items/UI/item_Armor_3.wav" , 1.0f },
        };

        static const Audio::SoundPathVector SmallMetalWeaponSelectionSounds =
        {
            // TODO
        };

        const Audio::SoundPathVector& GetWoodArmorSelectionSounds()
        {
            return WoodArmorSelectionSounds;
        }

        const Audio::SoundPathVector& GetMetalArmorSelectionSounds()
        {
            return MetalArmorSelectionSounds;
        }

        const Audio::SoundPathVector& GetClothArmorSelectionSounds()
        {
            return ClothArmorSelectionSounds;
        }

        const Audio::SoundPathVector& GetMetalTrinketSelectionSounds()
        {
            return MetalTrinketSelectionSounds;
        }

        const Audio::SoundPathVector& GetWoodTrinketSelectionSounds()
        {
            return WoodTrinketSelectionSounds;
        }

        const Audio::SoundPathVector& GetBookTrinketSelectionSounds()
        {
            return BookTrinketSelectionSounds;
        }

        const Audio::SoundPathVector& GetWoodWeaponSelectionSounds()
        {
            return WoodWeaponSelectionSounds;
        }

        const Audio::SoundPathVector& GetMetalWeaponSelectionSounds()
        {
            return MetalWeaponSelectionSounds;
        }

        const Audio::SoundPathVector& GetSmallMetalWeaponSelectionSounds()
        {
            return SmallMetalWeaponSelectionSounds;
        }
    }
}
