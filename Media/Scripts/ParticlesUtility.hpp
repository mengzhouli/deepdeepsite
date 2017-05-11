#pragma once

#include "Particles/ParticleSystemInstance.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Character;
    }

    namespace Animation
    {
        class SkeletonInstance;
    }

    namespace Particles
    {
        void FitParticlesToOval(ParticleSystemInstance* particles, const Vector2f& position, const Vector2f& radius);
        void FitParticlesToSkeleton(ParticleSystemInstance* particles, const Animation::SkeletonInstance* skeleton);
        void FitParticlesToCharacter(ParticleSystemInstance* particles, const Character::Character* character);
    }
}
