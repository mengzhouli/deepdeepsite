#include "Abilities/Leap.hpp"
#include "Characters/CharacterTraits.hpp"
#include "DamageTypes.hpp"
#include "NavigationUtility.hpp"
#include "ContentUtility.hpp"
#include "AbilityUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const float JumpDefaultRange = 1800.0f;
        static const float JumpSpeed = 2000.0f;
        static const float DamageRadius = 400.0f;
        static const float JumpDefaultMaxDamage = 7.0f;

        static const std::string LeapParticlesPath = "Particles/dust_05.partsys";
        static const float LeapParticlesScale = 5.0f;

        static const Audio::SoundPathVector LeapLandSoundPaths = 
        {
        };
        static const std::pair<float, float> LeapLandSoundRadius = std::make_pair(1000.0f, 3500.0f);
        static const float LeapLandSoundVolume = 1.0f;

        static const float LeapLandShakeMagnitude = 3.0f;
        static const float LeapLandShakeFrequency = 10.0f;
        static const float LeapLandShakeDuration = 0.25f;

        Leap::Leap(const AbilityParameters& parameters, float range, float damage)
            : BasicAbility(parameters, "ability_leap_name", AbilityType_OneTarget)
            , _executing(false)
            , _speed(JumpSpeed)
            , _range(range)
            , _damageRadius(DamageRadius)
            , _damage(Character::DamageType_Type_Explosion | Character::DamageType_Source_Explosion | Character::DamageType_Element_Physical, damage)
            , _draw(false)
            , _jumpSpline()
            , _targetValid(false)
            , _target()
        {
            SetIcon("HUD/abilities.hudmatset", "icon_leap");
            SetCursor("HUD/abilities.cursorset", "cursor_leap");
            SetCooldown(15.0f);
            SetResourceCost(Item::Resources(0));
            SetTooltipDescription("ability_leap_tooltip", _range, _damage.Amount);

            _landSounds.AddSounds(LeapLandSoundPaths);
        }

        Leap::Leap(const AbilityParameters& parameters)
            : Leap(parameters, JumpDefaultRange, JumpDefaultMaxDamage)
        {
        }

        static Vector2f computeJumpPosition(const Vector2f& origin, const Vector2f& dest, float range, float characterHeight, float t)
        {
            Vector2f jumpPos = Vector2f::Lerp(origin, dest, t);
            float length = Vector2f::Distance(origin, dest);

            // Jump height is a curve 1.5 - (6.0 * (x - 0.5)^2)
            float jumpHeight = 1.5f - (6.0f * (t - 0.5f) * (t - 0.5f));

            float jumpLengthPerc = Vector2f::Distance(origin, dest) / range;
            float jumpHeightPerc = 1.0f;

            jumpPos.Y -= jumpHeight * Clamp(jumpLengthPerc, 0.2f, 1.0f) * jumpHeightPerc * Max(characterHeight * 0.5f, length * 0.2f);

            return jumpPos;
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

        void Leap::ClearState()
        {
            _draw = false;
            _targetValid = false;
        }

        void Leap::SetPrimaryTarget(const Vector2f& pos)
        {
            _target = pos;
            _targetValid = validateTarget(_target, _jumpSpline);
            _draw = _targetValid;
        }

        bool Leap::IsInValidState() const
        {
            return _targetValid;
        }

        Character::Action Leap::GenerateAction() const
        {
            if (_targetValid)
            {
                return Character::CreateUseAbilityAction(GetID(), _target, _target);
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void Leap::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _target = Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            _executing = validateTarget(_target, _jumpSpline);

            if (_executing)
            {
                _startPos = owner->GetPosition();
                _ownerHeight = owner->GetBounds().H;

                _timer = 0.0f;
                _duration = Vector2f::Distance(_startPos, _target) / _speed;

                if (IsA<Character::Leaper>(owner))
                {
                    Character::Leaper* leaper = AsA<Character::Leaper>(owner);
                    _windUpTimer = Max(leaper->OnPreLeap(getTerrainType(owner->GetLevelLayer(), _startPos)), 0.0f);
                }
                else
                {
                    _windUpTimer = 0.0f;
                }
                _landTimer = -1.0f;

                StartCooldown();
            }
        }

        void Leap::CancelAction()
        {
            // TODO
        }

        bool Leap::IsExecutingAction() const
        {
            return _executing;
        }

        void Leap::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            if (_executing)
            {
                Character::Character* owner = GetOwner();

                owner->SetRotation(Rotatorf(_target - _startPos));

                if (_windUpTimer >= 0.0f) // Winding up
                {
                    _windUpTimer -= dt;

                    if (_windUpTimer < 0.0f)
                    {
                        if (IsA<Character::Leaper>(owner))
                        {
                            Character::Leaper* leaper = AsA<Character::Leaper>(owner);
                            const float maxDuration = _range / _speed;
                            leaper->OnLeap(getTerrainType(owner->GetLevelLayer(), _startPos), _duration, maxDuration);
                        }
                    }
                }
                else if (_landTimer >= 0.0f) // Landing
                {
                    _landTimer -= dt;

                    if (_landTimer < 0.0f)
                    {
                        _executing = false;
                    }
                }
                else // In the air
                {
                    _timer += dt;

                    if (_timer >= _duration)
                    {
                        owner->SetPosition(_target);

                        // Explosion
                        _landParticles->SetPointSpawner(_target);
                        _landParticles->Burst();

                        // Damage enemies
                        Vector2f ownerMid = owner->GetBounds().Middle();
                        std::vector<Character::Character*> hitTargets = owner->GetLevelLayer()->GetCharacters<Character::Character>([&](const Character::Character* other)
                        {
                            return owner->IsCharacterAttackable(other, false) &&
                                Vector2f::DistanceSquared(ownerMid, other->GetBounds().Middle()) < (_damageRadius * _damageRadius);
                        });

                        for (auto& hitTarget : hitTargets)
                        {
                            Vector2f hitDirection = hitTarget->GetBounds().Middle() - ownerMid;
                            float distPerc = 1.0f - (hitDirection.Length() / _damageRadius);
                            Character::Damage dmg(_damage.Type, distPerc * _damage.Amount);

                            hitTarget->ApplyDamage(owner, ownerMid, dmg);
                            hitTarget->ApplyLinearImpulse(Vector2f::Normalize(hitDirection) * (distPerc * _damageRadius * 300.0f));
                        }

                        if (IsA<Character::Leaper>(owner))
                        {
                            Character::Leaper* leaper = AsA<Character::Leaper>(owner);
                            _landTimer = leaper->OnPostLeap(getTerrainType(owner->GetLevelLayer(), _target));
                        }
                        else
                        {
                            _landTimer = 0.0f;
                        }

                        auto soundManager = owner->GetLevel()->GetSoundManager();
                        soundManager->PlaySinglePositionalSound(_landSounds.GetNextSound(), Audio::SoundPriority::High, _target, LeapLandSoundRadius.first, LeapLandSoundRadius.second, LeapLandSoundVolume);

                        auto& cameraController = owner->GetLevel()->GetCameraController();
                        cameraController.Shake(_target, LeapLandSoundRadius.first, LeapLandSoundRadius.second, LeapLandShakeMagnitude, LeapLandShakeFrequency, LeapLandShakeDuration);
                    }
                    else
                    {
                        const Vector2f jumpPos = computeJumpPosition(_startPos, _target, _range, _ownerHeight, _timer / _duration);
                        owner->SetPosition(jumpPos);
                    }
                }
            }

            _landParticles->Update(totalTime, dt);
        }

        void Leap::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _landParticles = Content::CreateParticleSystemInstance(contentManager, LeapParticlesPath);
            _landParticles->SetScale(LeapParticlesScale);
            
            _landSounds.LoadContent(contentManager);
        }

        void Leap::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_landParticles);

            _landSounds.UnloadContent();
        }

        void Leap::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(_landParticles, false);
        }

        bool Leap::validateTarget(Vector2f& target, Splinef& spline) const
        {
            spline.Clear();

            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();

            static const float maxSearchDist = 400.0f;
            Vector2f normal;
            bool valid = Level::GetMoveTarget(layer, owner, Pathfinding::EdgeType_Climb, target, maxSearchDist, target, normal);

            auto ownerPathPosition = owner->GetCurrentPathPosition();
            if (!ownerPathPosition || !ownerPathPosition->IsWalkable())
            {
                valid = false;
            }

            const Vector2f& origin = owner->GetPosition();
            if (Vector2f::Distance(origin, target) > _range)
            {
                valid = false;
            }

            static const uint32_t hitTests = 32;
            const float height = owner->GetBounds().H;

            spline.AddPoint(origin);
            for (uint32_t i = 0; i < hitTests; i++)
            {
                float t = float(i + 1) / (hitTests + 1);

                Vector2f pos = computeJumpPosition(origin, target, _range, height, t);
                Vector2f headPos = pos - Vector2f(0.0f, height);

                Pathfinding::EdgeType hitEdgeType;
                if (layer->HitTerrain(pos, hitEdgeType, Pathfinding::EdgeType_Walk) ||
                    layer->HitTerrain(headPos, hitEdgeType, Pathfinding::EdgeType_Walk))
                {
                    valid = false;
                }

                spline.AddPoint(pos);
            }
            spline.AddPoint(target);

            return valid;
        }

        void Leap::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            BasicAbility::OnDrawHUD(spriteRenderer);

            if (_draw)
            {
                Character::Character *owner = GetOwner();
                assert(owner);

                const Camera& worldCam = owner->GetLevel()->GetCameraController().GetCamera();
                const Camera& hudCam = owner->GetLevel()->GetHUDCamera();

                Vector2f hudCenter = Camera::Transpose(worldCam, _target, hudCam);
                float hudRadius = Vector2f::Distance(hudCenter, Camera::Transpose(worldCam, _target + Vector2f(_damageRadius, 0.0f), hudCam));

                const uint32_t segmentCount = 64;

                Color innerColor = Color::Red;
                innerColor.A = 63;
                Graphics::DrawFilledCircle(spriteRenderer, hudCenter, 0.0f, innerColor, hudRadius, innerColor, segmentCount);

                Color outterColor(innerColor.R, innerColor.G, innerColor.B, 127);
                Graphics::DrawCircle(spriteRenderer, hudCenter, hudRadius, outterColor, 2.0f, segmentCount);

                Splinef hudJumpSpline;
                for (uint32_t i = 0; i < _jumpSpline.Size(); i++)
                {
                    hudJumpSpline.AddPoint(Camera::Transpose(worldCam, _jumpSpline[i], hudCam));
                }
                Color splineStartColor = _targetValid ? Color::Green : Color::Red;
                Color splineEndColor = _targetValid ? Color::Blue : Color::Red;
                Graphics::DrawSpline(spriteRenderer, hudJumpSpline, splineStartColor, 4.0f, splineEndColor, 4.0f, segmentCount);
            }
        }
    }

    template <>
    void EnumeratePreloads<Ability::Leap>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        preloads.insert("HUD/abilities.hudmatset");
        preloads.insert("HUD/abilities.cursorset");
        preloads.insert(Ability::LeapParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Ability::LeapLandSoundPaths);
    }
}
