#include "Characters/Worm.hpp"
#include "Item/Weapon.hpp"

#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Item
    {
        class WormWeapon : public Weapon
        {
        public:
            WormWeapon(const ItemParameters& parameters, Character::CharacterID worm)
                : Weapon(parameters, "null", WeaponType_Melee_1H)
                , _wormID(worm)
                , _dps(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 2.5f)
            {
            }

            virtual const Rectanglef& GetBounds() const override
            {
                static const Rectanglef defaultRect;
                const Character::Worm* worm = GetLevelLayer()->GetCharacter<Character::Worm>(_wormID);
                return (worm != nullptr) ? worm->GetBounds() : defaultRect;
            }

            virtual Physics::Collision* GetCollision() const override
            {
                const Character::Worm* worm = GetLevelLayer()->GetCharacter<Character::Worm>(_wormID);
                return (worm != nullptr) ? worm->GetCollision() : nullptr;
            }

            virtual const Character::Damage& GetDPS() const override
            {
                return _dps;
            }

            virtual float GetRange() const override
            {
                const Character::Worm* worm = GetLevelLayer()->GetCharacter<Character::Worm>(_wormID);
                return (worm != nullptr) ? worm->GetAttackRange() : 0.0f;
            }

        private:
            Character::CharacterID _wormID;
            Character::Damage _dps;
        };
    }

    namespace Character
    {
        static std::string WormSkeletonPath = "Skeletons/Characters/Worm/flavor_worm.skel";
        static std::string WormMatsetPath = "Skeletons/Characters/Worm/flavor_worm.polymatset";

        static const std::string WormRangeBaseJoint = "origin";
        static const std::string WormRangeExtentJoint = "range_joint";

        static const std::string WormIdleExposedAnimation = "idle_out";
        static const std::string WormIdleHiddenAnimation = "idle_in";
        static const std::string WormEmergeAnimation = "out";
        static const std::string WormHideAnimation = "in";

        static const Audio::SoundPathVector WormAttackSoundPaths =
        {
            { "Audio/Characters/worm/Worm_Attack.ogg", 1.0f },
            { "Audio/Characters/worm/Worm_Attack_Alt.ogg", 1.0f },
        };
        static const Audio::SoundPathVector WormIdleSoundPaths =
        {
            { "Audio/Characters/worm/Worm_Loop.ogg", 1.0f },
        };

        Worm::Worm(const CharacterParameters& parameters)
            : BasicCharacter(parameters, WormSkeletonPath, WormMatsetPath)
            , _visible(false)
            , _emerging(false)
            , _emergeTimer(0.0f)
            , _hidePerc(Random::RandomBetween(0.25f, 0.75f))
            , _hideTimer(10.0f)
            , _hiding(false)
            , _doneHiding(false)
            , _wormEmergedSounds()
        {
            SetEntityMask(CharacterMask_None);
            SetMoveType(MoveType_None);
            SetAttachToGroundOnSpawn(true);
            SetMaxHealth(70.0f);
            SetHeadName("los");

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, WormIdleHiddenAnimation, 1.0f);

            SetItemSlotCount(Item::ItemSlot::Weapon, 1);
            AddWeaponAttachment(WormRangeBaseJoint, WormRangeBaseJoint, WormRangeExtentJoint, { { Item::WeaponType_Melee_1H, WormRangeBaseJoint } });
            AddAttackAnimation("attack1", 1.0f, { { WormRangeBaseJoint, { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" }, }, {}, { "attack_sound0", } } } });
            AddAttackAnimation("attack2", 1.0f, { { WormRangeBaseJoint, { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" }, }, {}, { "attack_sound0", } } } });

            AddIdleSounds(WormIdleSoundPaths);
            AddAttackSounds(WormAttackSoundPaths);
            SetAttackSoundPlayChance(0.3f);
            _wormEmergedSounds.AddSounds(WormIdleSoundPaths);

            SetPositionalSpeechParameters(1.0f, 500.0f, 2500.0f);

            SetAggroRange(1000.0f);

            SetBodyMaterial(MaterialType_Type_Flesh);
            AddDamageSounds(Audio::GetStandardOnFleshDamageSoundPaths());
        }

        float Worm::GetAttackRange() const
        {
            const Animation::SkeletonInstance* skeleton = GetSkeleton();
            return Vector2f::Distance(skeleton->GetJointPosition(WormRangeBaseJoint), skeleton->GetJointPosition(WormRangeExtentJoint));
        }

        void Worm::OnSpawn()
        {
            BasicCharacter::OnSpawn();

            GiveItem(Item::BindItemConstructor<Item::WormWeapon>(GetID()));
        }

        void Worm::OnDeath()
        {
            if (_curWormEmergedSound)
            {
                _curWormEmergedSound->Stop(0.5f);
                _curWormEmergedSound = nullptr;
            }
        }

        void Worm::OnUpdate(double totalTime, float dt)
        {
            BasicCharacter::OnUpdate(totalTime, dt);

            if (_emerging)
            {
                _emergeTimer -= dt;
                if (_emergeTimer <= 0.0f)
                {
                    _visible = true;
                    _emerging = false;
                    SetEntityMask(CharacterMask_Normal);
                }
            }

            if (_hiding)
            {
                _hideTimer -= dt;
                if (_hideTimer <= 0.0f)
                {
                    _hiding = false;
                }
            }

            CharacterState curState = GetCurrentState();
            if (curState == CharacterState_Idle && !_emerging && !_hiding)
            {
                CharacterMask prevMask = GetEntityMask();

                float enemySearchRange = GetWeaponRange() * 1.5f;

                SetEntityMask(CharacterMask_Normal);
                Character* nearbyEnemy = FindNearbyAttackableTarget(enemySearchRange);
                SetEntityMask(prevMask);

                if (nearbyEnemy && !_visible)
                {
                    SetEntityMask(CharacterMask_Normal);
                    std::vector<Character*> nearbyEnemies = FindNearbyAttackableTargets(Max(GetAggroRange(), enemySearchRange));
                    SetEntityMask(prevMask);

                    assert(!nearbyEnemies.empty());
                    Character* attackEnemy = Random::RandomItem(nearbyEnemies);

                    std::shared_ptr<const Pathfinding::Path> enemyPath = attackEnemy->GetCurrentPath();
                    if (enemyPath != nullptr)
                    {
                        auto position = enemyPath->GetPathPosition(enemySearchRange * 0.5f);
                        SetPosition(position->GetPosition());
                    }
                    else
                    {
                        SetPosition(attackEnemy->GetPosition());
                    }

                    emerge();
                }
                else if (nearbyEnemy == nullptr && _visible)
                {
                    hide();
                }
            }
        }

        void Worm::emerge()
        {
            ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_Walk);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, WormIdleExposedAnimation, 1.0f);

            PlayAnimation(WormEmergeAnimation, false, 0.0f, 0.0f);
            _emerging = true;
            _emergeTimer = GetAnimationLength(WormEmergeAnimation);

            auto soundManager = GetLevel()->GetSoundManager();
            _curWormEmergedSound = soundManager->PlayLoopingPositionalSound(_wormEmergedSounds.GetNextSound(), Audio::SoundPriority::Medium, GetPosition(), 500.0f, 2500.0f, 0.5f);
        }

        void Worm::hide()
        {
            ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_Walk);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, WormIdleHiddenAnimation, 1.0f);

            PlayAnimation(WormHideAnimation, false, 0.0f, 0.0f);
            SetEntityMask(CharacterMask_None);
            _visible = false;

            if (_curWormEmergedSound)
            {
                _curWormEmergedSound->Stop(0.1f);
                _curWormEmergedSound = nullptr;
            }
        }

        void Worm::OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg)
        {
            if (!_doneHiding && GetHealth().GetPercent() <= _hidePerc)
            {
                _hiding = true;
                _doneHiding = true;
                _hideTimer = Random::RandomBetween(3.0f, 8.0f);
                hide();
            }
        }

        void Worm::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicCharacter::OnLoadContent(contentManager);
            _wormEmergedSounds.LoadContent(contentManager);
        }

        void Worm::OnUnloadContent()
        {
            BasicCharacter::OnUnloadContent();
            _wormEmergedSounds.UnloadContent();
        }
    }

    template <>
    void EnumeratePreloads<Character::Worm>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert(Character::WormSkeletonPath);
        preloads.insert(Character::WormMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Character::WormAttackSoundPaths);
        EnumerateSoundVectorPreloads(preloads, Character::WormIdleSoundPaths);

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnFleshDamageSoundPaths());
    }
}
