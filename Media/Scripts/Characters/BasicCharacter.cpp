#include "Characters/BasicCharacter.hpp"
#include "DamageTypes.hpp"
#include "AttachPoints.hpp"
#include "EmoteTypes.hpp"
#include "Item/Weapon.hpp"
#include "Item/Armor.hpp"
#include "Item/Trinket.hpp"
#include "ContentUtility.hpp"
#include "ParticlesUtility.hpp"

#include "Drawables/OverheadTextDisplay.hpp"
#include "Drawables/EmoteDisplay.hpp"

#include "Items/Weapons/WeaponTraits.hpp"

#include "Particles/ParticleSystemInstance.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string DamageFontPath = "Fonts/numbers_red.spritefont";
        static const std::string HealingFontPath = "Fonts/numbers_green.spritefont";

        static const uint32_t OverheadTextFontSize = 50;
        static const uint32_t OverheadTextFontCriticalSize = 80;

        static const std::pair<float, float> DamageHealingSoundRange = { 1000.0f, 3000.0f };
        static const float DamageSoundVolume = 0.4f;
        static const float DamageCriticalSoundVolume = 0.75f;
        static const float HealingSoundVolume = 0.05f;

        BasicCharacter::BasicCharacter(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath)
            : SkeletonCharacter(parameters, skeletonPath, matsetPath)

            , _mouthName()
            , _headName()
            , _headRotationSpeed(Pi)
            , _hasForcedLookPos(false)

            , _mainArmNames()
            , _offArmNames()
            , _armRotationSpeed(TwoPi)

            , _damageParticles()

            , _runParticleSystemPath()
            , _runParticles(nullptr)
            , _runParticleSystemTags()

            , _feet()
            , _baseFootstepSounds()
            , _terrainFootstepSounds()

            , _jawName()
            , _remainingJawOpenDuration(0.0f)

            , _aggroSounds()
            , _attackSounds()
            , _attackSoundPlayChance(0.3f)
            , _idleSounds()
            , _deathSounds()
            , _affirmativeSounds()
            , _negatorySounds()
            , _tauntSounds()

            , _speechMinDist(1000.0f)
            , _speechMaxDist(3000.0f)

            , _aggroRange(0.0f)

            , _injuredThreshold(0.2f)

            , _physicsTime(0.0f)

            , _dieing(false)
            , _ragDollOnDeath(true)

            , _deathFadeBegin(6.0f)
            , _deathFadeDuration(4.0f)
            , _timeDead(0.0f)

            , _dropWeaponsOnDeath(true)
            , _dropArmorOnDeath(false)
            , _dropTrinketsOnDeath(false)
            , _destroyItemsOnDeath(true)

            , _curPathEdgeType(Pathfinding::EdgeType_Walk)

            , _curEdgeType(Pathfinding::EdgeType_Walk)
            , _startingClimb(false)
            , _endingSlide(false)

            , _fallDmgStartVelocity(1700.0f)
            , _fallDmgMult(1.0f / 50.0f)

            , _lastAttacker(0)
            , _lastDmgPos()

            , _textDisplay(nullptr)
            , _emoteDisplay(nullptr)
        {
        }

        void BasicCharacter::LookAt(const Vector2f& position)
        {
            lookAt(position, true);
        }

        void BasicCharacter::LookTo(const Vector2f& direction)
        {
            lookTo(direction, true);
        }

        void BasicCharacter::StopLooking()
        {
            stopLooking(true);
        }

        void BasicCharacter::PointMainArmAt(const Vector2f& position)
        {
            Animation::SkeletonInstance* skeleton = GetSkeleton();
            for (uint32_t i = 0; i < _mainArmNames.size(); i++)
            {
                skeleton->PointJointAt(_mainArmNames[i], position, _armRotationSpeed);
            }
        }

        void BasicCharacter::PointMainArmTo(const Vector2f& direction)
        {
            Animation::SkeletonInstance* skeleton = GetSkeleton();
            for (uint32_t i = 0; i < _mainArmNames.size(); i++)
            {
                skeleton->PointJointTo(_mainArmNames[i], direction, _armRotationSpeed);
            }
        }

        void BasicCharacter::StopPointingMainArm()
        {
            Animation::SkeletonInstance* skeleton = GetSkeleton();
            for (uint32_t i = 0; i < _mainArmNames.size(); i++)
            {
                skeleton->StopPointingJoint(_mainArmNames[i], _armRotationSpeed);
            }
        }

        void BasicCharacter::PointOffArmAt(const Vector2f& position)
        {
            Animation::SkeletonInstance* skeleton = GetSkeleton();
            for (uint32_t i = 0; i < _offArmNames.size(); i++)
            {
                skeleton->PointJointAt(_offArmNames[i], position, _armRotationSpeed);
            }
        }

        void BasicCharacter::PointOffArmTo(const Vector2f& direction)
        {
            Animation::SkeletonInstance* skeleton = GetSkeleton();
            for (uint32_t i = 0; i < _offArmNames.size(); i++)
            {
                skeleton->PointJointTo(_offArmNames[i], direction, _armRotationSpeed);
            }
        }

        void BasicCharacter::StopPointingOffArm()
        {
            Animation::SkeletonInstance* skeleton = GetSkeleton();
            for (uint32_t i = 0; i < _offArmNames.size(); i++)
            {
                skeleton->StopPointingJoint(_offArmNames[i], _armRotationSpeed);
            }
        }

        bool BasicCharacter::HasRangedAttackFireAnimationPassed(const Item::Weapon* weap) const
        {
            return IsInAttackDmgRange(weap);
        }

        void BasicCharacter::AddOverheadText(const Graphics::PreparedText& text)
        {
            const Animation::SkeletonInstance* skeleton = GetSkeleton();

            Rectanglef dmgDisplayBounds;
            if (skeleton->HasJoint(_headName))
            {
                dmgDisplayBounds = skeleton->GetJointBounds(_headName).Bounds();
            }
            else
            {
                dmgDisplayBounds = GetBounds();
            }

            Vector2f mid(dmgDisplayBounds.Middle().X, dmgDisplayBounds.Top());
            Vector2f midRange(dmgDisplayBounds.W * 0.3f, dmgDisplayBounds.H * 0.15f);
            Vector2f displayPos = mid + (Vector2f::FromRotation(Random::RandomBetween(0.0f, TwoPi), Random::RandomBetween(0.0f, 1.0f)) * midRange);

            _textDisplay->AddText(text, displayPos);
        }

        void BasicCharacter::Emote(EmoteType type, float duration)
        {
            _emoteDisplay->Emote(type, duration);
        }

        void BasicCharacter::OnSpawn()
        {
            SkeletonCharacter::OnSpawn();
        }

        void BasicCharacter::OnDeath()
        {
            SkeletonCharacter::OnDeath();

            Speak(Speech_Death, true);

            if (_ragDollOnDeath)
            {
                _dieing = true;
                DoDeathRagdoll(true);
                DoDeathDropItems();
            }
            else if (_deathAnimations.Count() > 0)
            {
                _dieing = true;
                PlayAnimationSet(_deathAnimations, false, 0.1f, 0.0f);
            }
            else
            {
                Terminate();
            }
        }

        Character::AttackInfo BasicCharacter::AttackTarget(Character* target)
        {
            _curDmgRangeTags.clear();
            _curDmgResetTags.clear();
            _curAttackSoundTags.clear();
            _curWooshSoundTags.clear();

            const float targetRange = Vector2f::Distance(GetPosition(), target->GetPosition());

            std::map<attackAnimationRequirements*, Animation::AnimationSet> matchingAnimationSets;
            for (auto& animation : _attackAnimations)
            {
                bool matches = true;
                for (const auto& requirement : animation.requiredAttachments)
                {
                    if (_weaponAttachmentLocations[requirement.first].equippedWeaponType != requirement.second)
                    {
                        matches = false;
                        break;
                    }
                }


                if (matches)
                {
                    for (const auto& rangeRequirement : animation.animationRanges)
                    {
                        const AttackRange& range = rangeRequirement.second;
                        if (targetRange >= range.first && targetRange <= range.second)
                        {
                            const std::string& animationName = rangeRequirement.first;
                            matchingAnimationSets[&animation].AddAnimation(animationName, animation.animationWeights[animationName]);
                        }
                    }
                }
            }

            if (!matchingAnimationSets.empty())
            {
                const std::pair<attackAnimationRequirements*, Animation::AnimationSet>& animation =
                    *Random::RandomItem(matchingAnimationSets);

                attackAnimationRequirements& animationRequirements = *animation.first;
                const std::string& nextAnimName = animation.second.PeekNextAnimation();

                float animLength = PlayAnimation(nextAnimName, false, 0.05f, 0.0f);

                _curDmgRangeTags.clear();
                for (const auto& attackWeapon : animationRequirements.requiredAttachments)
                {
                    const auto& attackWeaponAttachment = _weaponAttachmentLocations[attackWeapon.first];
                    _curDmgRangeTags[attackWeaponAttachment.equippedWeapon] = animationRequirements.animationDmgRanges[nextAnimName][attackWeapon.first];
                    _curDmgResetTags[attackWeaponAttachment.equippedWeapon] = animationRequirements.animationDmgResetTags[nextAnimName][attackWeapon.first];

                    const auto& attackTags = animationRequirements.animationAttackSoundTags[nextAnimName][attackWeapon.first];
                    _curAttackSoundTags.insert(attackTags.begin(), attackTags.end());

                    const auto& wooshTags = animationRequirements.animationWooshSoundTags[nextAnimName][attackWeapon.first];
                    for (const auto& wooshTag : wooshTags)
                    {
                        _curWooshSoundTags[wooshTag] = attackWeapon.second;
                    }
                }

                AttackInfo attackInfo;
                attackInfo.Duration = animLength;

                for (const auto& requirement : animationRequirements.requiredAttachments)
                {
                    Item::ItemID weap = _weaponAttachmentLocations[requirement.first].equippedWeapon;
                    attackInfo.Weapons[weap] = _curDmgResetTags[weap].size() + 1;
                }

                return attackInfo;
            }
            else
            {
                return SkeletonCharacter::AttackTarget(target);
            }
        }

        bool BasicCharacter::IsInAttackDmgRange(const Item::Weapon* weap) const
        {
            assert(weap != nullptr && _curDmgRangeTags.find(weap->GetID()) != _curDmgRangeTags.end());
            const auto& dmgRangeTags = _curDmgRangeTags.at(weap->GetID());
            if (dmgRangeTags.empty())
            {
                return true;
            }

            const Animation::SkeletonInstance* skeleton = GetSkeleton();

            for (const auto& dmgRangeTag : dmgRangeTags)
            {
                if (dmgRangeTag.first == dmgRangeTag.second)
                {
                    if (skeleton->HasAnimationTagJustPassed(dmgRangeTag.first))
                    {
                        return true;
                    }
                }
                else
                {
                    if (skeleton->IsBetweenAnimationTags(dmgRangeTag.first, dmgRangeTag.second))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        Rayf BasicCharacter::GetWeaponAttachPoint(const std::string& name, Item::WeaponType type) const
        {
            const Animation::SkeletonInstance* skeleton = GetSkeleton();

            for (const auto& attachmentLocation : _weaponAttachmentLocations)
            {
                auto iter = attachmentLocation.second.jointNames.find(type);
                if (iter != attachmentLocation.second.jointNames.end() && iter->second == name)
                {
                    const Vector2f& attachA = skeleton->GetJointPosition(attachmentLocation.second.attachA);
                    const Vector2f& attachB = skeleton->GetJointPosition(attachmentLocation.second.attachB);
                    return Rayf(attachA, attachB - attachA);
                }
            }

            return SkeletonCharacter::GetWeaponAttachPoint(name, type);
        }

        float BasicCharacter::GetAggroRange() const
        {
            return _aggroRange;
        }

        MaterialType BasicCharacter::GetBodyMaterial() const
        {
            return _bodyMaterial;
        }

        Vector2f BasicCharacter::GetMouthPosition() const
        {
            if (!_mouthName.empty())
            {
                return GetSkeleton()->GetJointPosition(_mouthName);
            }
            else
            {
                return GetHeadPosition();
            }
        }

        Vector2f BasicCharacter::GetHeadPosition() const
        {
            if (!_headName.empty())
            {
                return GetSkeleton()->GetJointPosition(_headName);
            }
            else
            {
                return GetBounds().Middle();
            }
        }

        Vector2f BasicCharacter::GetFeetPosition() const
        {
            if (_feet.empty())
            {
                return GetBounds().Middle();
            }
            else
            {

                Vector2f footPositionSum;
                for (auto& foot : _feet)
                {
                    const Vector2f footFront = GetAttachPoint(foot.front).Position;
                    const Vector2f footBack = GetAttachPoint(foot.back).Position;
                    footPositionSum += footFront + footBack;
                }
                return footPositionSum / Vector2f(_feet.size() * 2);
            }
        }

        bool BasicCharacter::WantsToLookAtMouse() const
        {
            for (const auto& mouseLookItem : GetItems<Item::MouseLooker>())
            {
                if (mouseLookItem->WantsToLookAtMouse())
                {
                    return true;
                }
            }

            return false;
        }

        void BasicCharacter::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);

            for (auto& damageParticles : _damageParticles)
            {
                damageParticles.particles = Content::CreateParticleSystemInstance(contentManager, damageParticles.path);
            }
            
            if (!_healParticlesPath.empty())
            {
                _healParticles = Content::CreateParticleSystemInstance(contentManager, _healParticlesPath);
            }

            if (_runParticleSystemPath.size() > 0)
            {
                _runParticles = contentManager->Load<Particles::ParticleSystem>(_runParticleSystemPath);
                for (uint32_t i = 0; i < _feet.size(); i++)
                {
                    _feet[i].particleSystem = new Particles::ParticleSystemInstance(_runParticles);
                }
            }

            _aggroSounds.LoadContent(contentManager);
            _attackSounds.LoadContent(contentManager);
            for (auto& attackWooshSound : _attackWooshSounds)
            {
                attackWooshSound.second.LoadContent(contentManager);
            }
            _idleSounds.LoadContent(contentManager);
            _deathSounds.LoadContent(contentManager);
            _affirmativeSounds.LoadContent(contentManager);
            _negatorySounds.LoadContent(contentManager);
            _tauntSounds.LoadContent(contentManager);
            for (auto& damageSound : _damageSounds)
            {
                damageSound.sounds.LoadContent(contentManager);
            }
            _healingSounds.LoadContent(contentManager);

            _baseFootstepSounds.LoadContent(contentManager);
            for (auto& footstepSounds : _terrainFootstepSounds)
            {
                footstepSounds.second.LoadContent(contentManager);
            }

            _damageFont = contentManager->Load<Graphics::Font>(DamageFontPath);
            _healingFont = contentManager->Load<Graphics::Font>(HealingFontPath);

            _textDisplay = new Graphics::OverheadTextDisplay();

            _emoteDisplay = new Graphics::EmoteDisplay();
            const Graphics::HUDMaterialSet* emotesMatset = contentManager->Load<Graphics::HUDMaterialSet>("HUD/emotes.hudmatset");
            _emoteDisplay->SetEmoteMaterial(Emote_Confused, emotesMatset->GetMaterial("emote_confused"));
            _emoteDisplay->SetEmoteMaterial(Emote_Surprised, emotesMatset->GetMaterial("emote_surprised"));
            SafeRelease(emotesMatset);
        }

        void BasicCharacter::OnUnloadContent()
        {
            SkeletonCharacter::OnUnloadContent();

            for (auto& damageParticles : _damageParticles)
            {
                SafeRelease(damageParticles.particles);
            }
            
            SafeRelease(_healParticles);

            SafeRelease(_runParticles);
            for (uint32_t i = 0; i < _feet.size(); i++)
            {
                SafeRelease(_feet[i].particleSystem);
            }

            _aggroSounds.UnloadContent();
            _attackSounds.UnloadContent();
            for (auto& attackWooshSound : _attackWooshSounds)
            {
                attackWooshSound.second.UnloadContent();
            }
            _idleSounds.UnloadContent();
            _deathSounds.UnloadContent();
            _affirmativeSounds.UnloadContent();
            _negatorySounds.UnloadContent();
            _tauntSounds.UnloadContent();
            for (auto& damageSound : _damageSounds)
            {
                damageSound.sounds.UnloadContent();
            }
            _healingSounds.UnloadContent();

            _baseFootstepSounds.UnloadContent();
            for (auto& footstepSounds : _terrainFootstepSounds)
            {
                footstepSounds.second.UnloadContent();
            }

            SafeRelease(_damageFont);
            SafeRelease(_healingFont);

            SafeRelease(_textDisplay);
            SafeRelease(_emoteDisplay);
        }

        template <typename T>
        void updateAlpha(T* object, float newAlpha, float t)
        {
            Color color = object->GetColor();
            color.A = Saturate(Lerp(color.A / 255.0f, newAlpha, t)) * 255;
            object->SetColor(color);
        }

        void BasicCharacter::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            Animation::SkeletonInstance* skeleton = GetSkeleton();

            if (IsAlive())
            {
                CharacterState curState = GetCurrentState();
                if (curState == CharacterState_Idle)
                {
                    Character* nearbyEnemy = FindNearbyAttackableTarget(Max(GetAggroRange(), GetWeaponRange() * 0.1f));
                    if (nearbyEnemy)
                    {
                        Emote(Emote_Surprised, 2.0f);
                        Speak(Speech_Aggro, true);
                        PushAction(CreateWeakAttackAction(nearbyEnemy->GetID()), false);
                        aggroNearbyFriendlies(nearbyEnemy);
                    }

                    updateIdleAnimation();

                    _idleSoundTimer -= dt;
                    if (_idleSoundTimer <= 0.0f)
                    {
                        Speak(Speech_Idle, true);
                        _idleSoundTimer = Random::RandomBetween(_idleSoundTimeout.first, _idleSoundTimeout.second);
                    }
                }
                else if (curState == CharacterState_Physics)
                {
                    _physicsTime += dt;

                    static const float FallDamagePhysicsTimeRequirement = 0.5f;

                    const Physics::Collision* collision = GetCollision();
                    if (_physicsTime > FallDamagePhysicsTimeRequirement && collision != nullptr && !collision->GetCurrentContacts().empty())
                    {
                        for (const Physics::Contact& contact : collision->GetCurrentContacts())
                        {
                            float velocityIntoSurface = Vector2f::Dot(contact.Velocity, -contact.Normal);
                            if (velocityIntoSurface >= _fallDmgStartVelocity)
                            {
                                Damage dmg(DamageType_Type_Fall | DamageType_Source_Crushing | DamageType_Element_Physical, (velocityIntoSurface - _fallDmgStartVelocity) * _fallDmgMult);
                                ApplyDamage(nullptr, contact.Position, dmg);
                            }
                        }
                    }
                }
                else if (IsMoving())
                {
                    updateMoveAnimation();
                }
            }

            // If attacking, look at the target
            Character* attackTarget = GetAttackTarget();
            if (IsAlive() && attackTarget)
            {
                if (!_headName.empty())
                {
                    Vector2f headPos = GetAttachPoint(_headName).Position;
                    Vector2f lookPos = attackTarget->GetBounds().Middle();

                    if (IsA<BasicCharacter>(attackTarget))
                    {
                        BasicCharacter* attackTargetBasicCharacter = AsA<BasicCharacter>(attackTarget);
                        if (attackTargetBasicCharacter->_headName.size() > 0)
                        {
                            // Look at the closest point between the target's middle and head
                            const Polygonf& targetHeadBounds = attackTargetBasicCharacter->GetSkeleton()->GetJointBounds(attackTargetBasicCharacter->_headName);
                            Math::PointToLineDistance(Vector2f(lookPos.X, targetHeadBounds.Bounds().Top()), Vector2f(lookPos.X, lookPos.Y), headPos, lookPos);
                        }
                    }

                    lookAt(lookPos, false);
                }

                for (const auto& dmgResets : _curDmgResetTags)
                {
                    for (const auto& dmgResetTag : dmgResets.second)
                    {
                        if (skeleton->HasAnimationTagJustPassed(dmgResetTag))
                        {
                            ResetHitAttackTargets(GetItem<Item::Weapon>(dmgResets.first));
                        }
                    }
                }
                for (const auto& attackSoundTag : _curAttackSoundTags)
                {
                    if (skeleton->HasAnimationTagJustPassed(attackSoundTag) && Random::RandomBetween(0.0f, 1.0f) < _attackSoundPlayChance)
                    {
                        Speak(_attackSounds, false, true);
                    }
                }
                for (const auto& wooshSoundTag : _curWooshSoundTags)
                {
                    if (skeleton->HasAnimationTagJustPassed(wooshSoundTag.first))
                    {
                        auto soundManager = GetLevel()->GetSoundManager();
                        soundManager->PlaySinglePositionalSound(_attackWooshSounds[wooshSoundTag.second].GetNextSound(),
                                                                Audio::SoundPriority::High, GetMouthPosition(), _speechMinDist, _speechMaxDist,
                                                                _speechPositionalVolume);
                    }
                }
            }
            else if (IsAlive() && IsMoving())
            {
                std::shared_ptr<const Pathfinding::Path> curPath = GetCurrentPath();
                assert(curPath != nullptr);

                lookTo(curPath->GetAverageDirection(GetBounds().W), false);
            }
            else
            {
                stopLooking(false);
            }

            for (auto& damageParticles : _damageParticles)
            {
                damageParticles.particles->Update(totalTime, dt);
            }
            
            if (_healParticles)
            {
                _healParticles->Update(totalTime, dt);
            }

            bool burstFootParticles = false;
            for (const auto& runParticleSystemTag : _runParticleSystemTags)
            {
                if (skeleton->HasAnimationTagJustPassed(runParticleSystemTag))
                {
                    burstFootParticles = true;
                    break;
                }
            }

            for (auto& foot : _feet)
            {
                if (foot.particleSystem)
                {
                    if (burstFootParticles)
                    {
                        const Vector2f footFront = GetAttachPoint(foot.front).Position;
                        const Vector2f footBack = GetAttachPoint(foot.back).Position;

                        foot.particleSystem->SetLineSpawner(footFront, footBack);

                        Rotatorf rotation(0.0f);
                        if (footFront != footBack)
                        {
                            rotation = footFront - footBack;
                        }
                        else
                        {
                            auto path = GetCurrentPath();
                            if (path)
                            {
                                rotation = Rotatorf::Reflect(path->GetCurrentRotation(), Rotatorf::PiOver2);
                            }
                        }

                        foot.particleSystem->SetInvertedX(skeleton->IsInvertedX());
                        foot.particleSystem->SetInvertedY(skeleton->IsInvertedY());
                        foot.particleSystem->SetRotation(rotation);

                        foot.particleSystem->Burst();
                    }

                    foot.particleSystem->Update(totalTime, dt);
                }
            }

            const auto& currentPath = GetCurrentPath();
            if (burstFootParticles && _feet.size() > 0 && currentPath != nullptr)
            {
                Vector2f avgFootPos = GetFeetPosition();
                float footstepMinDist = _speechMinDist;
                float footstepMaxDist = _speechMaxDist;
                Pathfinding::TerrainType curTurrain = currentPath->GetCurrentPosition()->GetTerrain();

                auto soundManager = GetLevel()->GetSoundManager();

                auto baseSound = _baseFootstepSounds.GetNextSound();
                soundManager->PlaySinglePositionalSound(baseSound, Audio::SoundPriority::Low, avgFootPos,
                    footstepMinDist, footstepMaxDist, Random::RandomBetween(_footstepVolumeRange.first, _footstepVolumeRange.second));

                auto terrainSound = _terrainFootstepSounds[curTurrain].GetNextSound();
                soundManager->PlaySinglePositionalSound(terrainSound, Audio::SoundPriority::Low, avgFootPos,
                    footstepMinDist, footstepMaxDist, Random::RandomBetween(_footstepVolumeRange.first, _footstepVolumeRange.second));

                if (_footstepCameraShakeEnabled)
                {
                    float length = 0.1f;
                    if (baseSound)
                    {
                        length = Max(length, baseSound->GetLength());
                    }
                    if (terrainSound)
                    {
                        length = Max(length, terrainSound->GetLength());
                    }

                    static const float MaximumStepShake = 0.8f;
                    length = Min(length, MaximumStepShake);

                    auto& cameraController = GetLevel()->GetCameraController();
                    cameraController.Shake(avgFootPos, footstepMinDist, footstepMaxDist, _footstepCameraShakeMagnitude, _footstepCameraShakeFrequency, length);
                }
            }

            if (_weaponAlphaTimer >= 0.0f)
            {
                _weaponAlphaTimer -= dt;
                float t = 1.0f - Saturate(_weaponAlphaTimer / _weaponAlphaTotalTime);
                for (Item::Weapon* weapon : GetWeapons())
                {
                    updateAlpha(weapon, _targetWeaponAlpha, t);
                }
            }

            if (_dieing)
            {
                _timeDead += dt;

                if (_timeDead > _deathFadeBegin)
                {
                    float alpha = 1.0f - Clamp((_timeDead - _deathFadeBegin) / _deathFadeDuration, 0.0f, 1.0f);
                    SetWeaponAlpha(alpha, 0.1f);
                    updateAlpha(this, alpha, 1.0f);
                }

                if (_ragDollOnDeath)
                {
                    if (_timeDead > _deathFadeBegin + _deathFadeDuration)
                    {
                        Terminate();
                    }
                }
                else if (!IsPlayingAnimation())
                {
                    Terminate();
                }
            }

            _textDisplay->Update(totalTime, dt);

            const Rectanglef& emoteHeadLocation = skeleton->HasJoint(_headName) ? skeleton->GetJointBounds(_headName).Bounds() : GetBounds();
            _emoteDisplay->Update(emoteHeadLocation, totalTime, dt);

            auto vocalizationIter = _vocalizations.begin();
            while (vocalizationIter != _vocalizations.end())
            {
                const auto& vocalization = vocalizationIter->lock();
                if (vocalization == nullptr)
                {
                    vocalizationIter = _vocalizations.erase(vocalizationIter);
                    continue;
                }

                vocalization->SetPosition(GetMouthPosition());
                vocalizationIter++;
            }

            _remainingJawOpenDuration -= dt;
            if (!_jawName.empty())
            {
                if (!_vocalizations.empty())
                {
                    // Keep the jaw open while there are vocalizations
                    _remainingJawOpenDuration = Max(_remainingJawOpenDuration, dt);
                }

                if (_remainingJawOpenDuration > 0.0f)
                {
                    const float jawRotationSpeed = TwoPi / 2.0f;
                    const float jawCloseThreshold = 0.15f;

                    Vector2f jointTargetPos = _remainingJawOpenDuration > jawCloseThreshold ? Vector2f(0.0f, 1000.0f) : Vector2f(0.0f, -1000.0f);

                    skeleton->PointJointTo(_jawName, jointTargetPos, jawRotationSpeed);
                }
                else
                {
                    skeleton->StopPointingJoint(_jawName);
                }
            }
        }

        void BasicCharacter::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            SkeletonCharacter::OnDraw(levelRenderer);

            for (const auto& damageParticles : _damageParticles)
            {
                if (damageParticles.particles->GetParticleCount() > 0)
                {
                    levelRenderer->AddDrawable(damageParticles.particles, false);
                }
            }

            if (_healParticles && _healParticles->GetParticleCount() > 0)
            {
                levelRenderer->AddDrawable(_healParticles, false);
            }
            
            for (uint32_t i = 0; i < _feet.size(); i++)
            {
                if (_feet[i].particleSystem)
                {
                    levelRenderer->AddDrawable(_feet[i].particleSystem, false);
                }
            }

            _textDisplay->Draw(levelRenderer);
            _emoteDisplay->Draw(levelRenderer);
        }

        std::shared_ptr<Audio::ManagedSoundInstance> BasicCharacter::Speak(const Audio::SoundSet& sounds, bool loop, bool positional)
        {
            auto sound = sounds.GetNextSound();
            if (sound == nullptr)
            {
                return nullptr;
            }

            float volume = positional ? _speechPositionalVolume : _speechGlobalVolume;

            auto soundManager = GetLevel()->GetSoundManager();
            std::shared_ptr<Audio::ManagedSoundInstance> soundInstance = nullptr;
            if (loop)
            {
                if (positional)
                {
                    soundInstance = soundManager->PlayLoopingPositionalSound(sound, Audio::SoundPriority::High, GetMouthPosition(), _speechMinDist, _speechMaxDist, volume);
                }
                else
                {
                    soundInstance = soundManager->PlayLoopingGlobalSound(sound, Audio::SoundPriority::High, volume);
                }
            }
            else
            {
                if (positional)
                {
                    soundInstance = soundManager->PlaySinglePositionalSound(sound, Audio::SoundPriority::High, GetMouthPosition(), _speechMinDist, _speechMaxDist, volume);
                }
                else
                {
                    soundInstance = soundManager->PlaySingleGlobalSound(sound, Audio::SoundPriority::High, volume);
                }
            }

            if (!loop)
            {
                float soundDuration = sound->GetLength();
                const float minJawOpenTime = 0.5f;
                const float jawTimeClip = 0.5f;
                float targetJawOpenTime = Max(soundDuration - jawTimeClip, minJawOpenTime);

                _remainingJawOpenDuration = Max(_remainingJawOpenDuration, targetJawOpenTime);
            }

            assert(soundInstance != nullptr);
            _vocalizations.push_back(soundInstance);
            return soundInstance;
        }

        const Audio::SoundSet& BasicCharacter::getSoundsForSpeech(SpeechType type) const
        {
            switch (type)
            {
            case Speech_Affirmative: return _affirmativeSounds;
            case Speech_Negatory: return _negatorySounds;
            case Speech_Aggro: return _aggroSounds;
            case Speech_Taunt: return _tauntSounds;
            case Speech_Death: return _deathSounds;
            case Speech_Idle: return _idleSounds;
            default: assert(false); return _deathSounds;
            }
        }

        std::shared_ptr<Audio::ManagedSoundInstance> BasicCharacter::Speak(SpeechType type, bool positional)
        {
            return Speak(getSoundsForSpeech(type), false, positional);
        }

        void BasicCharacter::aggroNearbyFriendlies(const Character* target) const
        {
            if (GetController() != nullptr)
            {
                const float warnDist = GetAggroRange();
                Vector2f middle = GetBounds().Middle();
                std::vector<Character*> nearbyFriends = GetController()->GetCharacters<Character>([&](const Character* character)
                {
                    if (character->GetCurrentState() != CharacterState_Idle)
                    {
                        return false;
                    }

                    if (Vector2f::DistanceSquared(middle, character->GetBounds().Middle()) > warnDist * warnDist)
                    {
                        return false;
                    }

                    if (!HasLineOfSight(character))
                    {
                        return false;
                    }

                    if (!character->IsCharacterAttackable(target, false))
                    {
                        return false;
                    }

                    return true;
                });

                for (auto& nearbyFriend : nearbyFriends)
                {
                    nearbyFriend->PushAction(CreateWeakAttackAction(target->GetID()), false);
                    if (IsA<BasicCharacter>(nearbyFriend))
                    {
                        AsA<BasicCharacter>(nearbyFriend)->Emote(Emote_Surprised, 2.0f);
                    }
                }
            }
        }

        Vector2f BasicCharacter::getDeathPosition() const
        {
            Vector2f deathSourcePos = _lastDmgPos;
            if (_lastAttacker)
            {
                Character* deathSourceCharacter = GetLevelLayer()->GetCharacter(_lastAttacker);
                if (deathSourceCharacter != nullptr)
                {
                    deathSourcePos = deathSourceCharacter->GetBounds().Middle();
                }
            }

            return deathSourcePos;
        }

        void BasicCharacter::DoDeathRagdoll(bool forcedPhysics)
        {
            Vector2f deathSourcePos = getDeathPosition();

            Physics::Collision* collision = GetCollision();

            EnablePhysics(forcedPhysics);
            collision->SetBehavior(Physics::CollisionBehavior_Dynamic, forcedPhysics);
            ApplyLinearImpulse(GetDeathImpulse(collision));
        }

        void BasicCharacter::DoDeathDropItems()
        {
            doDeathDropItemsImpl<Item::Weapon>(_dropWeaponsOnDeath, _destroyItemsOnDeath);
            doDeathDropItemsImpl<Item::Armor>(_dropArmorOnDeath, _destroyItemsOnDeath);
            doDeathDropItemsImpl<Item::Trinket>(_dropTrinketsOnDeath, _destroyItemsOnDeath);
        }

        template <typename T>
        void BasicCharacter::doDeathDropItemsImpl(bool drop, bool destroy)
        {
            Vector2f deathSourcePos = getDeathPosition();

            for (auto item : GetItems<T>())
            {
                DiscardItem(item);

                if (drop)
                {
                    Physics::Collision* collision = item->GetCollision();
                    if (collision)
                    {
                        item->ApplyLinearImpulse(GetDeathImpulse(collision));
                    }
                }

                if (destroy)
                {
                    item->Destroy();
                }
            }
        }

        void BasicCharacter::SetWeaponAlpha(float alpha, float time)
        {
            _targetWeaponAlpha = alpha;
            _weaponAlphaTimer = Max(time, 0.0f);
            _weaponAlphaTotalTime = Max(time, Epsilon);
        }

        Vector2f BasicCharacter::GetDeathImpulse(const Physics::Collision* collision) const
        {
            Vector2f deathSourcePos = getDeathPosition();
            float mid = GetBounds().Middle().X;
            float impulse = Sign(mid - deathSourcePos.X) * collision->GetMass() * 750.0f;
            return Vector2f(impulse, 0.0f);
        }

        void BasicCharacter::AddWeaponAttachment(const std::string& name, const std::string attachA, const std::string attachB,
                                                 const std::map<Item::WeaponType, std::string>& weapTypesAttachJoints)
        {
            weaponAttachment attachment;
            attachment.attachA = attachA;
            attachment.attachB = attachB;
            attachment.jointNames = weapTypesAttachJoints;
            attachment.equippedWeaponType = Item::WeaponType_None;
            _weaponAttachmentLocations[name] = attachment;

            for (const auto& weapTypesAttachJoint : weapTypesAttachJoints)
            {
                _weaponAttachmentTypes[weapTypesAttachJoint.first].push_back(name);
            }
        }

        void BasicCharacter::ClearWeaponAttachments()
        {
            _weaponAttachmentLocations.clear();
            _weaponAttachmentTypes.clear();
            assignWeapons();
        }

        void BasicCharacter::AddAttackAnimation(const std::string& animName, float weight,
                                                const std::map<std::string, AttackAnimation>& attachments,
                                                const AttackRange& attackRange)
        {
            std::map<std::string, Item::WeaponType> requiredAttachments;
            for (const auto& attachment : attachments)
            {
                requiredAttachments[attachment.first] = attachment.second.Type;
            }

            for (auto& animation : _attackAnimations)
            {
                if (animation.requiredAttachments == requiredAttachments)
                {
                    animation.animationWeights[animName] = weight;

                    for (const auto& attachment : attachments)
                    {
                        animation.animationDmgRanges[animName][attachment.first] = attachment.second.DamageRangeTags;
                        animation.animationDmgResetTags[animName][attachment.first] = attachment.second.DamageResetTags;
                        animation.animationAttackSoundTags[animName][attachment.first] = attachment.second.AttackSoundTags;
                        animation.animationWooshSoundTags[animName][attachment.first] = attachment.second.WooshSoundTags;
                        animation.animationRanges[animName] = attackRange;
                    }
                    return;
                }
            }

            attackAnimationRequirements newAnim;
            newAnim.requiredAttachments = requiredAttachments;
            newAnim.animationWeights[animName] = weight;
            for (const auto& attachment : attachments)
            {
                newAnim.animationDmgRanges[animName][attachment.first] = attachment.second.DamageRangeTags;
                newAnim.animationDmgResetTags[animName][attachment.first] = attachment.second.DamageResetTags;
                newAnim.animationAttackSoundTags[animName][attachment.first] = attachment.second.AttackSoundTags;
                newAnim.animationWooshSoundTags[animName][attachment.first] = attachment.second.WooshSoundTags;
                newAnim.animationRanges[animName] = attackRange;
            }

            _attackAnimations.push_back(newAnim);
        }

        void BasicCharacter::ClearAttackAnimations()
        {
            _attackAnimations.clear();
        }

        void BasicCharacter::AddInteractAnimation(const std::string& animName, float weight)
        {
            _interactAnimations.AddAnimation(animName, weight);
        }

        void BasicCharacter::SetMouthName(const std::string& jointName)
        {
            _mouthName = jointName;
        }

        void BasicCharacter::SetHeadName(const std::string& jointName)
        {
            _headName = jointName;
        }

        void BasicCharacter::AddMainArmJoint(const std::string& jointName)
        {
            _mainArmNames.push_back(jointName);
        }

        void BasicCharacter::AddOffArmJoint(const std::string& jointName)
        {
            _offArmNames.push_back(jointName);
        }

        void BasicCharacter::AddFootJoint(const std::string& jointAName, const std::string& jointBName)
        {
            foot foot;
            foot.front = jointAName;
            foot.back = jointBName;
            foot.particleSystem = _runParticles ? new Particles::ParticleSystemInstance(_runParticles) : nullptr;
            _feet.push_back(foot);
        }

        void BasicCharacter::SetJawJoint(const std::string& jointName)
        {
            _jawName = jointName;
        }

        void BasicCharacter::AddTerrainAnimation(AnimationType type, Pathfinding::EdgeType edgeType, Pathfinding::TerrainType terrainType,
                                                 float minAngle, AnimationVariant variant, const std::string& animationName, float weight)
        {
            _terrainAnimations[type][edgeType][terrainType][minAngle][variant].AddAnimation(animationName, weight);
        }

        void BasicCharacter::ClearTerrainAnimations(AnimationType type, Pathfinding::EdgeType edgeType)
        {
            for (auto& edgeTypeAnimation : _terrainAnimations[type])
            {
                if ((edgeTypeAnimation.first & edgeType) != 0)
                {
                    edgeTypeAnimation.second.clear();
                }
            }
        }

        void BasicCharacter::AddDamagedParticleSystem(const DamageType& damageType, const std::string& particleSystemPath, const std::string& spawnJoint)
        {
            damageParticles parts;
            parts.damageType = damageType;
            parts.path = particleSystemPath;
            parts.spawnJoint = spawnJoint;

            _damageParticles.push_back(parts);
        }

        void BasicCharacter::AddHealingParticleSystem(const std::string& particleSystemPath)
        {
            _healParticlesPath = particleSystemPath;
        }
        
        void BasicCharacter::SetAggroRange(float range)
        {
            _aggroRange = range;
        }

        void BasicCharacter::SetRunParticleSystem(const std::string& particleSystemPath)
        {
            _runParticleSystemPath = particleSystemPath;
        }

        void BasicCharacter::AddRunParticleTag(const std::string& animationTagName)
        {
            _runParticleSystemTags.push_back(animationTagName);
        }

        void BasicCharacter::AddAggroSounds(const Audio::SoundPathVector& sounds)
        {
            _aggroSounds.AddSounds(sounds);
        }

        void BasicCharacter::AddAttackSounds(const Audio::SoundPathVector& sounds)
        {
            _attackSounds.AddSounds(sounds);
        }

        void BasicCharacter::AddAttackWooshSounds(Item::WeaponType weaponType, const Audio::SoundPathVector& sounds)
        {
            _attackWooshSounds[weaponType].AddSounds(sounds);
        }

        void BasicCharacter::AddAttackWooshSounds(const std::map<Item::WeaponType, Audio::SoundPathVector>& sounds)
        {
            for (const auto& weaponType : sounds)
            {
                AddAttackWooshSounds(weaponType.first, weaponType.second);
            }
        }

        void BasicCharacter::SetAttackSoundPlayChance(float chance)
        {
            _attackSoundPlayChance = Clamp(chance, 0.0f, 1.0f);
        }

        void BasicCharacter::AddIdleSounds(const Audio::SoundPathVector& sounds)
        {
            _idleSounds.AddSounds(sounds);
        }

        void BasicCharacter::SetIdleSoundInterval(float minTimeout, float maxTimeout)
        {
            _idleSoundTimeout = std::make_pair(minTimeout, maxTimeout);
        }

        void BasicCharacter::AddDeathSounds(const Audio::SoundPathVector& sounds)
        {
            _deathSounds.AddSounds(sounds);
        }

        void BasicCharacter::AddAffirmativeSounds(const Audio::SoundPathVector& sounds)
        {
            _affirmativeSounds.AddSounds(sounds);
        }

        void BasicCharacter::AddNegatorySounds(const Audio::SoundPathVector& sounds)
        {
            _negatorySounds.AddSounds(sounds);
        }

        void BasicCharacter::AddTauntSounds(const Audio::SoundPathVector& sounds)
        {
            _tauntSounds.AddSounds(sounds);
        }

        void BasicCharacter::SetGlobalSpeechParameters(float volume)
        {
            _speechGlobalVolume = volume;
        }

        void BasicCharacter::SetPositionalSpeechParameters(float volume, float minDist, float maxDist)
        {
            _speechPositionalVolume = volume;
            _speechMinDist = minDist;
            _speechMaxDist = maxDist;
        }

        void BasicCharacter::AddDamageSounds(const Audio::DamageSoundPaths& sounds)
        {
            bool foundMatch = false;
            for (auto& currentDamageSound : _damageSounds)
            {
                if (currentDamageSound.damageType == sounds.DamageType &&
                    currentDamageSound.materialType == sounds.MaterialType)
                {
                    currentDamageSound.sounds.AddSounds(sounds.Sounds);
                    foundMatch = true;
                    break;
                }
            }

            if (!foundMatch)
            {
                damageSoundSet newSet;
                newSet.damageType = sounds.DamageType;
                newSet.materialType = sounds.MaterialType;
                newSet.sounds.AddSounds(sounds.Sounds);
                _damageSounds.push_back(newSet);
            }
        }

        void BasicCharacter::AddDamageSounds(const std::vector<Audio::DamageSoundPaths>& sounds)
        {
            for (const auto& newDamageSound : sounds)
            {
                AddDamageSounds(newDamageSound);
            }
        }

        void BasicCharacter::AddHealingSounds(const Audio::SoundPathVector& sounds)
        {
            _healingSounds.AddSounds(sounds);
        }

        void BasicCharacter::AddBaseFootstepSounds(const Audio::SoundPathVector& sounds)
        {
            _baseFootstepSounds.AddSounds(sounds);
        }

        void BasicCharacter::AddTerrainFootstepSounds(Pathfinding::TerrainType terrainType, const Audio::SoundPathVector& sounds)
        {
            _terrainFootstepSounds[terrainType].AddSounds(sounds);
        }

        void BasicCharacter::SetFootstepVolumeRange(float minVolume, float maxVolume)
        {
            _footstepVolumeRange = std::make_pair(minVolume, maxVolume);
        }

        void BasicCharacter::EnableFootstepCameraShake(float magnitude, float frequency)
        {
            _footstepCameraShakeEnabled = true;
            _footstepCameraShakeMagnitude = magnitude;
            _footstepCameraShakeFrequency = frequency;
        }

        void BasicCharacter::DisableFootstepCameraShake()
        {
            _footstepCameraShakeEnabled = false;
        }

        void BasicCharacter::SetDropWeaponsOnDeath(bool drop)
        {
            _dropWeaponsOnDeath = drop;
        }

        void BasicCharacter::SetDropArmorsOnDeath(bool drop)
        {
            _dropArmorOnDeath = drop;
        }

        void BasicCharacter::SetDropTrinketsOnDeath(bool drop)
        {
            _dropTrinketsOnDeath = drop;
        }

        void BasicCharacter::SetDestroyItemsOnDeath(bool destroy)
        {
            _destroyItemsOnDeath = destroy;
        }

        void BasicCharacter::SetFallDamage(float startVelocity, float velocityMult)
        {
            _fallDmgStartVelocity = startVelocity;
            _fallDmgMult = velocityMult;
        }

        void BasicCharacter::SetBodyMaterial(MaterialType material)
        {
            _bodyMaterial = material;
        }

        void BasicCharacter::OnScaleChange(float oldScale, float newScale)
        {
            SkeletonCharacter::OnScaleChange(oldScale, newScale);
            for (auto& damageParticles : _damageParticles)
            {
                damageParticles.particles->SetScale(newScale);
            }
        }

        Damage BasicCharacter::OnPreRecieveDamage(Character* source, const Vector2f& pos, const Damage& dmg)
        {
            return dmg;
        }

        void BasicCharacter::OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg)
        {
            if (dmg.Amount > 0.0f)
            {
                for (const auto& damageParticles : _damageParticles)
                {
                    if ((dmg.Type & damageParticles.damageType) != 0)
                    {
                        Animation::SkeletonInstance* skeleton = GetSkeleton();

                        Vector2f particleSpawnPos;
                        if (skeleton->HasJoint(damageParticles.spawnJoint))
                        {
                            particleSpawnPos = skeleton->GetJointPosition(damageParticles.spawnJoint);
                        }
                        else
                        {
                            particleSpawnPos = position;
                        }

                        damageParticles.particles->SetPointSpawner(particleSpawnPos);
                        damageParticles.particles->Burst();
                    }
                }

                // Record the last dmg for generating physics upon death
                _lastDmgPos = position;
                _lastAttacker = (source != nullptr) ? source->GetID() : 0;

                // Find a sound to play
                std::vector<Item::Armor*> armors = GetArmors();
                MaterialType material = armors.empty() ? GetBodyMaterial() : armors.front()->GetMaterial();
                for (auto& damageSound : _damageSounds)
                {
                    if ((damageSound.damageType & dmg.Type) != 0 &&
                        (damageSound.materialType & material) != 0)
                    {
                        auto soundManager = GetLevel()->GetSoundManager();

                        float damageSoundVolume = dmg.Critical ? DamageCriticalSoundVolume : DamageSoundVolume;
                        soundManager->PlaySinglePositionalSound(damageSound.sounds.GetNextSound(), Audio::SoundPriority::Medium,
                                                                position, DamageHealingSoundRange.first, DamageHealingSoundRange.second,
                                                                damageSoundVolume);
                        break;
                    }
                }
            }

            uint32_t dmgTextSize = dmg.Critical ? OverheadTextFontCriticalSize : OverheadTextFontSize;
            AddOverheadText(Graphics::PreparedText(Format("%i", static_cast<int32_t>(dmg.Amount)), _damageFont, dmgTextSize, Color::White, false, false));

            // Attack the attacker if idle
            if (source != nullptr && IsAlive() && GetCurrentState() == CharacterState_Idle)
            {
                bool attackable = IsCharacterAttackable(source, false);
                if (attackable && CanMoveToAttackTarget(source))
                {
                    PushAction(CreateWeakAttackAction(source->GetID()), false);
                    Speak(Speech_Aggro, true);
                    aggroNearbyFriendlies(source);
                }
                else if (GetMoveType() != MoveType_None)
                {
                    PushAction(CreateFleeAction(source->GetPosition()), false);
                }

                if (attackable)
                {
                    Emote(Emote_Surprised, 2.0f);
                }
            }
        }

        float BasicCharacter::OnPreInteractWithCharacter(Character* target)
        {
            if (_interactAnimations.Count() > 0)
            {
                return PlayAnimationSet(_interactAnimations, false, 0.1f, 0.0f);
            }
            else
            {
                return SkeletonCharacter::OnPreInteractWithCharacter(target);
            }
        }

        float BasicCharacter::OnPreInteractWithItem(Item::Item* target)
        {
            if (_interactAnimations.Count() > 0)
            {
                return PlayAnimationSet(_interactAnimations, false, 0.1f, 0.0f);
            }
            else
            {
                return SkeletonCharacter::OnPreInteractWithItem(target);
            }
        }

        void BasicCharacter::OnPostHealed(Character* source, float ammount)
        {
            if (ammount > 0.0f)
            {
                AddOverheadText(Graphics::PreparedText(Format("%i", static_cast<int32_t>(ammount)), _healingFont, OverheadTextFontSize, 
                                                       Color::White, false, false));

                auto soundManager = GetLevel()->GetSoundManager();
                soundManager->PlaySinglePositionalSound(_healingSounds.GetNextSound(), Audio::SoundPriority::Medium,
                                                        GetPosition(), DamageHealingSoundRange.first, DamageHealingSoundRange.second,
                                                        HealingSoundVolume);

                if (_healParticles)
                {
                    Particles::FitParticlesToSkeleton(_healParticles, GetSkeleton());
                    _healParticles->Burst();
                }
            }
        }

        void BasicCharacter::OnStateChange(CharacterState newState)
        {
            _physicsTime = 0.0f;

            if (newState == CharacterState_Physics)
            {
                updateIdleAnimation();
            }
            else if (newState == CharacterState_Idle)
            {
                _idleSoundTimer = Random::RandomBetween(_idleSoundTimeout.first, _idleSoundTimeout.second);
            }
        }

        void BasicCharacter::OnEquipItem(Item::Item* item)
        {
            if (item->GetSlot() == +Item::ItemSlot::Weapon)
            {
                assignWeapons();
            }
        }

        void BasicCharacter::OnUnequipItem(Item::Item* item)
        {
            if (item->GetSlot() == +Item::ItemSlot::Weapon)
            {
                assignWeapons();
            }
        }

        const Animation::AnimationSet& BasicCharacter::getTerrainAnimation(AnimationType type, Pathfinding::EdgeType edgeType,
                                                                           Pathfinding::TerrainType terrainType, float angle,
                                                                           AnimationVariant variant) const
        {
            static const Animation::AnimationSet DefaultEmptyAnimationSet;

            // Animation type and edge type require a specific animation, can't fall back
            terrainAnimations::const_iterator animationTypeIter = _terrainAnimations.find(type);
            if (animationTypeIter == _terrainAnimations.end())
            {
                return DefaultEmptyAnimationSet;
            }

            for (const auto& edgeTypeAnimations : animationTypeIter->second)
            {
                // Match the first edge type that contains the required one
                if ((edgeTypeAnimations.first & edgeType) != 0 || edgeType == Pathfinding::EdgeType_None)
                {
                    for (const auto& terrainTypeAnimations : edgeTypeAnimations.second)
                    {
                        // Match the first terrain type that contains the required one
                        if ((terrainTypeAnimations.first._to_integral() & terrainType._to_integral()) != 0 || terrainType == +Pathfinding::TerrainType::None)
                        {
                            // Cycle until an animation is found that is at least as high as the min angle
                            for (auto edgeAngle = terrainTypeAnimations.second.rbegin(); edgeAngle != terrainTypeAnimations.second.rend(); edgeAngle++)
                            {
                                if (angle >= edgeAngle->first)
                                {
                                    // Try to find the requested variant first
                                    auto requestedVariantAnimationsIter = edgeAngle->second.find(variant);
                                    if (requestedVariantAnimationsIter != edgeAngle->second.end() &&
                                        requestedVariantAnimationsIter->second.Count() > 0)
                                    {
                                        return requestedVariantAnimationsIter->second;
                                    }

                                    // Fall back to standard animations
                                    auto standardAnimationsIter = edgeAngle->second.find(AnimationVariant_Standard);
                                    if (standardAnimationsIter != edgeAngle->second.end() &&
                                        standardAnimationsIter->second.Count() > 0)
                                    {
                                        return standardAnimationsIter->second;
                                    }

                                    return DefaultEmptyAnimationSet;
                                }
                            }
                        }
                    }
                }
            }

            return DefaultEmptyAnimationSet;
        }

        const Animation::AnimationSet& BasicCharacter::getCurrentTerrainAnimation(AnimationType type) const
        {
            return getTerrainAnimation(type, GetCurrenPathEdgeType(), GetCurrenPathTerrainType(), GetCurrentPathAngle(),
                                       GetCurrentAnimationVariant(_injuredThreshold));
        }

        void BasicCharacter::updateMoveAnimation()
        {
            std::shared_ptr<const Pathfinding::PathPosition> pathPos = GetCurrentPathPosition();
            assert(pathPos);
            Pathfinding::EdgeType curEdgeType = pathPos->GetType();

            const Animation::AnimationSet& targetAnimation = getCurrentTerrainAnimation(AnimationType_Move);
            if (!IsPlayingAnimation(targetAnimation) && targetAnimation.Count() > 0)
            {
                float transitionTime = _curPathEdgeType != curEdgeType ? 0.25f : 0.1f;
                const std::string& animName = targetAnimation.PeekNextAnimation();
                float animLength = GetAnimationLength(animName);
                PlayAnimationSet(targetAnimation, true, transitionTime, Random::RandomBetween(0.0f, animLength));
            }

            _curPathEdgeType = curEdgeType;
        }

        void BasicCharacter::updateIdleAnimation()
        {
            const Animation::AnimationSet& targetAnimation = getCurrentTerrainAnimation(AnimationType_Idle);
            if (!IsPlayingAnimation(targetAnimation) && targetAnimation.Count() > 0)
            {
                float transitionTime = 0.1f;
                const std::string& animName = targetAnimation.PeekNextAnimation();
                float animLength = GetAnimationLength(animName);
                PlayAnimationSet(targetAnimation, true, transitionTime, Random::RandomBetween(0.0f, animLength));
            }
        }

        void BasicCharacter::lookAt(const Vector2f& position, bool forced)
        {
            if (_headName.size() > 0)
            {
                if (forced || !_hasForcedLookPos)
                {
                    GetSkeleton()->PointJointAt(_headName, position, _headRotationSpeed);
                    _hasForcedLookPos = forced;
                }
            }
        }

        void BasicCharacter::lookTo(const Vector2f& direction, bool forced)
        {
            if (_headName.size() > 0)
            {
                if (forced || !_hasForcedLookPos)
                {
                    GetSkeleton()->PointJointTo(_headName, direction, _headRotationSpeed);
                    _hasForcedLookPos = forced;
                }
            }
        }

        void BasicCharacter::stopLooking(bool forced)
        {
            if (_headName.size() > 0)
            {
                if (forced == _hasForcedLookPos)
                {
                    GetSkeleton()->StopPointingJoint(_headName, _headRotationSpeed);
                    _hasForcedLookPos = false;
                }
            }
        }

        void BasicCharacter::assignWeapons()
        {
            // Clear attachments
            for (auto& weapAttachment : _weaponAttachmentLocations)
            {
                weapAttachment.second.equippedWeaponType = Item::WeaponType_None;
                weapAttachment.second.equippedWeapon = 0;
            }

            // Grab weapons
            std::vector<Item::Weapon*> weapons = GetWeapons();

            // Sort weapons by the number of viable locations they can fit, this makes sure that shields
            // and other items that only fit in one place get the first choice.
            std::sort(weapons.begin(), weapons.end(), [this](const Item::Weapon* a, const Item::Weapon* b)
            {
                return _weaponAttachmentTypes[a->GetWeaponType()].size() < _weaponAttachmentTypes[b->GetWeaponType()].size();
            });

            // Assign weapons
            for (Item::Weapon* weap : weapons)
            {
                Item::WeaponType type = weap->GetWeaponType();
                const auto& attachmentLocations = _weaponAttachmentTypes[type];
                for (const auto& location : attachmentLocations)
                {
                    auto& attachmentLocation = _weaponAttachmentLocations[location];
                    if (attachmentLocation.equippedWeaponType == Item::WeaponType_None)
                    {
                        weap->SetAttachPoint(attachmentLocation.jointNames[type]);
                        attachmentLocation.equippedWeaponType = type;
                        attachmentLocation.equippedWeapon = weap->GetID();
                        break;
                    }
                }
            }
        }

        Vector2f BasicCharacter::GetLineOfSightPoint() const
        {
            if (_headName.empty())
            {
                return SkeletonCharacter::GetLineOfSightPoint();
            }
            else
            {
                return GetSkeleton()->GetJointPosition(_headName);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::BasicCharacter>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);

        preloads.insert("Fonts/hud_font.ttf");
        preloads.insert("HUD/emotes.hudmatset");

        preloads.insert(Character::DamageFontPath);
        preloads.insert(Character::HealingFontPath);
    }
}
