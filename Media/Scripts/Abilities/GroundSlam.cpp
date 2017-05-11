#include "Abilities/GroundSlam.hpp"
#include "Characters/CharacterTraits.hpp"
#include "DamageTypes.hpp"
#include "NavigationUtility.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const float DamageRadius = 2000.0f;
        static const float MaxDamage = 7.5f;
        static const float JumpHeightMultiplier = 1.0f;
        static const float ImpluseMultiplier = 500.0f;

        static const std::string GroundSlamParticlesPath = "Particles/dust_05.partsys";
        static const float GroundSlamParticlesScale = 5.0f;

        static const Audio::SoundPathVector GroundSlamLandSoundPaths =
        {
        };
        static const std::pair<float, float> GroundSlamLandSoundRadius = std::make_pair(1000.0f, 3500.0f);
        static const float GroundSlamLandSoundVolume = 1.0f;

        static const float GroundSlamLandShakeMagnitude = 8.0f;
        static const float GroundSlamLandShakeFrequency = 12.0f;
        static const float GroundSlamLandShakeDuration = 0.6f;

        GroundSlam::GroundSlam(const AbilityParameters& parameters)
            : BasicAbility(parameters, "ability_leap_name", AbilityType_NoTarget)
            , _executing(false)
            , _damage(Character::DamageType_Type_Explosion | Character::DamageType_Element_Physical, MaxDamage)
        {
            SetCooldown(15.0f);

            _landSounds.AddSounds(GroundSlamLandSoundPaths);
        }

        void GroundSlam::ClearState()
        {
            _executing = false;
            _hit = false;
        }

        bool GroundSlam::IsInValidState() const
        {
            return true;
        }

        Character::Action GroundSlam::GenerateAction() const
        {
            return Character::CreateUseAbilityAction(GetID());
        }

        static Pathfinding::TerrainType getTerrainType(Level::LevelLayerInstance* layer, const Vector2f& position)
        {
            auto pathPos = layer->GetPathSystem()->FindNearestPathEdge(position, 100.0f, Pathfinding::EdgeType_All);
            if (pathPos)
            {
                return pathPos->GetTerrain();
            }
            else
            {
                return Pathfinding::TerrainType::None;
            }
        }

        void GroundSlam::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _executing = true;

            _startPos = owner->GetPosition();
            _ownerHeight = owner->GetBounds().H;

            _hit = false;
            _hitTimmer = 0.0f;
            _actionTimer = 0.0f;

            if (IsA<Character::GroundSlammer>(owner))
            {
                Character::GroundSlammer* slammer = AsA<Character::GroundSlammer>(owner);
                Character::GroundSlammer::Info info = slammer->OnGroundSlam(getTerrainType(owner->GetLevelLayer(), _startPos));

                _hitTimmer = info.HitTime;
                _actionTimer = info.TotalDuration;
            }
            else
            {
                LogWarning("Using GroundSlam on a character that is not a GroundSlammer.");
            }

            StartCooldown();
        }

        void GroundSlam::CancelAction()
        {
            _executing = false;
            _hit = false;
        }

        bool GroundSlam::IsExecutingAction() const
        {
            return _executing;
        }

        void GroundSlam::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            if (_executing)
            {
                Character::Character* owner = GetOwner();

                _hitTimmer -= dt;
                _actionTimer -= dt;

                if (_hitTimmer < 0.0f && !_hit)
                {
                    _hit = true;

                    // Explosion
                    _landParticles->SetPointSpawner(_startPos);
                    _landParticles->Burst();

                    // Damage enemies
                    Vector2f ownerMid = owner->GetBounds().Middle();
                    std::vector<Character::Character*> hitTargets = owner->GetLevelLayer()->GetCharacters<Character::Character>([&](const Character::Character* other)
                    {
                        return owner->IsCharacterAttackable(other, false) &&
                            Vector2f::DistanceSquared(ownerMid, other->GetBounds().Middle()) < (DamageRadius * DamageRadius);
                    });

                    for (auto& hitTarget : hitTargets)
                    {
                        Vector2f hitDirection = hitTarget->GetBounds().Middle() - ownerMid;
                        float distPerc = 1.0f - (hitDirection.Length() / DamageRadius);
                        Character::Damage dmg(_damage.Type, distPerc * _damage.Amount);

                        auto collision = hitTarget->GetCollision();
                        float mass = 1.0f;
                        if (collision)
                        {
                            mass = collision->GetMass();
                        }

                        hitTarget->ApplyDamage(owner, ownerMid, dmg);
                        hitTarget->ApplyLinearImpulse(Vector2f(0.0f, -(distPerc * ImpluseMultiplier * mass)));
                    }

                    if (IsA<Character::GroundSlammer>(owner))
                    {
                        Character::GroundSlammer* groundSlammer = AsA<Character::GroundSlammer>(owner);
                        groundSlammer->OnGroundSlamLand(getTerrainType(owner->GetLevelLayer(), _startPos));
                    }

                    auto soundManager = owner->GetLevel()->GetSoundManager();
                    soundManager->PlaySinglePositionalSound(_landSounds.GetNextSound(), Audio::SoundPriority::High, _startPos, GroundSlamLandSoundRadius.first, GroundSlamLandSoundRadius.second, GroundSlamLandSoundVolume);

                    auto& cameraController = owner->GetLevel()->GetCameraController();
                    cameraController.Shake(_startPos, GroundSlamLandSoundRadius.first, GroundSlamLandSoundRadius.second, GroundSlamLandShakeMagnitude, GroundSlamLandShakeFrequency, GroundSlamLandShakeDuration);
                }

                if (_actionTimer < 0.0f)
                {
                    _executing = false;
                }
            }

            _landParticles->Update(totalTime, dt);
        }

        void GroundSlam::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _landParticles = Content::CreateParticleSystemInstance(contentManager, GroundSlamParticlesPath);
            _landParticles->SetScale(GroundSlamParticlesScale);

            _landSounds.LoadContent(contentManager);
        }

        void GroundSlam::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_landParticles);

            _landSounds.UnloadContent();
        }

        void GroundSlam::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(_landParticles, false);
        }
    }

    template <>
    void EnumeratePreloads<Ability::GroundSlam>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        preloads.insert("HUD/abilities.hudmatset");
        preloads.insert(Ability::GroundSlamParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Ability::GroundSlamLandSoundPaths);
    }
}
