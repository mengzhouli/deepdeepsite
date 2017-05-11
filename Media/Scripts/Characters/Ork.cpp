#include "Characters/Ork.hpp"

#include "Physics/CircleCollision.hpp"

#include "Characters/Bomb.hpp"
#include "Items/Weapons/BasicRangedWeapon.hpp"

#include "Abilities/GroundSlam.hpp"

#include "Items/Weapons/OrkWeapons.hpp"

#include "ContentUtility.hpp"

#include "DamageTypes.hpp"
#include "Characters/FootstepSounds.hpp"

#include "Items/Weapons/ThrowingWeapon.hpp"

#include "Characters/Gobbo.hpp"
#include "Items/Weapons/GobboWeapons.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const Audio::SoundPathVector OrkAggroSounds =
        {
            { "Audio/vocalizations/Ork/Ork_Agro1_Uaghaaeuu.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Agro2_Waugheu.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Agro3_Oaughoeu.ogg", 1.0f },
        };

        static const Audio::SoundPathVector OrkAttackSounds =
        {
            { "Audio/vocalizations/Ork/Ork_Attack1_Woof.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Attack2_Heave.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Attack2_Heave_1.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Attack3_Blouagh.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Attack4_Uouagh.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Attack5_Wluagph.ogg", 1.0f },
        };

        static const Audio::SoundPathVector OrkDeathSounds =
        {
            { "Audio/vocalizations/Ork/Ork_Death1.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Death2_Gurgling.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Death3_Long.ogg", 1.0f },
        };

        static const Audio::SoundPathVector OrkIdleSounds =
        {
            { "Audio/vocalizations/Ork/Ork_idle1.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_idle2.ogg", 1.0f },
        };

        static const Audio::SoundPathVector OrkTauntSounds =
        {
            { "Audio/vocalizations/Ork/Ork_Taunt1_HeuHuaeeuu.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Taunt2_RegularOldRoar.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Taunt3_OohOohAhhhh.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Taunt4_HotDick.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Taunt5_UaghaOoOoOo.ogg", 1.0f },
            { "Audio/vocalizations/Ork/Ork_Taunt6_Uaghaaaa.ogg", 1.0f },
        };

        static const Audio::SoundPathVector OrkLeapSounds =
        {
        };

        static const std::map<Item::WeaponType, Audio::SoundPathVector> OrkAttackWooshSounds =
        {
            {
                Item::WeaponType_Melee_1H,
                {
                    { "Audio/Items/Weapons/Ork_Sword_Swipe.wav", 1.0f },
                }
            },
            {
                Item::WeaponType_Melee_Shield,
                {
                    { "Audio/Items/Weapons/Ork_Sword_Swipe.wav", 1.0f },
                }
            },
        };

        static const float OrkSpeechPositionalVolume = 1.0f;
        static const std::pair<float, float> OrkSpeechRange = std::make_pair(1500.0f, 3500.0f);

        static const float OrkFootstepShakeMagnitude = 0.75f;
        static const float OrkFootstepShakeFrequency = 7.0f;

        static const std::string OrkWarriorSkeletonPath = "Skeletons/Characters/Ork/orkwar/orkwar.skel";
        static const std::string OrkWarriorMatsetPath = "Skeletons/Characters/Ork/orkwar/orkwar0.polymatset";

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> OrkWarriorInterchangeMatsetPaths =
        {
            {
                "body",
                {
                    { "Skeletons/Characters/Ork/orkwar/orkwar_chest0.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_chest1.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_chest2.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_chest3.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_chest4.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_chest5.polymatset", 1.0f },
                }
            },
            {
                "arm",
                {
                    { "Skeletons/Characters/Ork/orkwar/orkwar_arm0.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_arm1.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_arm2.polymatset", 1.0f },
                }
            },
            {
                "head",
                {
                    { "Skeletons/Characters/Ork/orkwar/orkwar_head0.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_head1.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_head2.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_head3.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_head4.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_head5.polymatset", 1.0f },
                }
            },
            {
                "jaw",
                {
                    { "Skeletons/Characters/Ork/orkwar/orkwar_jaw0.polymatset", 1.0f },
                    { "Skeletons/Characters/Ork/orkwar/orkwar_jaw1.polymatset", 1.0f },
                }
            },
        };

        static const Audio::SoundPathVector OrkLeapLandAlwaysSounds =
        {
            { "Audio/Footsteps/step_Land_OrcAlways_1.wav", 1.0f },
            { "Audio/Footsteps/step_Land_OrcAlways_2.wav", 1.0f },
        };

        static const std::string OrkDamageParticlePath = "Particles/blood_splat.partsys";
        static const std::string OrkRunParticlesPath = "Particles/dust_02.partsys";

        Ork::Ork(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicCharacter(parameters, skeletonPath, matsetPath)
        {
            DisablePhysics();

            SetMoveType(MoveType_Walk);
            SetPathableEdges(Pathfinding::EdgeType_Walk);

            SetRunParticleSystem(OrkRunParticlesPath);

            SetAggroRange(1000.0f);

            AddDamagedParticleSystem(DamageType_Element_Physical, OrkDamageParticlePath);

            SetBodyMaterial(MaterialType_Type_Flesh);
            AddDamageSounds(Audio::GetStandardOnFleshDamageSoundPaths());

            SetPositionalSpeechParameters(OrkSpeechPositionalVolume, OrkSpeechRange.first, OrkSpeechRange.second);

            AddAttackWooshSounds(OrkAttackWooshSounds);
            AddAttackSounds(OrkAttackSounds);
            SetAttackSoundPlayChance(1.0f);

            AddDeathSounds(OrkDeathSounds);

            AddIdleSounds(OrkIdleSounds);
            SetIdleSoundInterval(5.0f, 15.0f);

            AddAggroSounds(OrkAggroSounds);

            AddBaseFootstepSounds(Audio::GetHeavyBasicFootstepSoundPaths());
            for (const auto& terrainFootsteps : Audio::GetStandardTerrainFootstepSoundPaths())
            {
                AddTerrainFootstepSounds(terrainFootsteps.first, terrainFootsteps.second);
            }
            EnableFootstepCameraShake(OrkFootstepShakeMagnitude, OrkFootstepShakeFrequency);
        }

        void Ork::OnDeath()
        {
            EnablePhysics(false);
            BasicCharacter::OnDeath();
        }

        static const std::pair<float, float> SlamInterval = { 7.5f, 10.0f };

        OrkWarrior::OrkWarrior(const CharacterParameters& parameters)
            : Ork(parameters, OrkWarriorSkeletonPath, OrkWarriorMatsetPath)
            , _leapAnimations()
            , _slamTimer(Random::RandomBetween(SlamInterval.first, SlamInterval.second))
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            float size = Random::RandomBetween(0.9f, 1.1f);

            SetMoveSpeed(200.0f);
            SetMaxHealth(120.0f * size);
            SetHealthRegeneration(0.0f);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(size);
            SetSkeletonColor(Color::White);

            SetHeadName("head");

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle0", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run0", 1.0f);

            _leapAnimations.AddAnimation("fall0", 1.0f);
            _groundSlamAnimations.AddAnimation("jump0", 1.0f);
            _leapSounds.AddSounds(OrkLeapSounds);
            for (const auto& leapTerrainSounds : Audio::GetStandardTerrainLeapFootstepSoundPaths())
            {
                _leapTerrainSounds[leapTerrainSounds.first] = Audio::SoundSet(leapTerrainSounds.second);
            }
            for (const auto& leapLandTerrainSounds : Audio::GetStandardTerrainLandFootstepSoundPaths())
            {
                _leapLandTerrainSounds[leapLandTerrainSounds.first] = Audio::SoundSet(leapLandTerrainSounds.second);
            }
            _leapLandAlwaysSounds.AddSounds(OrkLeapLandAlwaysSounds);

            AddWeaponAttachment("handa", "weapon_a_0", "weapon_a_1", { { Item::WeaponType_Melee_1H, "handa" }, });
            AddWeaponAttachment("handb", "weapon_b_0", "weapon_b_1", { { Item::WeaponType_Melee_1H, "handb" }, });

            AddAttackAnimation("attack0", 1.0f, { { "handb", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, { "woosh0", } } } });
            AddAttackAnimation("attack1", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, { "woosh0", } } } });
            AddAttackAnimation("attack2", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, { "woosh0", } } },
                                                  { "handb", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, {} } } });

            AddFootJoint("foota_front", "foota_back");
            AddFootJoint("footb_front", "footb_back");
            AddRunParticleTag("dust_0");
            AddRunParticleTag("dust_1");

            for (auto interchangeMat : OrkWarriorInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }

            SetJawJoint("jaw");
        }

        float OrkWarrior::OnPreLeap(Pathfinding::TerrainType terrainType)
        {
            // No wind up animation
            return 0.0f;
        }

        void OrkWarrior::OnLeap(Pathfinding::TerrainType terrainType, float airTime, float maxAirTime)
        {
            Speak(_leapSounds, false, true);

            auto footSoundIter = _leapTerrainSounds.find(terrainType);
            if (footSoundIter != _leapTerrainSounds.end())
            {
                auto soundManager = GetLevel()->GetSoundManager();
                soundManager->PlaySinglePositionalSound(footSoundIter->second.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(), 
                                                        OrkSpeechRange.first, OrkSpeechRange.second, 1.0f);
            }

            PlayAnimationSet(_leapAnimations, true, 0.1f, 0.0f);
        }

        float OrkWarrior::OnPostLeap(Pathfinding::TerrainType terrainType)
        {
            auto soundManager = GetLevel()->GetSoundManager();

            auto footSoundIter = _leapLandTerrainSounds.find(terrainType);
            if (footSoundIter != _leapLandTerrainSounds.end())
            {
                soundManager->PlaySinglePositionalSound(footSoundIter->second.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                                                        OrkSpeechRange.first, OrkSpeechRange.second, 1.0f);
            }
            soundManager->PlaySinglePositionalSound(_leapLandAlwaysSounds.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                                                    OrkSpeechRange.first, OrkSpeechRange.second, 1.0f);

            return 0.0f;
        }

        GroundSlammer::Info OrkWarrior::OnGroundSlam(Pathfinding::TerrainType terrainType)
        {
            Speak(_leapSounds, false, true);

            auto footSoundIter = _leapTerrainSounds.find(terrainType);
            if (footSoundIter != _leapTerrainSounds.end())
            {
                auto soundManager = GetLevel()->GetSoundManager();
                soundManager->PlaySinglePositionalSound(footSoundIter->second.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                    OrkSpeechRange.first, OrkSpeechRange.second, 1.0f);
            }

            std::string anim = _groundSlamAnimations.PeekNextAnimation();
            PlayAnimationSet(_groundSlamAnimations, false, 0.1f, 0.0f);

            const auto& skeleton = GetSkeleton();

            GroundSlammer::Info info;
            info.TotalDuration = skeleton->GetAnimationLength(anim);
            info.HitTime = skeleton->GetAnimationTagTime(anim, "hit");

            return info;
        }

        void OrkWarrior::OnGroundSlamLand(Pathfinding::TerrainType terrainType)
        {
            OnPostLeap(terrainType);
        }

        void OrkWarrior::OnUpdate(double totalTime, float dt)
        {
            Ork::OnUpdate(totalTime, dt);

            if (GetAttackTarget() != nullptr)
            {
                Ability::GroundSlam* slamAbility = GetAbility<Ability::GroundSlam>();
                if (slamAbility)
                {
                    _slamTimer -= dt;
                    if (_slamTimer <= 0.0f)
                    {
                        PushAction(slamAbility->GenerateAction(), false);
                        _slamTimer += Random::RandomBetween(SlamInterval.first, SlamInterval.second);
                    }
                }
            }
        }


        void OrkWarrior::OnLoadContent(Content::ContentManager* contentManager)
        {
            Ork::OnLoadContent(contentManager);

            _leapSounds.LoadContent(contentManager);
            for (auto& leapTerrainType : _leapTerrainSounds)
            {
                leapTerrainType.second.LoadContent(contentManager);
            }
            for (auto& leapTerrainType : _leapLandTerrainSounds)
            {
                leapTerrainType.second.LoadContent(contentManager);
            }
            _leapLandAlwaysSounds.LoadContent(contentManager);
        }

        void OrkWarrior::OnUnloadContent()
        {
            Ork::OnUnloadContent();

            _leapSounds.UnloadContent();
            for (auto& leapTerrainType : _leapTerrainSounds)
            {
                leapTerrainType.second.UnloadContent();
            }
            for (auto& leapTerrainType : _leapLandTerrainSounds)
            {
                leapTerrainType.second.UnloadContent();
            }
            _leapLandAlwaysSounds.UnloadContent();
        }

        static const std::string ShieldOrkSkeletonPath = "Skeletons/Characters/Ork/orkshield/orkshield.skel";
        static const std::string ShieldOrkMatsetPath = "Skeletons/Characters/Ork/orkshield/orkshield.polymatset";

        static const std::string ShieldOrkRubbleSkeletonPath = "Skeletons/Characters/Orkpile/orkpile.skel";
        static const std::string ShieldOrkRubbleMatsetPath = "Skeletons/Characters/Orkpile/orkpile.polymatset";

        static const float ShieldOrkUnhideRange = 1000.0f;
        static const float ShieldOrkAggroRadius = 1000.0f;

        static const std::vector<std::string> ShieldOrkRubblePieces =
        {
            "junk_1",
            "junk_2",
            "junk_3",
            "junk_4",
            "junk_5",
            "junk_6",
            "junk_7",
            "junk_8",
            "junk_9",
        };

        static const Audio::SoundPathVector ShieldOrkCollisionSounds =
        {
            { "Audio/Characters/Debris/Debris_MiscWood_Tower_1.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_MiscWood_Tower_2.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_MiscWood_Tower_3.wav", 1.0f },
        };

        ShieldOrk::ShieldOrk(const CharacterParameters& parameters)
            : Ork(parameters, ShieldOrkSkeletonPath, ShieldOrkMatsetPath)
            , _shieldState()
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            SetMaxHealth(600.0f);
            SetHealthRegeneration(0.0f);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);

            SetHeadName("head");
            SetJawJoint("jaw");

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_0", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_1", 0.5f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "walk_0", 1.0f);

            AddWeaponAttachment("handa", "weapon_a_0", "weapon_a_1", { { Item::WeaponType_Melee_1H, "handa" }, });
            AddWeaponAttachment("handb", "weapon_b_0", "weapon_b_1", { { Item::WeaponType_Melee_Shield, "handb" }, });

            AddFootJoint("foota_front", "foota_back");
            AddFootJoint("footb_front", "footb_back");
            AddRunParticleTag("particle_1");
            AddRunParticleTag("particle_0");

            SetAggroRange(1000.0f);

            _shieldUpAnimations.AddAnimation("shieldup_0", 1.0f);
            _shieldDownAnimations.AddAnimation("shielddown", 1.0f);
            _aggroAnimations.AddAnimation("aggro", 1.0f);

            SetMaterialCollisionSound(ShieldOrkCollisionSounds);
        }

        bool ShieldOrk::HasLineOfSight(const Vector2f& target) const
        {
            // Can see/attack through walls
            return true;
        }

        void ShieldOrk::StandUp()
        {
            if (_shieldState == Hiding)
            {
                PushAction(CreateSpecialAction(ShieldDown), false);
            }
        }

        void ShieldOrk::OnSpawn()
        {
            Ork::OnSpawn();

            GiveItem(Item::BindItemConstructor<Item::OrkLance>());

            Item::OrkTowerShield* shield = GiveItem(Item::BindItemConstructor<Item::OrkTowerShield>());
            uint32_t shieldBreakCount = shield->GetBreakablePieceCount();
            if (shieldBreakCount > 0)
            {
                for (uint32_t i = 0; i < shieldBreakCount; i++)
                {
                    _shieldBreakThresholds.push_back(float(i + 1) / float(shieldBreakCount + 1));
                }
            }

            setShieldState(Hiding);
        }

        void ShieldOrk::OnDeath()
        {
            EnablePhysics(false);
            Ork::OnDeath();
        }

        void ShieldOrk::OnActionChange(const Action& newAction)
        {
            if (newAction.Type == Action::ActionType_Special)
            {
                setShieldState(static_cast<ShieldState>(newAction.SpecialID));
            }
        }

        void ShieldOrk::OnLoadContent(Content::ContentManager* contentManager)
        {
            Ork::OnLoadContent(contentManager);

            _rubble = Content::CreateSkeletonInstance(contentManager, ShieldOrkRubbleSkeletonPath, ShieldOrkRubbleMatsetPath);
            _rubbleCollision = new Physics::SkeletonCollision(GetLevelLayer()->GetPhysicsWorld(), _rubble, 0.0f);

            GetSkeleton()->AddChildDrawable("rubble_attach", _rubble);
        }

        void ShieldOrk::OnUnloadContent()
        {
            GetSkeleton()->RemoveChildDrawable("rubble_attach", _rubble);

            SafeRelease(_rubble);
            SafeRelease(_rubbleCollision);

            Ork::OnUnloadContent();
        }

        void ShieldOrk::OnUpdate(double totalTime, float dt)
        {
            Ork::OnUpdate(totalTime, dt);

            if (_transitionTimer >= 0.0f)
            {
                _transitionTimer -= dt;
                if (_transitionTimer < 0.0f)
                {
                    onPostShieldSwitch(_shieldState);
                }

                const Animation::SkeletonInstance* skeleton = GetSkeleton();
                if (skeleton->IsPlayingAnimationFromSet(_aggroAnimations))
                {
                    for (const auto& piece : ShieldOrkRubblePieces)
                    {
                        if (skeleton->HasAnimationTagJustPassed(piece))
                        {
                            InsertBrokenMaterial(BreakSkeletonMaterial(_rubble, GetLevelLayer()->GetPhysicsWorld(), piece));
                        }
                    }
                }
            }
            else
            {
                if (_shieldState == Hiding)
                {
                    if (FindNearbyAttackableTarget(ShieldOrkUnhideRange) != nullptr)
                    {
                        PushAction(CreateSpecialAction(ShieldDown), false);
                    }

                    const Animation::SkeletonInstance* skeleton = GetSkeleton();
                    _rubble->SetPosition(skeleton->GetPosition());
                    _rubble->SetScale(skeleton->GetScale());
                    _rubble->SetInvertedX(skeleton->IsInvertedX());
                    _rubble->SetInvertedY(skeleton->IsInvertedY());
                }
                else
                {
                    Character* target = GetAttackTarget();
                    if (target != nullptr)
                    {
                        const Rectanglef& targetBounds = target->GetBounds();
                        if (targetBounds.Middle().Y < GetHeadPosition().Y)
                        {
                            if (_shieldState != ShieldUp)
                            {
                                PushAction(CreateSpecialAction(ShieldUp), false);
                            }
                        }
                        else
                        {
                            if (_shieldState != ShieldDown)
                            {
                                PushAction(CreateSpecialAction(ShieldDown), false);
                            }
                        }

                        const Animation::SkeletonInstance* skeleton = GetSkeleton();
                        if (skeleton->IsBetweenAnimationTags("knockback_start", "knockback_end"))
                        {
                            const Item::OrkTowerShield* shield = GetWeapons<Item::OrkTowerShield>().front();
                            Level::LevelLayerInstance* layer = GetLevelLayer();

                            Vector2f knockbarDir = Vector2f::Normalize(skeleton->GetJointPosition("knockback_dir") - skeleton->GetJointPosition("handb"));

                            std::vector<Vector2f> outPts;
                            std::vector<Character*> hitTargets = layer->FindIntersections<Character>(shield->GetCollision(), outPts,
                                [this](const Character* character) { return (character->GetCollision() != nullptr) && IsCharacterAttackable(character, false); });
                            for (auto hitTarget : hitTargets)
                            {
                                Physics::Collision* collision = hitTarget->GetCollision();
                                float force = collision->GetMass() * 100.0f;
                                hitTarget->ApplyLinearImpulse(knockbarDir * force);
                            }
                        }
                    }
                }
            }

            _rubbleCollision->Update(totalTime, dt);

            _rubble->SetColor(GetColor());
            _rubble->Update(totalTime, dt);
        }

        void ShieldOrk::OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg)
        {
            float healthPerc = GetHealth().GetPercent();
            if (!_shieldBreakThresholds.empty() && healthPerc <= _shieldBreakThresholds.back())
            {
                _shieldBreakThresholds.pop_back();

                Item::OrkTowerShield* shield = GetWeapons<Item::OrkTowerShield>().front();
                shield->BreakNextPiece();
            }

            if (_shieldState == Hiding)
            {
                PushAction(CreateSpecialAction(ShieldDown), false);
            }

            Ork::OnPostRecieveDamage(source, position, dmg);
        }

        void ShieldOrk::setShieldState(ShieldState state)
        {
            ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_Walk);
            ClearTerrainAnimations(AnimationType_Move, Pathfinding::EdgeType_Walk);
            ClearAttackAnimations();

            switch (state)
            {
            case Hiding:
                SetAggroRange(0.0f);
                SetInvulnerable(true);
                SetIncapacitated(true);
                _transitionTimer = 0.0f;
                break;

            case ShieldDown:
                if (_shieldState == Hiding)
                {
                    _transitionTimer = PlayAnimationSet(_aggroAnimations, false, 0.1f, 0.0f);
                }
                else
                {
                    _transitionTimer = PlayAnimationSet(_shieldDownAnimations, false, 0.1f, 0.0f);
                }
                break;

            case ShieldUp:
                if (_shieldState == Hiding)
                {
                    _transitionTimer = PlayAnimationSet(_aggroAnimations, false, 0.1f, 0.0f);
                }
                else
                {
                    _transitionTimer = PlayAnimationSet(_shieldUpAnimations, false, 0.1f, 0.0f);
                }
                break;

            default:
                assert(false);
                break;
            }

            SetMoveSpeed(0.0f);
            _shieldState = state;
        }

        void ShieldOrk::onPostShieldSwitch(ShieldState state)
        {
            FinishSpecialActions();
            SetInvulnerable(false);
            SetIncapacitated(false);

            switch (state)
            {
            case Hiding:
                SetMoveSpeed(0.0f);
                SetAggroRange(ShieldOrkUnhideRange);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_waiting", 1.0f);
                break;

            case ShieldDown:
                SetMoveSpeed(200.0f);
                SetAggroRange(ShieldOrkAggroRadius);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_0", 1.0f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_1", 0.5f);
                AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "walk_0", 1.0f);

                AddAttackAnimation("attack_1", 1.0f, { { "handa",{ Item::WeaponType_Melee_1H,{ { "dmg_start", "dmg_end" } },{},{},{ "woosh0", } } } });
                AddAttackAnimation("attack_2", 1.0f, { { "handa",{ Item::WeaponType_Melee_1H,{ { "dmg_start0", "dmg_end0" },{ "dmg_start1", "dmg_end1" },{ "dmg_start2", "dmg_end2" },{ "dmg_start3", "dmg_end3" }, },{ "dmg_reset0", "dmg_reset1", "dmg_reset2", },{},{ "woosh0", "woosh1", "woosh2", "woosh3", } } } });
                AddAttackAnimation("attack_0", 1.0f, { { "handb",{ Item::WeaponType_Melee_Shield,{ { "dmg_start", "dmg_end" } },{},{},{ "woosh0", } } } });
                AddAttackAnimation("attack_3", 1.0f, { { "handb",{ Item::WeaponType_Melee_Shield,{ { "dmg_start", "dmg_end" } },{},{},{ "woosh0", } } } });
                break;

            case ShieldUp:
                SetMoveSpeed(200.0f);
                SetAggroRange(ShieldOrkAggroRadius);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "shieldup_idle", 1.0f);
                AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "shieldup_walk", 1.0f);
                AddAttackAnimation("shieldup_poke", 1.0f, { { "handa",{ Item::WeaponType_Melee_1H,{ { "dmg_start", "dmg_end" } },{},{ "woosh0", } } } });
                AddAttackAnimation("shieldup_smash", 1.0f, { { "handb",{ Item::WeaponType_Melee_Shield,{ { "dmg_start", "dmg_end" } },{},{ "woosh0", } } } });
                break;

            default:
                assert(false);
                break;
            }
        }
    }

    namespace Character
    {
        static const std::string OrkCannonBallSkeletonPath = "Skeletons/Items/Weapons/OrkWeapons/cannon_ball.skel";
        static const std::string OrkCannonBallMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/cannon_ball.polymatset";

        class OrkCannonBall : public Bomb
        {
        public:
            OrkCannonBall(const CharacterParameters& parameters, CharacterID owner, CharacterID target, float duration, float radius, float dmg)
                : Bomb(parameters, OrkCannonBallSkeletonPath, OrkCannonBallMatsetPath, "", "origin", owner, duration, radius, dmg)
            {
                SetHitsEnemiesOnly(true, { target });
            }
        };
    }

    template <>
    void EnumeratePreloads<Character::OrkCannonBall>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Bomb>(preloads);

        preloads.insert(Character::OrkCannonBallSkeletonPath);
        preloads.insert(Character::OrkCannonBallMatsetPath);
    }

    namespace Item
    {
        static const std::string OrkCannonSkeletonPath = "Skeletons/Items/Weapons/OrkWeapons/cannon.skel";
        static const std::string OrkCannonMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/cannon.polymatset";

        static const std::string OrkCannonFireParticlesPath = "Particles/explosion_2.partsys";

        static const float OrkCannonDPS = 6.0f;
        static const float OrkCannonRange = 3000.0f;
        static const float OrkCannonExplosionRadius = 300.0f;

        static const Audio::SoundPathVector OrkCannonFireSoundPaths =
        {
            { "Audio/Items/Weapons/Ork_Cannon.wav", 1.0f },
        };

        static const std::pair<float, float> OrkCannonShakeRadius = std::make_pair(1000.0f, 3000.0f);
        static const float OrkCannonShakeMagnitude = 5.0f;
        static const float OrkCannonShakeFrequency = 10.0f;
        static const float OrkCannonShakeDuration = 0.5f;

        class OrkCannon : public BasicRangedWeapon
        {
        public:
            OrkCannon(const ItemParameters& parameters)
                : BasicRangedWeapon(parameters, "", WeaponType_Ranged_Gun_2H, OrkCannonSkeletonPath, OrkCannonMatsetPath)
                , _lightBrightness(0.0f)
                , _light(Vector2f::Zero, 700.0f, Color::White, 0.0f)
                , _fireParticles(nullptr)
            {
                SetRange(OrkCannonRange);
                SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Element_Physical, OrkCannonDPS));
                AddFireSounds(OrkCannonFireSoundPaths);
            }

        protected:
            virtual void Fire(const Character::Damage& dmg) override
            {
                BasicRangedWeapon::Fire(dmg);

                Character::Character *owner = GetOwner();
                assert(owner);

                Character::Character *target = GetCurrentTarget();
                assert(target);

                const Rayf& cannonOpening = owner->GetWeaponAttachPoint(GetAttachPoint(), GetWeaponType());

                const Vector2f dir = ComputeTargetAttackPoint(target) - cannonOpening.Position;
                const float rangePerc = Clamp(dir.Length() / GetRange(), 0.0f, 1.0f);

                // Adjust flight time depending on range perc
                const float t = Lerp(0.6f, 1.8f, rangePerc);

                const float fuseLength = t * Random::RandomBetween(1.0f, 1.8f);

                Character::CharacterConstructor<Character::OrkCannonBall> cannonballConstructor =
                    Character::BindCharacterConstructor<Character::OrkCannonBall>(owner->GetID(), target->GetID(), fuseLength, OrkCannonExplosionRadius, dmg.Amount);

                Character::OrkCannonBall* cannonball = GetLevelLayer()->SpawnCharacter(cannonOpening.Position, "cannonball", nullptr, cannonballConstructor);

                const Physics::Collision *cannonballCollision = cannonball->GetCollision();
                const float m = cannonballCollision->GetMass();
                const float g = cannonballCollision->GetPhysicsWorld()->GetGravity().Y;

                const Vector2f vi(dir.X / t, ((2.0f * dir.Y) - (g * t * t)) / (2.0f * t));

                const Vector2f impulse(vi * m * Random::RandomBetween(0.7f, 1.1f));

                cannonball->ApplyLinearImpulse(impulse);

                _fireParticles->SetPointSpawner(cannonOpening.Position);
                _fireParticles->SetRotation(cannonOpening.Direction);
                _fireParticles->Burst();

                _light.Position = cannonOpening.Position;
                _lightBrightness = 1.0f;

                auto& cameraController = GetLevel()->GetCameraController();
                cameraController.Shake(cannonOpening.Position, OrkCannonShakeRadius.first, OrkCannonShakeRadius.second,
                                       OrkCannonShakeMagnitude, OrkCannonShakeFrequency, OrkCannonShakeDuration);
            }

            virtual void OnLoadContent(Content::ContentManager* contentManager) override
            {
                BasicRangedWeapon::OnLoadContent(contentManager);

                _fireParticles = Content::CreateParticleSystemInstance(contentManager, OrkCannonFireParticlesPath);
                _fireParticles->SetScale(5.0f);
            }

            virtual void OnUnloadContent() override
            {
                BasicRangedWeapon::OnUnloadContent();

                SafeRelease(_fireParticles);
            }

            virtual void OnUpdate(double totalTime, float dt) override
            {
                BasicRangedWeapon::OnUpdate(totalTime, dt);

                const float lightDuration = 0.4f;
                _lightBrightness = Clamp(_lightBrightness - (dt / lightDuration), 0.0f, 1.0f);
                _light.LightColor = Color::White * _lightBrightness;

                _fireParticles->Update(totalTime, dt);
            }

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override
            {
                BasicRangedWeapon::OnDraw(levelRenderer);

                if (_lightBrightness > 0.0f)
                {
                    levelRenderer->AddLight(_light);
                }

                levelRenderer->AddDrawable(_fireParticles, false);
            }

        private:
            float _lightBrightness;
            Lights::PointLight _light;

            Particles::ParticleSystemInstance* _fireParticles;
        };
    }

    template <>
    void EnumeratePreloads<Item::OrkCannon>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicRangedWeapon>(preloads);

        preloads.insert(Item::OrkCannonSkeletonPath);
        preloads.insert(Item::OrkCannonMatsetPath);
        preloads.insert(Item::OrkCannonFireParticlesPath);

        EnumeratePreloads<Character::OrkCannonBall>(preloads);

        EnumerateSoundVectorPreloads(preloads, Item::OrkCannonFireSoundPaths);
    }

    namespace Character
    {
        static const std::string OrkCannoneerSkeletonPath = "Skeletons/Characters/Ork/orkcan/orkcan.skel";
        static const std::string OrkCannoneerMatsetPath = "Skeletons/Characters/Ork/orkcan/orkcan.polymatset";

        OrkCannoneer::OrkCannoneer(const CharacterParameters& parameters)
            : Ork(parameters, OrkCannoneerSkeletonPath, OrkCannoneerMatsetPath)
            , _cannonballAttached(false)
            , _cannonball(nullptr)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            float size = Random::RandomBetween(1.0f, 1.2f);

            SetMoveSpeed(200.0f);
            SetMaxHealth(500.0f);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(size);
            SetSkeletonColor(Color::White);

            SetHeadName("head");
            SetJawJoint("jaw");

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle0", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle1", 0.1f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run0", 1.0f);

            //AddWeaponAttachment("handa", "weapon_a_0", "weapon_a_1", { });
            //AddWeaponAttachment("handb", "weapon_b_0", "weapon_b_1", { });

            AddWeaponAttachment("muzzle", "muzzle_pos", "muzzle_dir", { { Item::WeaponType_Ranged_Gun_2H, "muzzle_pos" } });

            AddAttackAnimation("attack0", 1.0f, { { "muzzle", { Item::WeaponType_Ranged_Gun_2H, { { "fire", "fire" } }, {}, {} } } }, { 1000.0f, std::numeric_limits<float>::max() });
            AddAttackAnimation("attack1", 1.0f, { { "muzzle", { Item::WeaponType_Ranged_Gun_2H,{ { "fire", "fire" } },{},{} } } }, { 0.0f, 1000.0f });
            AddAttackAnimation("attack2", 1.0f, { { "muzzle", { Item::WeaponType_Ranged_Gun_2H,{ { "fire", "fire" } },{},{} } } }, { 0.0f, 1000.0f });

            AddFootJoint("foota_front", "foota_back");
            AddFootJoint("footb_front", "footb_back");
            AddRunParticleTag("dust_0");
            AddRunParticleTag("dust_1");

            SetAggroRange(2000.0f);

            SetAttackSoundPlayChance(0.0f);
        }

        void OrkCannoneer::OnSpawn()
        {
            GiveItem(Item::BindItemConstructor<Item::OrkCannon>());
            Ork::OnSpawn();
        }

        void OrkCannoneer::OnLoadContent(Content::ContentManager* contentManager)
        {
            Ork::OnLoadContent(contentManager);

            _cannonball = Content::CreateSkeletonInstance(contentManager, OrkCannonBallSkeletonPath, OrkCannonBallMatsetPath);
        }

        void OrkCannoneer::OnUnloadContent()
        {
            Ork::OnUnloadContent();

            SafeRelease(_cannonball);
        }

        void OrkCannoneer::OnUpdate(double totalTime, float dt)
        {
            Ork::OnUpdate(totalTime, dt);

            const std::string cannonballHoldJoint = "cannonball_hold";

            if (IsAttacking() && HasAnimationTagPassed("add_ammo") && !HasAnimationTagPassed("remove_ammo"))
            {
                if (!_cannonballAttached)
                {
                    AttachDrawable(cannonballHoldJoint, _cannonball);
                    _cannonballAttached = true;
                }
                _cannonball->SetPosition(GetAttachPoint(cannonballHoldJoint).Position);
            }
            else
            {
                if (_cannonballAttached)
                {
                    DetachDrawable(cannonballHoldJoint, _cannonball);
                    _cannonballAttached = false;
                }
            }
        }
    }

    namespace Item
    {
        static const float OrkThrowingGobboDPS = 15.0f;
        static const float OrkThrowingGobboRange = 2500.0f;

        static const Rotatorf OrkThrowingGobboMinSpin = TwoPi * 20.0f;
        static const Rotatorf OrkThrowingGobboMaxSpin = (TwoPi + Pi) * 20.0f;

        static const std::string OrkThrowingGobboSkeletonPath = "Skeletons/Characters/Gobbo/gobbo.skel";
        static const std::vector<std::string> OrkThrowingGobboMatsetPaths =
        {
            "Skeletons/Characters/Gobbo/gobbo.polymatset",
        };

        static const Audio::SoundPathVector OrkThrowingGobboBounceSounds =
        {
            { "Audio/Characters/Damage/damage_crush_flesh1.wav", 1.0f },
            { "Audio/Characters/Damage/damage_crush_flesh2.wav", 1.0f },
        };

        class OrkThrowingGobbo : public ThrowingWeapon
        {
        public:
            OrkThrowingGobbo(const ItemParameters& parameters)
                : ThrowingWeapon(parameters, "", OrkThrowingGobboSkeletonPath, OrkThrowingGobboMatsetPaths)
            {
                SetAttachPoints("orklord_attach_0", "orklord_attach_1");
                SetRange(OrkThrowingGobboRange);
                SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, OrkThrowingGobboDPS));

                SetSpinLimits(OrkThrowingGobboMinSpin, OrkThrowingGobboMaxSpin);

                SetMaterialCollisionSound(OrkThrowingGobboBounceSounds);
            }

            void ShowGobbo()
            {
                _hidden = false;
            }

            void HideGobbo()
            {
                _hidden = true;
            }

            void Fire(const Character::Damage& dmg) override
            {
                ThrowingWeapon::Fire(dmg);
                HideGobbo();
                GetSkeleton()->PlayAnimation("ork_grab", true);
            }

        protected:
            void OnLoadContent(Content::ContentManager* contentManager) override
            {
                ThrowingWeapon::OnLoadContent(contentManager);

                GetSkeleton()->PlayAnimation("ork_grab", true);
            }

            void OnUnloadContent() override
            {
                ThrowingWeapon::OnUnloadContent();
            }

            void OnUpdate(double totalTime, float dt) override
            {
                ThrowingWeapon::OnUpdate(totalTime, dt);

                Color c = GetColor();
                if (_hidden)
                {
                    c.A = 0;
                }
                else
                {
                    c.A = 255;
                }
                SetColor(c);
            }

        private:
            bool _hidden = true;
        };

        static const float OrkAnvilGlovesDPS = 10.0f;

        static const std::string OrkAnvilGlovesSkeletonPath = "Skeletons/Items/Weapons/OrkWeapons/anvil_gloves.skel";
        static const std::string OrkAnvilGlovesMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/anvil_gloves.polymatset";

        class OrkAnvilGloves : public BasicMeleeWeapon
        {
        public:
            OrkAnvilGloves(const ItemParameters& parameters)
                : BasicMeleeWeapon(parameters, WeaponType_Melee_1H, OrkAnvilGlovesSkeletonPath, OrkAnvilGlovesMatsetPath)
            {
                SetSplash(true);
                SetAttachPoints("a", "b");
                SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, OrkAnvilGlovesDPS));
            }
        };
    }

    template <>
    void EnumeratePreloads<Item::OrkThrowingGobbo>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);

        preloads.insert(Item::OrkThrowingGobboSkeletonPath);
        preloads.insert(Item::OrkThrowingGobboMatsetPaths.begin(), Item::OrkThrowingGobboMatsetPaths.end());

        EnumerateSoundVectorPreloads(preloads, Item::OrkThrowingGobboBounceSounds);
    }

    template <>
    void EnumeratePreloads<Item::OrkAnvilGloves>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::OrkAnvilGlovesSkeletonPath);
        preloads.insert(Item::OrkAnvilGlovesMatsetPath);
    }

    namespace Character
    {
        static const std::string OrkLordSkeletonPath = "Skeletons/Characters/Ork/orklord/orklord_1.skel";
        static const std::string OrkLordMatsetPath = "Skeletons/Characters/Ork/orklord/orklord_1.polymatset";

        static const std::string OrkLordGobCageSkeletonPath = "Skeletons/Characters/Gobcage/Gobcage.skel";
        static const std::string OrkLordGobCageMatsetPath = "Skeletons/Characters/Gobcage/Gobcage.polymatset";

        static const std::vector<std::pair<float, std::string>> OrkLordMaterialBreakPoints = 
        {
            { 0.95f, "armor_2" },
            { 0.90f, "armor_3" },
            { 0.85f, "armor_0" },
            { 0.80f, "chestplate_0" },
            { 0.75f, "helm_0" },
            { 0.70f, "helm_1" },
        };
        static const std::vector<std::string> OrkLordTransitionToBoxingBreakPoints = 
        {
            "chestchain_a",
            "chestchain_b",
            "shoulder_a",
            "shoulder_b",
            "chain_a",
            "chain_b",
        };

        static const float BoxingPhaseStart = 0.6f;

        static const std::vector<std::string> OrkLordThrowAnimations
        {
            "gob_toss_0",
            "gob_toss_1",
        };
        static const std::string OrkLordThrowAnimationShowGobboTag = "spawn_gob0";

        static const Audio::SoundPathVector OrkLordLeapSounds =
        {
        };

        static const std::set<std::string> OrkLordGobAttachPoints
        {
            "range_0",
            "range_1",
            "range_2",
        };

        static const float OrkLordGobboRange = 2500.0f;

        static const Audio::SoundPathVector OrkLordArmorPhysicsSounds =
        {
            { "Audio/Characters/Debris/Debris_bouncing.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_2.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_3.wav", 1.0f },
        };

        OrkLord::OrkLord(const CharacterParameters parameters)
            : OrkLord(parameters, OrkLordPhase::ThrowingGoblins)
        {
        }

        OrkLord::OrkLord(const CharacterParameters parameters, OrkLordPhase phase)
            : Ork(parameters, OrkLordSkeletonPath, OrkLordMatsetPath)
            , _brokenMats(OrkLordMaterialBreakPoints.size(), false)
            , _phase(phase)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            SetEntityMask(GetEntityMask() | CharacterMask_Usable);

            SetMaxHealth(1500.0f);

            SetHeadName("head");

            setPhaseAnimations(_phase);

            AddWeaponAttachment("handa", "weapon_a_0", "weapon_a_1", { { Item::WeaponType_Melee_1H, "handa" }, });
            AddWeaponAttachment("handb", "weapon_b_0", "weapon_b_1", { { Item::WeaponType_Melee_1H, "handb" },
                                                                       { Item::WeaponType_Ranged_Thrown, "handb" }, });

            AddAttackAnimation("gob_toss_0", 1.0f, { { "handb", { Item::WeaponType_Ranged_Thrown,{ { "throw_0", "throw_0" } },{},{ "attack_sound0", },{} } } });
            AddAttackAnimation("gob_toss_1", 1.0f, { { "handb",{ Item::WeaponType_Ranged_Thrown,{ { "throw_0", "throw_0" } },{},{ "attack_sound0", },{} } } });

            AddAttackAnimation("attack_0", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start",   "dmg_end" }, },{},{},{ "woosh0" } } } });
            AddAttackAnimation("attack_1", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start",   "dmg_end" }, },{},{},{ "woosh0" } } } });
            AddAttackAnimation("attack_2", 1.0f, { { "handb", { Item::WeaponType_Melee_1H, { { "dmg_start",   "dmg_end" }, },{},{},{ "woosh0" } } } });
            AddAttackAnimation("attack_3", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" }, },{},{},{ "woosh0" } } },
                                                   { "handb", { Item::WeaponType_Melee_1H, { { "dmg_startb", "dmg_endb" }, },{},{},{ "woosh1" } } } });
            AddAttackAnimation("attack_4", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" }, { "dmg_startb", "dmg_endb" }, },{ "reset_0", },{},{ "woosh0", "woosh1" } } },
                                                   { "handb", { Item::WeaponType_Melee_1H, { { "dmg_startc", "dmg_endc" }, },{},{},{ "woosh2" } } } });
            AddAttackAnimation("attack_5", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" },{ "dmg_startb", "dmg_endb" }, },{ "reset_0", },{},{ "woosh0", "woosh1" } } } });
            AddAttackAnimation("attack_6", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_starta", "dmg_enda" }, { "dmg_startb", "dmg_endb" }, },{ "reset_0", },{},{ "woosh0", "woosh1" } } },
                                                   { "handb", { Item::WeaponType_Melee_1H, { { "dmg_startc", "dmg_endc" }, },{},{},{ "woosh2" } } } });
            AddAttackAnimation("attack_7", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start",   "dmg_end" }, },{},{},{ "woosh0" } } } });
            AddAttackAnimation("attack_8", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start",   "dmg_end" }, },{},{},{} } } });
            AddAttackAnimation("attack_9", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start",   "dmg_end" }, },{},{},{ "woosh0" } } } });

            AddFootJoint("foota_front", "foota_back");
            AddFootJoint("footb_front", "footb_back");
            AddRunParticleTag("dust_0");
            AddRunParticleTag("dust_1");
            AddRunParticleTag("dust_2");
            AddRunParticleTag("dust_3");
            AddRunParticleTag("dust_4");

            SetJawJoint("jaw");

            _leapWindUpAnimations.AddAnimation("crouch_0", 1.0f);
            _leapAnimations.AddAnimation("leap_0", 1.0f);
            _leapLandAnimations.AddAnimation("land_0", 1.0f);
            _leapSounds.AddSounds(OrkLeapSounds);
            for (const auto& leapTerrainSounds : Audio::GetStandardTerrainLeapFootstepSoundPaths())
            {
                _leapTerrainSounds[leapTerrainSounds.first] = Audio::SoundSet(leapTerrainSounds.second);
            }
            for (const auto& leapLandTerrainSounds : Audio::GetStandardTerrainLandFootstepSoundPaths())
            {
                _leapLandTerrainSounds[leapLandTerrainSounds.first] = Audio::SoundSet(leapLandTerrainSounds.second);
            }
            _leapLandAlwaysSounds.AddSounds(OrkLeapLandAlwaysSounds);

            for (const auto& gobAttachPoint : OrkLordGobAttachPoints)
            {
                AddCharacterAttachPoint(gobAttachPoint);
            }

            SetMaterialCollisionSound(OrkLordArmorPhysicsSounds);
        }

        float OrkLord::OnPreLeap(Pathfinding::TerrainType terrainType)
        {
            return PlayAnimationSet(_leapWindUpAnimations, false, 0.1f, 0.0f);
        }

        void OrkLord::OnLeap(Pathfinding::TerrainType terrainType, float airTime, float maxAirTime)
        {
            Speak(_leapSounds, false, true);

            auto footSoundIter = _leapTerrainSounds.find(terrainType);
            if (footSoundIter != _leapTerrainSounds.end())
            {
                auto soundManager = GetLevel()->GetSoundManager();
                soundManager->PlaySinglePositionalSound(footSoundIter->second.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                                                        OrkSpeechRange.first, OrkSpeechRange.second, 1.0f);
            }

            PlayAnimationSet(_leapAnimations, true, 0.1f, 0.0f);
        }

        float OrkLord::OnPostLeap(Pathfinding::TerrainType terrainType)
        {
            auto soundManager = GetLevel()->GetSoundManager();

            auto footSoundIter = _leapLandTerrainSounds.find(terrainType);
            if (footSoundIter != _leapLandTerrainSounds.end())
            {
                soundManager->PlaySinglePositionalSound(footSoundIter->second.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                                                        OrkSpeechRange.first, OrkSpeechRange.second, 1.0f);
            }
            soundManager->PlaySinglePositionalSound(_leapLandAlwaysSounds.GetNextSound(), Audio::SoundPriority::Medium, GetFeetPosition(),
                                                    OrkSpeechRange.first, OrkSpeechRange.second, 1.0f);

            return PlayAnimationSet(_leapLandAnimations, false, 0.1f, 0.0f);
        }

        Rayf OrkLord::GetAttachPoint(const std::string& name) const
        {
            if (OrkLordGobAttachPoints.count(name) > 0)
            {
                return Rayf(_gobCage->GetJointPosition(name), Vector2f::Zero);
            }
            else
            {
                return Ork::GetAttachPoint(name);
            }
        }

        void OrkLord::OnSpawn()
        {
            Ork::OnSpawn();
            setPhaseWeapons(_phase, true);
            if (_phase == +OrkLordPhase::ThrowingGoblins)
            {
                spawnCageGobbos();
            }
        }

        void OrkLord::OnLoadContent(Content::ContentManager* contentManager)
        {
            Ork::OnLoadContent(contentManager);

            _gobCage = Content::CreateSkeletonInstance(contentManager, OrkLordGobCageSkeletonPath, OrkLordGobCageMatsetPath);
            GetSkeleton()->AddChildDrawable("attach_0", _gobCage);

            _gobCage->PlayAnimation("idle_0", true);

            _gobCageCollision = new Physics::SkeletonCollision(GetLevelLayer()->GetPhysicsWorld(), _gobCage);

            _leapSounds.LoadContent(contentManager);
            for (auto& leapTerrainType : _leapTerrainSounds)
            {
                leapTerrainType.second.LoadContent(contentManager);
            }
            for (auto& leapTerrainType : _leapLandTerrainSounds)
            {
                leapTerrainType.second.LoadContent(contentManager);
            }
            _leapLandAlwaysSounds.LoadContent(contentManager);
        }

        void OrkLord::OnUnloadContent()
        {
            SafeRelease(_gobCageCollision);

            GetSkeleton()->RemoveChildDrawable("attach_0", _gobCage);
            SafeRelease(_gobCage);

            _leapSounds.UnloadContent();
            for (auto& leapTerrainType : _leapTerrainSounds)
            {
                leapTerrainType.second.UnloadContent();
            }
            for (auto& leapTerrainType : _leapLandTerrainSounds)
            {
                leapTerrainType.second.UnloadContent();
            }
            _leapLandAlwaysSounds.UnloadContent();

            Ork::OnUnloadContent();
        }

        void OrkLord::OnUpdate(double totalTime, float dt)
        {
            Ork::OnUpdate(totalTime, dt);

            _gobCage->Update(totalTime, dt);
            _gobCageCollision->Update(totalTime, dt);

            float healthPerc = GetHealth().GetPercent();
            for (uint32_t i = 0; i < OrkLordMaterialBreakPoints.size(); i++)
            {
                if (!_brokenMats[i] && healthPerc < OrkLordMaterialBreakPoints[i].first)
                {
                    BreakMaterial(OrkLordMaterialBreakPoints[i].second);
                    _brokenMats[i] = true;
                }
            }

            for (const auto& throwAnim : OrkLordThrowAnimations)
            {
                if (IsPlayingAnimation(throwAnim) && HasAnimationTagJustPassed(OrkLordThrowAnimationShowGobboTag))
                {
                    auto weapons = GetWeapons<Item::OrkThrowingGobbo>();
                    for (auto weapon : weapons)
                    {
                        weapon->ShowGobbo();
                    }
                    break;
                }
            }

            switch (_phase)
            {
                case OrkLordPhase::ThrowingGoblins:
                {
                    Animation::AttachmentInfo attachInfo =
                        Animation::GetSkeletonAttachmentInfo(GetSkeleton(), "attach_0", "attach_1", true,
                            _gobCage, "attach_0", "attach_1");
                    _gobCage->SetInvertedX(attachInfo.InvertX);
                    _gobCage->SetInvertedY(attachInfo.InvertY);
                    _gobCage->SetPosition(attachInfo.Position);
                    _gobCage->SetScale(attachInfo.Scale);
                    _gobCage->SetRotation(attachInfo.Rotation);

                    if (healthPerc < BoxingPhaseStart)
                    {
                        setPhase(OrkLordPhase::TransitionToBoxing);
                    }
                }
                break;

            case OrkLordPhase::TransitionToBoxing:
                setPhase(OrkLordPhase::Boxing);
                break;

            case OrkLordPhase::Boxing:
                break;
            }
        }

        void OrkLord::setPhase(OrkLordPhase phase)
        {
            if (_phase == phase)
            {
                return;
            }
            _phase = phase;

            setPhaseAnimations(_phase);
            setPhaseWeapons(_phase, false);

            if (_phase == +OrkLordPhase::TransitionToBoxing)
            {
                for (const auto& breakmat : OrkLordTransitionToBoxingBreakPoints)
                {
                    BreakMaterial(breakmat);
                }

                _gobCageCollision->SetBehavior(Physics::CollisionBehavior_Dynamic, true);

                DetachChildren();
            }
        }

        void OrkLord::setPhaseAnimations(OrkLordPhase phase)
        {
            ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_Walk);
            ClearTerrainAnimations(AnimationType_Move, Pathfinding::EdgeType_Walk);

            switch (phase)
            {
            case OrkLordPhase::ThrowingGoblins:
            case OrkLordPhase::TransitionToBoxing:
                SetMoveSpeed(300.0f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_0", 1.0f);
                AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "walk_0", 1.0f);
                break;

            case OrkLordPhase::Boxing:
                SetMoveSpeed(600.0f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_1", 1.0f);
                AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "walk_1", 1.0f);
                break;
            }
        }

        void OrkLord::setPhaseWeapons(OrkLordPhase phase, bool justSpawned)
        {
            auto discardWeapons = [this]()
            {
                for (auto weapon : GetWeapons())
                {
                    DiscardItem(weapon);
                }
            };

            switch (phase)
            {
            case OrkLordPhase::ThrowingGoblins:
                discardWeapons();
                GiveItem(Item::BindItemConstructor<Item::OrkThrowingGobbo>());
                break;

            case OrkLordPhase::TransitionToBoxing:
                if (justSpawned)
                {
                    discardWeapons();
                    GiveItem(Item::BindItemConstructor<Item::OrkThrowingGobbo>());
                }
                break;

            case OrkLordPhase::Boxing:
                discardWeapons();
                GiveItem(Item::BindItemConstructor<Item::OrkAnvilGloves>());
                GiveItem(Item::BindItemConstructor<Item::OrkAnvilGloves>());
                break;
            }
        }

        void OrkLord::spawnCageGobbos()
        {
            const std::vector<Item::ItemConstructor<Item::BasicRangedWeapon>> gobItems
            {
                Item::BindItemConstructor<Item::GobBow>(),
                Item::BindItemConstructor<Item::GobBow>(),
                Item::BindItemConstructor<Item::GobThrowingRocks>()
            };

            for (uint32_t i = 0; i < OrkLordGobAttachPoints.size(); i++)
            {
                auto* gobbo = GetLevelLayer()->SpawnCharacter(GetPosition(), "orklord_cage_gobbo", GetController(), BindCharacterConstructor<GobboBowman>(), Level::SpawnOrdering::Back);
                gobbo->SetAggroRange(OrkLordGobboRange);

                auto* weapon = GetLevelLayer()->CreateItem(GetPosition(), gobItems[i % gobItems.size()]);
                weapon->SetRange(OrkLordGobboRange);

                gobbo->GiveItem(weapon);

                gobbo->PushAction(CreateInteractCharacterAction(GetID()), false);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::Ork>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert(Character::OrkDamageParticlePath);
        preloads.insert(Character::OrkRunParticlesPath);

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnFleshDamageSoundPaths());

        EnumerateSoundVectorPreloads(preloads, Character::OrkAggroSounds);
        EnumerateSoundVectorPreloads(preloads, Character::OrkAttackSounds);
        EnumerateSoundVectorPreloads(preloads, Character::OrkDeathSounds);
        EnumerateSoundVectorPreloads(preloads, Character::OrkIdleSounds);
        EnumerateSoundVectorPreloads(preloads, Character::OrkTauntSounds);

        EnumerateSoundVectorPreloads(preloads, Audio::GetHeavyBasicFootstepSoundPaths());
        for (const auto& terrainFootsteps : Audio::GetStandardTerrainFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainFootsteps.second);
        }

        for (const auto& wooshSound : Character::OrkAttackWooshSounds)
        {
            EnumerateSoundVectorPreloads(preloads, wooshSound.second);
        }
    }

    template <>
    void EnumeratePreloads<Character::OrkWarrior>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Ork>(preloads);

        preloads.insert(Character::OrkWarriorSkeletonPath);
        preloads.insert(Character::OrkWarriorMatsetPath);

        for (auto interchangeMat : Character::OrkWarriorInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }

        EnumerateSoundVectorPreloads(preloads, Character::OrkLeapSounds);
        for (const auto& terrainLeapFootsteps : Audio::GetStandardTerrainLeapFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainLeapFootsteps.second);
        }
        for (const auto& terrainLeapLandFootsteps : Audio::GetStandardTerrainLandFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainLeapLandFootsteps.second);
        }
        EnumerateSoundVectorPreloads(preloads, Character::OrkLeapLandAlwaysSounds);
    }

    template <>
    void EnumeratePreloads<Character::ShieldOrk>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Ork>(preloads);
        EnumeratePreloads<Item::OrkLance>(preloads);
        EnumeratePreloads<Item::OrkTowerShield>(preloads);
        
        preloads.insert(Character::ShieldOrkSkeletonPath);
        preloads.insert(Character::ShieldOrkMatsetPath);
        preloads.insert(Character::ShieldOrkRubbleSkeletonPath);
        preloads.insert(Character::ShieldOrkRubbleMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Character::ShieldOrkCollisionSounds);
    }

    template <>
    void EnumeratePreloads<Character::OrkCannoneer>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Ork>(preloads);

        preloads.insert(Character::OrkCannoneerSkeletonPath);
        preloads.insert(Character::OrkCannoneerMatsetPath);

        preloads.insert(Character::OrkCannonBallSkeletonPath);
        preloads.insert(Character::OrkCannonBallMatsetPath);

        EnumeratePreloads<Item::OrkCannon>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::OrkLord>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Ork>(preloads);

        preloads.insert(Character::OrkLordSkeletonPath);
        preloads.insert(Character::OrkLordMatsetPath);
        preloads.insert(Character::OrkLordGobCageSkeletonPath);
        preloads.insert(Character::OrkLordGobCageMatsetPath);

        EnumeratePreloads<Item::OrkThrowingGobbo>(preloads);
        EnumeratePreloads<Item::OrkAnvilGloves>(preloads);

        EnumerateSoundVectorPreloads(preloads, Character::OrkLeapSounds);
        for (const auto& terrainLeapFootsteps : Audio::GetStandardTerrainLeapFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainLeapFootsteps.second);
        }
        for (const auto& terrainLeapLandFootsteps : Audio::GetStandardTerrainLandFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainLeapLandFootsteps.second);
        }
        EnumerateSoundVectorPreloads(preloads, Character::OrkLeapLandAlwaysSounds);

        EnumeratePreloads<Character::Gobbo>(preloads);
        EnumeratePreloads<Item::GobBow>(preloads);

        EnumerateSoundVectorPreloads(preloads, Character::OrkLordArmorPhysicsSounds);
    }
}
