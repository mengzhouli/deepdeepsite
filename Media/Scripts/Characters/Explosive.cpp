#include "Characters/Explosive.hpp"
#include "DamageTypes.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const Audio::SoundPathVector ExplosiveFuseSoundPaths =
        {
            { "Audio/Characters/Explosive/Bomb_fuse_loop.ogg", 1.0f },
        };

        static const std::pair<float, float> ExplosiveBlastSoundRadius = std::make_pair(1000.0f, 4000.0f);
        static const float ExplosiveShakeMagnitude = 5.0f;
        static const float ExplosiveShakeFrequency = 10.0f;
        static const float ExplosiveShakeDuration = 0.75f;

        static const std::string ExplosiveParticlesPath = "Particles/explosion_1.partsys";
        static const std::string ExplosiveFuseParticlesPath = "Particles/fuse.partsys";

        Explosive::Explosive(const CharacterParameters& parameters, const std::string& skeleton, const std::string& matset, const std::string& fuseName,
            const std::string& exposionOrigin, CharacterID owner, float duration, float radius, float dmg)
            : SkeletonCharacter(parameters, skeleton, matset)
            , _owner(owner ? owner : parameters.ID)
            , _hitsEnemiesOnly(false)
            , _drawSkeletonAfterExplosion(true)

            , _countingDown(true)
            , _durationRemaining(duration)

            , _timeDead(0.0f)
            , _deathFadeBegin(5.0f)
            , _deathFadeDuration(5.0f)

            , _blastRadius(radius)
            , _blastImpulse(radius * 100.0f)
            , _dmg(dmg)

            , _fuseName(fuseName)
            , _explosionOrigin(exposionOrigin)

            , _fuseParticlesPath(ExplosiveFuseParticlesPath)
            , _explosionParticlesPath(ExplosiveParticlesPath)

            , _blastLightDuration(0.3f)
            , _explosionLight(Vector2f::Zero, radius * 12.0f, Color::White, 0.25f)
            , _fuseLight(Vector2f::Zero, radius * 0.2f, Color::FromFloats(1.0f, 1.0f, 0.75f, 1.0f), 0.0f)

            , _exploding(false)

            , _fuseSounds()
            , _currentFuseSound()
            , _explosionSounds()
        {
            SetEntityMask(CharacterMask_None);
            SetMoveType(MoveType_None);

            SetSkeletonCastsShadows(false);
            SetSkeletonColor(Color::White);
            EnablePhysics(true);
            SetInvulnerable(true);

            AddFuseSounds(ExplosiveFuseSoundPaths);
        }

        void Explosive::OnSpawn()
        {
            _exploding = false;
            if (_countingDown)
            {
                Start();
            }
            else
            {
                Stop();
            }
        }

        void Explosive::Start()
        {
            if (_countingDown)
            {
                return;
            }

            _countingDown = true;
            if (_fuse && !_exploding)
            {
                _fuse->Start();

                Rayf fuseAttachPt = GetAttachPoint(_fuseName);

                auto soundManager = GetLevel()->GetSoundManager();
                _currentFuseSound = soundManager->PlayLoopingPositionalSound(_fuseSounds.GetNextSound(), Audio::SoundPriority::High, fuseAttachPt.Position, 1000.0f, 4000.0f, 0.20f);
            }
        }

        void Explosive::Stop()
        {
            if (!_countingDown)
            {
                return;
            }

            _countingDown = false;
            if (_fuse && !_exploding)
            {
                _fuse->Stop();
                if (_currentFuseSound)
                {
                    _currentFuseSound->Stop(0.1f);
                    _currentFuseSound = nullptr;
                }
            }
        }

        void Explosive::Toggle()
        {
            if (_countingDown)
            {
                Stop();
            }
            else
            {
                Start();
            }
        }

        void Explosive::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            Level::LevelLayerInstance* layer = GetLevelLayer();

            if (_fuse)
            {
                Rayf fuseAttachPt = GetAttachPoint(_fuseName);
                _fuse->SetPointSpawner(fuseAttachPt.Position);
                _fuse->SetRotation(Rotatorf(fuseAttachPt.Direction));
                _fuse->Update(totalTime, dt);
                _fuseLight.Position = fuseAttachPt.Position;

                if (_currentFuseSound)
                {
                    _currentFuseSound->SetPosition(fuseAttachPt.Position);
                }
            }

            Rayf explosionAttachPt = GetAttachPoint(_explosionOrigin);
            _explosion->SetPointSpawner(explosionAttachPt.Position);
            _explosion->Update(totalTime, dt);

            if (_exploding)
            {
                if (_timeDead < _blastLightDuration)
                {
                    Color lightBaseColor = Color::White;

                    float fade = Power(1.0f - Clamp(_timeDead / _blastLightDuration, 0.0f, 1.0f), 3.0f);
                    _explosionLight.LightColor = lightBaseColor * fade;
                    _explosionLight.LightColor.A = 255;
                }

                if (_timeDead > _deathFadeBegin)
                {
                    Color curSkelColor = GetSkeleton()->GetColor();
                    float fade = 1.0f - Clamp((_timeDead - _deathFadeBegin) / _deathFadeDuration, 0.0f, 1.0f);
                    curSkelColor.A = uint8_t(fade * 255.0f);
                    GetSkeleton()->SetColor(curSkelColor);
                }

                if (_timeDead > _deathFadeBegin + _deathFadeDuration && _explosion->GetParticleCount() == 0)
                {
                    // terminate
                    Terminate();
                }

                _timeDead += dt;
            }
            else
            {
                if (_countingDown)
                {
                    _durationRemaining -= dt;
                }
                if (_durationRemaining < 0.0f && !_exploding)
                {
                    Vector2f blastOrigin = explosionAttachPt.Position;

                    // blow up
                    _exploding = true;
                    if (_fuse)
                    {
                        _fuse->Stop();
                    }
                    if (_currentFuseSound)
                    {
                        _currentFuseSound->Stop(0.1f);
                        _currentFuseSound = nullptr;
                    }
                    _explosion->Burst();
                    _explosionLight.Position = blastOrigin;

                    auto soundManager = GetLevel()->GetSoundManager();
                    soundManager->PlaySinglePositionalSound(_explosionSounds.GetNextSound(), Audio::SoundPriority::High, blastOrigin, ExplosiveBlastSoundRadius.first, ExplosiveBlastSoundRadius.second, 1.0f);

                    Rectanglef blastRect(blastOrigin - Vector2f(_blastRadius * 2.0f), Vector2f(_blastRadius * 4.0f));

                    std::vector<Vector2f> hitPoints;
                    std::vector<Character*> hitCharacters = layer->FindIntersections<Character>(blastRect, hitPoints);

                    Character* owner = GetLevelLayer()->GetCharacter(_owner);

                    for (uint32_t i = 0; i < hitCharacters.size(); i++)
                    {
                        bool isException = std::find(_hitExceptions.begin(), _hitExceptions.end(), hitCharacters[i]->GetID()) != _hitExceptions.end();
                        if (_hitsEnemiesOnly && !owner->IsCharacterAttackable(hitCharacters[i], false) && !isException)
                        {
                            continue;
                        }

                        if (!_hitsEnemiesOnly && isException)
                        {
                            continue;
                        }

                        Vector2f hitDirection = hitPoints[i] - blastOrigin;
                        float hitDist = hitDirection.Length();
                        if (hitDist < _blastRadius && hitCharacters[i]->GetID() != GetID())
                        {
                            float hitPerc = (_blastRadius - hitDist) / _blastRadius;

                            Damage dmg(DamageType_Element_Fire | DamageType_Type_Explosion | DamageType_Source_Explosion, _dmg * hitPerc);
                            hitCharacters[i]->ApplyDamage(owner, hitPoints[i], dmg);
                            hitCharacters[i]->ApplyLinearImpulse(Vector2f::Normalize(hitDirection) * (hitPerc * _blastImpulse));
                        }
                    }

                    auto& cameraController = GetLevel()->GetCameraController();
                    cameraController.Shake(blastOrigin, ExplosiveBlastSoundRadius.first, ExplosiveBlastSoundRadius.second, ExplosiveShakeMagnitude, ExplosiveShakeFrequency, ExplosiveShakeDuration);

                    SetSkeletonJointStrength(0.0f);
                    EnablePhysics(true);
                    SetDrawSkeleton(_drawSkeletonAfterExplosion);
                    ApplyRadialImpulse(GetBounds().Middle(), 1500.0f * GetCollision()->GetMass());
                }
            }
        }

        void Explosive::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            SkeletonCharacter::OnDraw(levelRenderer);

            if (_fuse)
            {
                levelRenderer->AddDrawable(_fuse, false);
                if (_countingDown && !_exploding)
                {
                    levelRenderer->AddLight(_fuseLight);
                }
            }

            levelRenderer->AddDrawable(_explosion, false);

            if (_exploding && _timeDead < _blastLightDuration)
            {
                levelRenderer->AddLight(_explosionLight);
            }
        }

        void Explosive::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);

            if (!_fuseName.empty())
            {
                _fuse = EmplaceResource(CreateParticleSystemInstance(contentManager, _fuseParticlesPath));
                AttachDrawable(_fuseName, _fuse);
                _fuse->SetScale(1.5f);
            }

            _explosion = EmplaceResource(CreateParticleSystemInstance(contentManager, _explosionParticlesPath));
            _explosion->SetScale(5.0f);

            _fuseSounds.LoadContent(contentManager);
            _explosionSounds.LoadContent(contentManager);
        }

        void Explosive::OnUnloadContent()
        {
            SkeletonCharacter::OnUnloadContent();

            _fuse.Reset();
            _explosion.Reset();

            _fuseSounds.UnloadContent();
            _explosionSounds.UnloadContent();
        }

        void Explosive::SetExplosionParticles(const std::string& particlePath)
        {
            _explosionParticlesPath = particlePath;
        }

        void Explosive::SetDrawSkeletonAfterExplosion(bool drawSkeleton)
        {
            _drawSkeletonAfterExplosion = drawSkeleton;
        }

        void Explosive::SetHitsEnemiesOnly(bool hitsEnemiesOnly, const std::vector<CharacterID>& exceptions)
        {
            _hitsEnemiesOnly = hitsEnemiesOnly;
            _hitExceptions = exceptions;
        }

        void Explosive::AddFuseSounds(const Audio::SoundPathVector& paths)
        {
            _fuseSounds.AddSounds(paths);
        }

        void Explosive::AddExplosionSounds(const Audio::SoundPathVector& paths)
        {
            _explosionSounds.AddSounds(paths);
        }

        bool Explosive::HasExploded() const
        {
            return _durationRemaining <= 0.0f;
        }
    }

    template <>
    void EnumeratePreloads<Character::Explosive>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);

        preloads.insert(Character::ExplosiveParticlesPath);
        preloads.insert(Character::ExplosiveFuseParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Character::ExplosiveFuseSoundPaths);
    }
}
