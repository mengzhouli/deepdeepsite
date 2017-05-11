#pragma once

#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Item
    {
        const Audio::SoundPathVector& GetWoodArmorSelectionSounds();
        const Audio::SoundPathVector& GetMetalArmorSelectionSounds();
        const Audio::SoundPathVector& GetClothArmorSelectionSounds();

        const Audio::SoundPathVector& GetMetalTrinketSelectionSounds();
        const Audio::SoundPathVector& GetWoodTrinketSelectionSounds();
        const Audio::SoundPathVector& GetBookTrinketSelectionSounds();

        const Audio::SoundPathVector& GetWoodWeaponSelectionSounds();
        const Audio::SoundPathVector& GetMetalWeaponSelectionSounds();
        const Audio::SoundPathVector& GetSmallMetalWeaponSelectionSounds();
    }
}
