#pragma once

#include "Content/Preload.hpp"
#include "Character/CharacterTypes.hpp"

#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Audio
    {
        struct DamageSoundPaths
        {
            Character::MaterialType MaterialType;
            Character::DamageType DamageType;
            SoundPathVector Sounds;
        };

        const std::vector<DamageSoundPaths>& GetStandardOnFleshDamageSoundPaths();
        const std::vector<DamageSoundPaths>& GetStandardOnClothDamageSoundPaths();
        const std::vector<DamageSoundPaths>& GetStandardOnMetalDamageSoundPaths();
        const std::vector<DamageSoundPaths>& GetStandardOnBoneDamageSoundPaths();

        const SoundPathVector& GetStandardHealingSoundPaths();
    }

    void EnumerateDamageSoundPreloads(PreloadSet& preloads, const std::vector<Audio::DamageSoundPaths>& sounds);
}
