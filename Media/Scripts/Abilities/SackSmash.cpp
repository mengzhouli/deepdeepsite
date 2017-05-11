#include "Abilities/SackSmash.hpp"

#include "characters/CharacterTraits.hpp"

#include "NavigationUtility.hpp"
#include "ContentUtility.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const std::string SackSmashIconPath = "HUD/abilities.hudmatset";
        static const std::string SackSmashCursorPath = "HUD/abilities.cursorset";

        static const float SackSmashCooldown = 20.0f;
        static const float SackSmashDamage = 10.0f;
        static const float SackSmashRadius = 300.0f;

        static const float SackSmashShakeMagnitude = 5.0f;
        static const float SackSmashShakeFrequency = 10.0f;
        static const float SackSmashShakeDuration = 0.4f;

        static const std::string SackSmashParticlesPath = "Particles/dust_05.partsys";
        static const float SackSmashParticlesScale = 5.0f;

        static const Audio::SoundPathVector SackSmashHitSoundPaths =
        {
            { "Audio/Footsteps/step_Land_OrcAlways_1.wav", 1.0f },
        };
        static const std::pair<float, float> SackSmashHitSoundRadius = std::make_pair(1000.0f, 3500.0f);
        static const float SackSmashHitSoundVolume = 1.0f;

        static const Audio::SoundPathVector SackSmashWooshSoundPaths =
        {
            { "Audio/Items/Weapons/Ork_Sword_Swipe.wav", 1.0f },
        };
        static const std::pair<float, float> SackSmashWooshSoundRadius = std::make_pair(1000.0f, 3500.0f);
        static const float SackSmashWooshSoundVolume = 1.0f;

        SackSmash::SackSmash(const AbilityParameters& parameters)
            : BasicAbility(parameters, "ability_sack_smash_name", AbilityType_OneTarget)
        {
            SetIcon(SackSmashIconPath, "icon_sack_smash");
            SetCursor(SackSmashCursorPath, "cursor_sack_smash");
            SetCooldown(SackSmashCooldown);
            SetTooltipDescription("ability_sack_smash_tooltip", SackSmashDamage);

            _sackWooshSounds.AddSounds(SackSmashWooshSoundPaths);
            _sackHitSounds.AddSounds(SackSmashHitSoundPaths);
        }

        void SackSmash::ClearState()
        {
            _placementValid = false;
            _smashing = false;
            _executing = false;
            _smashTimer = -1.0f;
            _wooshTimer = -1.0f;
            _hitTimer = -1.0f;
        }

        void SackSmash::SetPrimaryTarget(const Vector2f& pos)
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
            _placementValid = Level::GetPlaceTarget(layer, Pathfinding::EdgeType_Walk, pos, 400.0f, _target);
        }

        bool SackSmash::IsInValidState() const
        {
            return _placementValid;
        }

        Character::Action SackSmash::GenerateAction() const
        {
            return Character::CreateUseAbilityAction(GetID(), _target);
        }

        void SackSmash::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _executionTarget = Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            _executing = owner->MoveTo(_executionTarget);
        }

        void SackSmash::CancelAction()
        {
            ClearState();
            if (_executing)
            {
                Character::Character* owner = GetOwner();
                owner->StopMoving();
            }
        }

        bool SackSmash::IsExecutingAction() const
        {
            return _executing;
        }

        void SackSmash::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            if (_executing)
            {
                Character::Character* owner = GetOwner();
                Character::SackSmasher* smasher = GetOwner<Character::SackSmasher>();
                assert(smasher);

                if (_smashing)
                {
                    if (_hitTimer >= 0.0f)
                    {
                        _hitTimer -= dt;
                        if (_hitTimer < 0.0f)
                        {
                            StartCooldown();

                            _dustParticles->SetPointSpawner(_target);
                            _dustParticles->Burst();

                            auto soundManager = owner->GetLevel()->GetSoundManager();
                            soundManager->PlaySinglePositionalSound(_sackHitSounds.GetNextSound(), Audio::SoundPriority::High, _target, SackSmashHitSoundRadius.first, SackSmashHitSoundRadius.second, SackSmashHitSoundVolume);

                            auto& cameraController = owner->GetLevel()->GetCameraController();
                            cameraController.Shake(_target, SackSmashHitSoundRadius.first, SackSmashHitSoundRadius.second, SackSmashShakeMagnitude, SackSmashShakeFrequency, SackSmashShakeDuration);

                            std::vector<Character::Character*> hitTargets = owner->GetLevelLayer()->GetCharacters<Character::Character>([&](const Character::Character* other)
                            {
                                return owner->IsCharacterAttackable(other, false) &&
                                    Vector2f::DistanceSquared(_target, other->GetBounds().Middle()) < (SackSmashRadius * SackSmashRadius);
                            });

                            for (auto& hitTarget : hitTargets)
                            {
                                Vector2f hitDirection = hitTarget->GetBounds().Middle() - _target;
                                float distPerc = 1.0f - (hitDirection.Length() / SackSmashRadius);
                                Character::Damage dmg(Character::DamageType_Type_Explosion | Character::DamageType_Source_Explosion | Character::DamageType_Element_Physical, distPerc * SackSmashDamage);

                                hitTarget->ApplyDamage(owner, _target, dmg);
                                hitTarget->ApplyLinearImpulse(Vector2f::Normalize(Vector2f::Normalize(hitDirection) - Vector2f::UnitY) * (distPerc * SackSmashRadius * 85.0f));
                            }
                        }
                    }

                    if (_wooshTimer >= 0)
                    {
                        _wooshTimer -= dt;
                        if (_wooshTimer < 0.0f)
                        {
                            auto soundManager = owner->GetLevel()->GetSoundManager();
                            soundManager->PlaySinglePositionalSound(_sackWooshSounds.GetNextSound(), Audio::SoundPriority::Medium, _target, SackSmashWooshSoundRadius.first, SackSmashWooshSoundRadius.second, SackSmashWooshSoundVolume);
                        }
                    }

                    if (_smashTimer >= 0)
                    {
                        _smashTimer -= dt;
                        if (_smashTimer < 0.0f)
                        {
                            _executing = false;
                            _smashing = false;
                            ClearState();
                        }
                    }
                }
                else
                {
                    const float minHitDist = owner->GetBounds().W;
                    bool closeEnoughToDropPos = Vector2f::Distance(owner->GetPosition(), _executionTarget) < minHitDist;

                    if (!owner->IsMoving() || closeEnoughToDropPos)
                    {
                        owner->StopMoving();

                        auto info = smasher->PlaySackSmashAnimation();
                        _smashing = true;
                        _smashTimer = info.TotalDuration;
                        _wooshTimer = info.WooshTime;
                        _hitTimer = info.SackHitTime;
                    }
                }
            }

            _dustParticles->Update(totalTime, dt);
        }

        void SackSmash::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _sackWooshSounds.LoadContent(contentManager);
            _sackHitSounds.LoadContent(contentManager);

            _dustParticles = Content::CreateParticleSystemInstance(contentManager, SackSmashParticlesPath);
            _dustParticles->SetScale(SackSmashParticlesScale);
        }

        void SackSmash::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            _sackWooshSounds.UnloadContent();
            _sackHitSounds.UnloadContent();
        }

        void SackSmash::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(_dustParticles, false);
        }

        void SackSmash::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            BasicAbility::OnDrawHUD(spriteRenderer);

            if (_placementValid)
            {
                Character::Character *owner = GetOwner();
                assert(owner);

                const Camera& worldCam = owner->GetLevel()->GetCameraController().GetCamera();
                const Camera& hudCam = owner->GetLevel()->GetHUDCamera();

                Vector2f hudCenter = Camera::Transpose(worldCam, _target, hudCam);
                float hudRadius = Vector2f::Distance(hudCenter, Camera::Transpose(worldCam, _target + Vector2f(SackSmashRadius, 0.0f), hudCam));

                const uint32_t segmentCount = 64;

                Color innerColor = Color::Red;
                innerColor.A = 63;
                Graphics::DrawFilledCircle(spriteRenderer, hudCenter, 0.0f, innerColor, hudRadius, innerColor, segmentCount);

                Color outterColor(innerColor.R, innerColor.G, innerColor.B, 127);
                Graphics::DrawCircle(spriteRenderer, hudCenter, hudRadius, outterColor, 2.0f, segmentCount);
            }
        }
    }

    template <>
    void EnumeratePreloads<Ability::SackSmash>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        preloads.insert(Ability::SackSmashIconPath);
        preloads.insert(Ability::SackSmashCursorPath);

        EnumerateSoundVectorPreloads(preloads, Ability::SackSmashHitSoundPaths);
        EnumerateSoundVectorPreloads(preloads, Ability::SackSmashWooshSoundPaths);
    }
}
