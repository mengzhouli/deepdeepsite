#include "Abilities/ShootFlare.hpp"

#include "Characters/SkeletonCharacter.hpp"
#include "Characters/BasicCharacter.hpp"
#include "Physics/CircleCollision.hpp"
#include "Particles/ParticleSystemInstance.hpp"
#include "Item/Weapon.hpp"

#include "AttachPoints.hpp"
#include "Items/Weapons/WeaponTraits.hpp"
#include "Characters/Torch.hpp"
#include "Characters/Explosive.hpp"
#include "Buffs/BurningDamageOverTime.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string FlareSkeletonPath = "Skeletons/Characters/Flare/flare.skel";
        static const std::string FlareMatsetPath = "Skeletons/Characters/Flare/flare.polymatset";
        static const std::string FlareFireParticlesPathPath = "Particles/flare_0.partsys";

        static const float FlareCollisionRadius = 20.0f;
        static const float FlareStartRadius = 1200.0f;
        static const float FlareFadeStartPerc = 0.2f;
        static const Color FlareColor = Color::FromBytes(255, 128, 64, 255);

        static const float FlareMaxVolume = 0.15f;
        static const std::pair<float, float> FlareSoundRadius = std::make_pair(1500.0f, 3000.0f);
        static const Audio::SoundPathVector FlareSizzleSoundPaths =
        {
            { "Audio/Characters/Explosive/Bomb_fuse_loop.ogg", 1.0f },
        };

        class FlareDebuff : public BasicBuff
        {
        public:
            FlareDebuff(const BuffParameters& parameters, CharacterID shooter, float duration, float dps, float damageTickRate)
                : BasicBuff(parameters, "buff_flare_name")
            {
                SetTotalDuration(duration);
                AddComponent<BurningDamageOverTime>(shooter, dps, damageTickRate);
            }
        };

        class Flare : public SkeletonCharacter
        {
        public:
            Flare(const CharacterParameters& parameters, CharacterID shooter, float duration, bool enableBurn, float damageDuration, float dps, float damageTickRate)
                : SkeletonCharacter(parameters, FlareSkeletonPath, FlareMatsetPath)
                , _shooter(shooter)
                , _totalDuration(duration)
                , _remainingDuration(duration)
                , _light(Vector2f::Zero, FlareStartRadius, FlareColor, 0.9f)
                , _enableBurn(enableBurn)
                , _burningConstructor(BindBuffConstructor<FlareDebuff>(shooter, damageDuration, dps, damageTickRate))
                , _litCharacters()
            {
                SetEntityMask(CharacterMask_None);
                SetMoveType(MoveType_None);
                SetSkeletonCastsShadows(false);
                EnablePhysics(true);
                SetInvulnerable(true);
                
                _flareSizzleSounds.AddSounds(FlareSizzleSoundPaths);
            }

            virtual Physics::Collision* CreateCollision() override
            {
                Physics::Collision* collision = new Physics::CircleCollision(GetLevelLayer()->GetPhysicsWorld(), FlareCollisionRadius);
                collision->SetBullet(true);
                return collision;
            }

            virtual void OnLoadContent(Content::ContentManager* contentManager) override
            {
                SkeletonCharacter::OnLoadContent(contentManager);

                _flareSizzleSounds.LoadContent(contentManager);
                auto soundManager = GetLevel()->GetSoundManager();;
                _curFlareSizzleSound = soundManager->PlayLoopingPositionalSound(_flareSizzleSounds.GetNextSound(), Audio::SoundPriority::Medium, GetPosition(), FlareSoundRadius.first, FlareSoundRadius.second, FlareMaxVolume);
                
                const Particles::ParticleSystem* particleSystem = contentManager->Load<Particles::ParticleSystem>(FlareFireParticlesPathPath);
                _partsys = new Particles::ParticleSystemInstance(particleSystem);
                SafeRelease(particleSystem);
                _partsys->Start();
            }

            virtual void OnUnloadContent() override
            {
                SkeletonCharacter::OnUnloadContent();
                
                _flareSizzleSounds.UnloadContent();
                
                SafeRelease(_partsys);
            }

            virtual void OnUpdate(double totalTime, float dt) override
            {
                SkeletonCharacter::OnUpdate(totalTime, dt);

                const Vector2f& position = GetPosition();

                _light.Position = position - Vector2f(0, FlareCollisionRadius * 2.0f);
                _partsys->SetPointSpawner(position);
                _partsys->Update(totalTime, dt);
                
                _curFlareSizzleSound->SetPosition(position);

                _remainingDuration -= dt;
                float flarePerc = Clamp(_remainingDuration / _totalDuration, 0.0f, 1.0f);
                if (flarePerc < FlareFadeStartPerc)
                {
                    float scalePerc = (flarePerc / FlareFadeStartPerc);
                    _light.Radius = scalePerc * FlareStartRadius;
                    _partsys->SetScale(scalePerc);
                    _curFlareSizzleSound->SetVolume(FlareMaxVolume * scalePerc);
                }
                else
                {
                    _light.Radius = FlareStartRadius;
                    _partsys->SetScale(1.0f);
                    _curFlareSizzleSound->SetVolume(FlareMaxVolume);
                }

                Level::LevelLayerInstance* layer = GetLevelLayer();

                std::vector<Vector2f> hitPts;
                std::vector<FlameHolder*> flameHolders = layer->FindIntersections<FlameHolder>(GetCollision(), hitPts, nullptr);
                for (auto& flameHolder : flameHolders)
                {
                    flameHolder->TurnOn();
                }

                std::vector<Explosive*> explosives = layer->FindIntersections<Explosive>(GetCollision(), hitPts, nullptr);
                for (auto& explosive : explosives)
                {
                    explosive->Start();
                }

                Character* owner = layer->GetCharacter(_shooter);
                if (owner && _enableBurn)
                {
                    std::vector<Character*> enemies = layer->FindIntersections<Character>(GetCollision(), hitPts, [&](const Character* character)
                    {
                        return owner->IsCharacterAttackable(character, false) && _litCharacters.count(character->GetID()) == 0;
                    });

                    for (auto enemy : enemies)
                    {
                        enemy->ApplyBuff(_burningConstructor);
                        _litCharacters.insert(enemy->GetID());
                    }
                }

                if (_remainingDuration <= 0.0f)
                {
                    Terminate();
                }
            }

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override
            {
                SkeletonCharacter::OnDraw(levelRenderer);

                levelRenderer->AddLight(_light);
                levelRenderer->AddDrawable(_partsys, false);
            }

        private:
            CharacterID _shooter;

            const float _totalDuration;
            float _remainingDuration;

            Lights::PointLight _light;
            Particles::ParticleSystemInstance* _partsys;
            
            Audio::SoundSet _flareSizzleSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _curFlareSizzleSound;

            bool _enableBurn;
            BuffConstructor<> _burningConstructor;
            std::unordered_set<CharacterID> _litCharacters;
        };
    }

    template <>
    void EnumeratePreloads<Character::FlareDebuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        EnumeratePreloads<Character::BurningDamageOverTime>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::Flare>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
        preloads.insert(Character::FlareSkeletonPath);
        preloads.insert(Character::FlareMatsetPath);
        preloads.insert(Character::FlareFireParticlesPathPath);

        EnumeratePreloads<Character::FlareDebuff>(preloads);
    }

    namespace Ability
    {
        static const std::string FlareIconPath = "HUD/abilities.hudmatset";
        static const std::string FlareCursorPath = "HUD/abilities.cursorset";

        static const float FlareDuration = 20.0f;
        static const float FlareCooldown = 7.0f;
        static const float FlareRange = 3000.0f;

        static const bool FlareBurnEnabled = true;
        static const float FlareBurnDuration = 3.0f;
        static const float FlareBurnDPS = 2.0f;
        static const float FlareBurnTickRate = 0.5f;
        
        static const float FlareShootVolume = 1.0f;
        static const std::pair<float, float> FlareShootSoundRadius = std::make_pair(1500.0f, 3000.0f);
        static const Audio::SoundPathVector FlareShootSoundPaths =
        {
            { "Audio/Items/Weapons/Navigator_GunPop.wav", 1.0f },
            { "Audio/Items/Weapons/Navigator_GunPop_2.wav", 1.0f },
            { "Audio/Items/Weapons/Navigator_GunPop_3.wav", 1.0f },
        };

        ShootFlare::ShootFlare(const AbilityParameters& parameters)
            : ShootFlare(parameters, true)
        {
        }

        ShootFlare::ShootFlare(const AbilityParameters& parameters, bool drawSpline)
            : BasicAbility(parameters, "ability_shoot_flare_name", AbilityType_OneTarget)
            , _flareDuration(FlareDuration)
            , _drawSpline(false)
            , _drawSplineEnabled(drawSpline)
            , _trajectorySpline()
            , _target()
            , _flareShootSounds()
        {
            SetIcon(FlareIconPath, "icon_shoot_flare");
            SetCursor(FlareCursorPath, "cursor_shoot_flare");
            SetCooldown(FlareCooldown);
            SetTooltipDescription("ability_shoot_flare_tooltip", FlareBurnDPS, FlareBurnDuration);

            _flareShootSounds.AddSounds(FlareShootSoundPaths);
        }
        void ShootFlare::ClearState()
        {
            _drawSpline = false;

            Character::Character *owner = GetOwner();
            assert(owner);

            if (IsA<Character::BasicCharacter>(owner))
            {
                AsA<Character::BasicCharacter>(owner)->StopPointingMainArm();
            }

            for (Item::FlareAttachable* grappleAttachableWeapon : owner->GetWeapons<Item::FlareAttachable>())
            {
                grappleAttachableWeapon->SetFlareAttached(false);
            }
        }

        static Vector2f getAdjustedAimPosition(const Vector2f& origin, const Vector2f& target, float &outRangePerc)
        {
            float dist = Vector2f::Distance(origin, target);
            if (dist > FlareRange)
            {
                Vector2f dir = Vector2f::Normalize(target - origin);
                outRangePerc = 1.0f;
                return origin + (dir * FlareRange);
            }
            else
            {
                outRangePerc = dist / FlareRange;
                return target;
            }
        }

        static Vector2f getShootPos(Character::Character *owner)
        {
            std::vector<Item::MuzzleHaver*> muzzleWeapons = owner->GetWeapons<Item::MuzzleHaver>();
            if (!muzzleWeapons.empty())
            {
                return muzzleWeapons.front()->GetMuzzlePosition().Position;
            }
            else
            {
                return owner->GetAttachPoint(Character::AttachPoint_WeaponMain).Position;
            }
        }

        void ShootFlare::SetPrimaryTarget(const Vector2f& pos)
        {
            _target = pos;
            _drawSpline = true;

            Character::Character *owner = GetOwner();
            assert(owner);

            for (Item::FlareAttachable* grappleAttachableWeapon : owner->GetWeapons<Item::FlareAttachable>())
            {
                grappleAttachableWeapon->SetFlareAttached(true);
            }
        }

        bool ShootFlare::IsInValidState() const
        {
            return true;
        }

        Character::Action ShootFlare::GenerateAction() const
        {
            return Character::CreateUseAbilityAction(GetID(), _target);
        }

        void ShootFlare::ExecuteAction(const Character::Action& action)
        {
            _target = Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY);

            Character::Character *owner = GetOwner();
            assert(owner);

            Vector2f launchPos, velocity, impulse;
            computeTrajectory(launchPos, velocity, impulse);

            Character::CharacterConstructor<Character::Flare> constructor =
                Character::BindCharacterConstructor<Character::Flare>(owner->GetID(), _flareDuration, FlareBurnEnabled, FlareBurnDuration, FlareBurnDPS, FlareBurnTickRate);
            Character::Flare* flare = owner->GetLevelLayer()->SpawnCharacter(launchPos, "flare", nullptr, constructor);
            flare->ApplyLinearImpulse(impulse);
            
            auto soundManager = owner->GetLevel()->GetSoundManager();
            soundManager->PlaySinglePositionalSound(_flareShootSounds.GetNextSound(), Audio::SoundPriority::High, launchPos, FlareShootSoundRadius.first, FlareShootSoundRadius.second, FlareShootVolume);

            StartCooldown();
        }

        void ShootFlare::CancelAction()
        {
            _drawSpline = false;
        }

        bool ShootFlare::IsExecutingAction() const
        {
            return false;
        }

        void ShootFlare::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);
            
            _flareShootSounds.LoadContent(contentManager);
        }

        void ShootFlare::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();
            
            _flareShootSounds.UnloadContent();
        }

        void ShootFlare::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            if (_drawSpline)
            {
                Vector2f launchPos, velocity, impulse;
                computeTrajectory(launchPos, velocity, impulse);

                Character::Character *owner = GetOwner();
                assert(owner);

                Physics::PhysicsWorld* physicsWorld = owner->GetLevelLayer()->GetPhysicsWorld();
                float g = physicsWorld->GetGravity().Y;

                const uint32_t numSteps = 32;
                const float simulationTime = 1.0f;
                const float timePerStep = simulationTime / numSteps;

                _trajectorySpline.Clear();
                for (uint32_t i = 0; i < numSteps; i++)
                {
                    float t = timePerStep * i;
                    Vector2f pos(launchPos.X + (velocity.X * t), launchPos.Y + (velocity.Y * t) + (0.5f * g * t * t));
                    _trajectorySpline.AddPoint(pos);
                }

                if (!owner->IsMoving())
                {
                    owner->SetRotation(Rotatorf(_target - owner->GetPosition()));
                }

                if (IsA<Character::BasicCharacter>(owner))
                {
                    AsA<Character::BasicCharacter>(owner)->PointMainArmAt(_target);
                }
            }
        }

        void ShootFlare::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            BasicAbility::OnDrawHUD(spriteRenderer);

            if (_drawSpline && _drawSplineEnabled)
            {
                Character::Character *owner = GetOwner();
                assert(owner);

                const Camera& worldCam = owner->GetLevel()->GetCameraController().GetCamera();
                const Camera& hudCam = owner->GetLevel()->GetHUDCamera();

                uint32_t numSegments = _trajectorySpline.Length() / 5.0f;
                Vector2f prev = Camera::Transpose(worldCam, _trajectorySpline.Evalulate(0.0f), hudCam);
                for (uint32_t i = 1; i < numSegments; i++)
                {
                    float t = i / (float)numSegments;
                    Vector2f cur = Camera::Transpose(worldCam, _trajectorySpline.Evalulate(t), hudCam);

                    Color color = Color::Lerp(Color::Green, Color::Blue, t);
                    if (t > 0.5f)
                    {
                        color.A = 255.0f * (1.0f - ((t - 0.5f) * 2.0f));
                    }

                    spriteRenderer->DrawLine(prev, cur, 4, color);

                    prev = cur;
                }
            }
        }

        void ShootFlare::computeTrajectory(Vector2f& outLaunchPos, Vector2f& outVelocity, Vector2f& outImpulse)
        {
            Character::Character *owner = GetOwner();
            assert(owner);

            Physics::PhysicsWorld* physicsWorld = owner->GetLevelLayer()->GetPhysicsWorld();

            float m = Physics::CircleCollision::ComputeMass(physicsWorld, Character::FlareCollisionRadius);
            float g = physicsWorld->GetGravity().Y;

            Vector2f flareShootPos = getShootPos(owner);

            float rangePerc;
            Vector2f target = getAdjustedAimPosition(flareShootPos, _target, rangePerc);
            Vector2f dir = target - flareShootPos;

            const float t = Lerp(0.1f, 1.2f, rangePerc);
            const Vector2f vi(dir.X / t, ((2.0f * dir.Y) - (g * t * t)) / (2.0f * t));
            const Vector2f impulse(vi * m);

            outLaunchPos = flareShootPos;
            outVelocity = vi;
            outImpulse = impulse;
        }
    }

    template <>
    void EnumeratePreloads<Ability::ShootFlare>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);
        EnumeratePreloads<Character::Flare>(preloads);
        preloads.insert(Ability::FlareIconPath);
        preloads.insert(Ability::FlareCursorPath);
        EnumerateSoundVectorPreloads(preloads, Ability::FlareShootSoundPaths);
    }
}
