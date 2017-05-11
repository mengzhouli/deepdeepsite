#include "ParticlesUtility.hpp"

#include "Animation/SkeletonInstance.hpp"

#include "Character/Character.hpp"
#include "Characters/SkeletonCharacter.hpp"

namespace Dwarf
{
    namespace Particles
    {
        void FitParticlesToOval(ParticleSystemInstance* particles, const Vector2f& position, const Vector2f& radius)
        {
            Particles::ParticleSpawnFunction spawnerFunc = [=]
            {
                Vector2f rotator = Vector2f::FromRotation(Random::RandomBetween(0.0f, TwoPi), 1.0f);
                return position + (rotator * (radius * 0.5f));
            };
            Particles::ParticleBoundsFunction boundsFunc = [=]
            {
                return Rectanglef(position - radius, radius * 2.0f);
            };

            particles->SetFunctionSpawner(spawnerFunc, boundsFunc);
        }

        void FitParticlesToSkeleton(ParticleSystemInstance* particles, const Animation::SkeletonInstance* skeleton)
        {
            std::vector<Polygonf> materials;
            for (uint32_t i = 0; i < skeleton->GetJointCount(); i++)
            {
                auto material = skeleton->GetMaterial(i);
                if (material)
                {
                    materials.push_back(skeleton->GetJointBounds(i));
                }
            }

            particles->SetPolygonSpawners(materials);
        }

        void FitParticlesToCharacter(ParticleSystemInstance* particles, const Character::Character* character)
        {
            const Character::SkeletonCharacter* skeletonCharacter = AsA<Character::SkeletonCharacter>(character);
            if (skeletonCharacter != nullptr)
            {
                FitParticlesToSkeleton(particles, skeletonCharacter->GetSkeleton());
            }
            else
            {
                const Rectanglef& bounds = character->GetBounds();
                FitParticlesToOval(particles, bounds.Middle(), bounds.Size * 0.5f);
            }
        }
    }
}
