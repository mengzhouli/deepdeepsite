#include "Characters/Dynamite.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const Audio::SoundPathVector DynamiteExplosionSoundPaths =
        {
            //{ "Audio/Characters/Pkeg/Keg_Kaboom_MASTER.ogg", 1.0f },
            { "Audio/Characters/Pkeg/Keg_Kaboom_wDebris_Long_MASTER.ogg", 1.0f },
            { "Audio/Characters/Pkeg/Keg_Kaboom_wDebris_short_MASTER.ogg", 1.0f },
        };

        static const std::string DynamiteSkeletonPath = "Skeletons/Characters/Pkeg/Pkeg.skel";
        static const std::string DynamiteMatsetPath = "Skeletons/Characters/Pkeg/Pkeg.polymatset";
        static const std::string DynamiteParticlesPath = "Particles/explosion_4.partsys";

        Dynamite::Dynamite(const CharacterParameters& parameters, CharacterID owner, float duration, float radius, float dmg)
            : Explosive(parameters, DynamiteSkeletonPath, DynamiteMatsetPath, "fuse", "origin", owner, duration, radius, dmg)
        {
            DisablePhysics();
            SetDrawSkeletonAfterExplosion(true);
            SetSkeletonJointStrength(0.0f);
            SetAttachToGroundOnSpawn(true);
            Stop();
            SetEntityMask(CharacterMask_Usable);
            SetTooltip("character_dynamite_name");
            AddExplosionSounds(DynamiteExplosionSoundPaths);
            SetExplosionParticles(DynamiteParticlesPath);
        }

        float Dynamite::OnPreInteractedWith(Character* interactor)
        {
            Toggle();
            return 0.0f;
        }
    }

    template <>
    void EnumeratePreloads<Character::Dynamite>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Explosive>(preloads);

        preloads.insert(Character::DynamiteSkeletonPath);
        preloads.insert(Character::DynamiteMatsetPath);
        preloads.insert(Character::DynamiteParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Character::DynamiteExplosionSoundPaths);
    }
}
