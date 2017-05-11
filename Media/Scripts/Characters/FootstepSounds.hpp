#pragma once

#include "Pathfinding/Path.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Audio
    {
        const SoundPathVector& GetStandardBasicFootstepSoundPaths();
        const SoundPathVector& GetHeavyBasicFootstepSoundPaths();
        const std::map<Pathfinding::TerrainType, SoundPathVector>& GetStandardTerrainFootstepSoundPaths();

        const std::map<Pathfinding::TerrainType, SoundPathVector>& GetStandardTerrainLeapFootstepSoundPaths();
        const std::map<Pathfinding::TerrainType, SoundPathVector>& GetStandardTerrainLandFootstepSoundPaths();
    }
}
