#include "Characters/FootstepSounds.hpp"

namespace Dwarf
{
    namespace Audio
    {
        static const Audio::SoundPathVector BaseFootstepSounds =
        {
            { "Audio/Footsteps/step_always1.ogg", 1.0f },
            { "Audio/Footsteps/step_always2.ogg", 1.0f },
        };
        
        static const Audio::SoundPathVector HeavyFootstepSounds =
        {
            { "Audio/Footsteps/step_OrcAlways1.wav", 1.0f },
            { "Audio/Footsteps/step_OrcAlways2.wav", 1.0f },
            { "Audio/Footsteps/step_OrcAlways3.wav", 1.0f },
        };

        static const std::map<Pathfinding::TerrainType, Audio::SoundPathVector> TerrainFootstepSounds =
        {
            {
                Pathfinding::TerrainType::Rock,
                {
                    { "Audio/Footsteps/step_rock1.ogg", 1.0f },
                    { "Audio/Footsteps/step_rock2.ogg", 1.0f },
                    { "Audio/Footsteps/step_rock3.ogg", 1.0f },
                    { "Audio/Footsteps/step_rock4.ogg", 1.0f },
                    { "Audio/Footsteps/step_gravel1.ogg", 1.0f },
                    { "Audio/Footsteps/step_gravel2.ogg", 1.0f },
                    { "Audio/Footsteps/step_gravel3.ogg", 1.0f },
                },
            },
            {
                Pathfinding::TerrainType::Gravel,
                {
                    { "Audio/Footsteps/step_gravel1.ogg", 1.0f },
                    { "Audio/Footsteps/step_gravel2.ogg", 1.0f },
                    { "Audio/Footsteps/step_gravel3.ogg", 1.0f },
                },
            },
            {
                Pathfinding::TerrainType::Wood,
                {
                    { "Audio/Footsteps/step_wood1.ogg", 1.0f },
                    { "Audio/Footsteps/step_wood2.ogg", 1.0f },
                    { "Audio/Footsteps/step_wood3.ogg", 1.0f },
                },
            },
        };

        static const std::map<Pathfinding::TerrainType, Audio::SoundPathVector> TerrainLeapFootstepSounds =
        {
            {
                Pathfinding::TerrainType::Rock,
                {
                    { "Audio/Footsteps/stepLeap_rock.ogg", 1.0f },
                    { "Audio/Footsteps/stepLeap_gravel.ogg", 1.0f },
                },
            },
            {
                Pathfinding::TerrainType::Gravel,
                {
                    { "Audio/Footsteps/stepLeap_gravel.ogg", 1.0f },
                },
            },
            {
                Pathfinding::TerrainType::Wood,
                {
                    { "Audio/Footsteps/stepLeap_wood.ogg", 1.0f },
                },
            },
        };

        static const std::map<Pathfinding::TerrainType, Audio::SoundPathVector> TerrainLandFootstepSounds =
        {
            {
                Pathfinding::TerrainType::Rock,
                {
                    { "Audio/Footsteps/stepLand_rock.ogg", 1.0f },
                    { "Audio/Footsteps/stepLand_gravel.ogg", 1.0f },
                },
            },
            {
                Pathfinding::TerrainType::Gravel,
                {
                    { "Audio/Footsteps/stepLand_gravel.ogg", 1.0f },
                },
            },
            {
                Pathfinding::TerrainType::Wood,
                {
                    { "Audio/Footsteps/stepLand_wood.ogg", 1.0f },
                },
            },
        };

        const SoundPathVector& GetStandardBasicFootstepSoundPaths()
        {
            return BaseFootstepSounds;
        }
        
        const SoundPathVector& GetHeavyBasicFootstepSoundPaths()
        {
            return HeavyFootstepSounds;
        }

        const std::map<Dwarf::Pathfinding::TerrainType, Dwarf::Audio::SoundPathVector>& GetStandardTerrainFootstepSoundPaths()
        {
            return TerrainFootstepSounds;
        }

        const std::map<Dwarf::Pathfinding::TerrainType, Dwarf::Audio::SoundPathVector>& GetStandardTerrainLeapFootstepSoundPaths()
        {
            return TerrainLeapFootstepSounds;
        }

        const std::map<Dwarf::Pathfinding::TerrainType, Dwarf::Audio::SoundPathVector>& GetStandardTerrainLandFootstepSoundPaths()
        {
            return TerrainLandFootstepSounds;
        }
    }
}
