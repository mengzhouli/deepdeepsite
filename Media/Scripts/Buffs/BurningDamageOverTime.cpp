#include "Buffs/BurningDamageOverTime.hpp"
#include "Characters/BasicCharacter.hpp"
#include "DamageTypes.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        const std::string FlameParticlesPath = "Particles/fire06.partsys";
        const float FlameParticlesScale = 1.0f;

        BurningDamageOverTime::BurningDamageOverTime(const BuffComponentParameters& parameters, CharacterID source, float dps, float tickRate)
            : DamageOverTime(parameters, source, Damage(DamageType_Source_Magic | DamageType_Element_Fire, dps), tickRate)
            , _flame(Graphics::DefaultFlameColor, false, false, true, true, parameters.owner->GetLevel()->GetSoundManager(), FlameParticlesPath)
        {
            _flame.SetScale(FlameParticlesScale);
        }

        void BurningDamageOverTime::OnLoadContent(Content::ContentManager* contentManager)
        {
            DamageOverTime::OnLoadContent(contentManager);

            _flame.LoadContent(contentManager);
        }

        void BurningDamageOverTime::OnUnloadContent()
        {
            DamageOverTime::OnUnloadContent();

            _flame.UnloadContent();
        }

        void BurningDamageOverTime::OnApply()
        {
            DamageOverTime::OnApply();

            updateFlamePositions();
            _flame.SetState(true);
        }

        void BurningDamageOverTime::OnFinish()
        {
            DamageOverTime::OnFinish();

            _flame.SetState(false);
        }

        bool BurningDamageOverTime::CanTerminate() const
        {
            return DamageOverTime::CanTerminate() && _flame.GetParticleCount() == 0;
        }

        void BurningDamageOverTime::OnUpdate(double totalTime, float dt)
        {
            DamageOverTime::OnUpdate(totalTime, dt);

            updateFlamePositions();

            _flame.Update(totalTime, dt);
        }

        void BurningDamageOverTime::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            DamageOverTime::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(&_flame, false);
            if (_flame.ShouldDrawLights())
            {
                levelRenderer->AddLight(_flame.GetCenterLight());
                levelRenderer->AddLight(_flame.GetMainLight());
            }
        }

        void BurningDamageOverTime::updateFlamePositions()
        {
            Character* owner = GetOwner();
            const Rectanglef& bounds = owner->GetBounds();
            SkeletonCharacter* skeletonOwner = AsA<SkeletonCharacter>(owner);
            if (skeletonOwner != nullptr)
            {
                auto skeleton = skeletonOwner->GetSkeleton();

                std::vector<Polygonf> materials;
                for (uint32_t i = 0; i < skeleton->GetJointCount(); i++)
                {
                    auto material = skeleton->GetMaterial(i);
                    if (material)
                    {
                        materials.push_back(skeleton->GetJointBounds(i));
                    }
                }

                _flame.SetCustomParticlePolygonSpawners(materials);
            }
            else
            {
                Particles::ParticleSpawnFunction spawnerFunc = [=]
                {
                    Vector2f rotator = Vector2f::FromRotation(Random::RandomBetween(0.0f, TwoPi), 1.0f);
                    return bounds.Middle() + (rotator * (bounds.Size * 0.5f));
                };
                Particles::ParticleBoundsFunction boundsFunc = [=]
                {
                    return bounds;
                };

                _flame.SetCustomParticleFunctionSpawner(spawnerFunc, boundsFunc);
            }

            _flame.SetPosition(bounds.Middle());
            _flame.SetRadius(Max(bounds.W, bounds.H) * 2.0f);
        }
    }

    template <>
    void EnumeratePreloads<Character::BurningDamageOverTime>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::DamageOverTime>(preloads);
        EnumeratePreloads<Graphics::Flame>(preloads);
        preloads.insert(Character::FlameParticlesPath);
    }
}
