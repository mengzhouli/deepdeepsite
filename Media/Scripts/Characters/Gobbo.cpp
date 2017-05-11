#include "Characters/Gobbo.hpp"

#include "Item/Weapon.hpp"

#include "Characters/Bomb.hpp"

#include "DamageTypes.hpp"
#include "Buffs/SpeedBoost.hpp"
#include "Buffs/DamageOverTime.hpp"
#include "Characters/DamageSounds.hpp"
#include "Characters/FootstepSounds.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string GobboSkeletonPath = "Skeletons/Characters/Gobbo/gobbo.skel";
        static const std::string GobboMatsetPath = "Skeletons/Characters/Gobbo/gobbo.polymatset";

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> GobboInterchangeMatsetPaths =
        {
            {
                "body",
                {
                    { "Skeletons/Characters/Gobbo/gobchest_1.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobchest_2.polymatset", 0.1f },
                }
            },
            {
                "head",
                {
                    { "Skeletons/Characters/Gobbo/gobhead_01.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_02.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_03.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_04.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_05.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_06.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_07.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_08.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_09.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_10.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_11.polymatset", 1.0f },
                }
            },
            {
                "jaw",
                {
                    { "Skeletons/Characters/Gobbo/gobjaw_1.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobjaw_2.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobjaw_3.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobjaw_4.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobjaw_5.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobjaw_6.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobjaw_7.polymatset", 1.0f },
                }
            },
            {
                "upper_arm",
                {
                    { "Skeletons/Characters/Gobbo/gobupperarm_0.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobupperarm_1.polymatset", 0.1f },
                }
            },
            {
                "forearma",
                {
                    { "Skeletons/Characters/Gobbo/gobforearma_0.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobforearma_1.polymatset", 0.1f },
                }
            },
            {
                "forearmb",
                {
                    { "Skeletons/Characters/Gobbo/gobforearmb_0.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobforearmb_1.polymatset", 0.1f },
                }
            },
        };

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> RangedGobboInterchangeMatsetPaths =
        {
            {
                "head",
                {
                    { "Skeletons/Characters/Gobbo/gobheadrange_00.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobheadrange_01.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobheadrange_02.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobheadrange_03.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobheadrange_04.polymatset", 1.0f },
                }
            },
        };

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> GobboThrowerInterchangeMatsetPaths =
        {
            {
                "body",
                {
                    { "Skeletons/Characters/Gobbo/gobchestthrow_00.polymatset", 1.0f },
                }
            },
        };

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> GobboBowmanInterchangeMatsetPaths =
        {
            {
                "body",
                {
                    { "Skeletons/Characters/Gobbo/gobchestrange_00.polymatset", 1.0f },
                }
            },
        };

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> GobboBomberInterchangeMatsetPaths =
        {
            {
                "body",
                {
                    { "Skeletons/Characters/Gobbo/gobchestbomb_00.polymatset", 1.0f },
                }
            },
        };

        static const std::map<std::string, std::vector<std::pair<std::string, float>>> GobboChiefInterchangeMatsetPaths =
        {
            {
                "body",
                {
                    { "Skeletons/Characters/Gobbo/gobchest_chief.polymatset", 1.0f },
                }
            },
            {
                "head",
                {
                    { "Skeletons/Characters/Gobbo/gobhead_chief.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_chief1.polymatset", 1.0f },
                    { "Skeletons/Characters/Gobbo/gobhead_chief2.polymatset", 1.0f },
                }
            },
        };

        static const Audio::SoundPathVector GobboAggroSounds =
        {
            { "Audio/vocalizations/Goblin/Goblin_Aggro1.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Aggro2.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Aggro3.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Aggro4.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Aggro5.ogg", 1.0f },
        };

        static const Audio::SoundPathVector GobboAttackSounds =
        {
            { "Audio/vocalizations/Goblin/Goblin_Attack1.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Attack1.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Attack1.ogg", 1.0f },
        };

        static const Audio::SoundPathVector GobboIdleSounds =
        {
            { "Audio/vocalizations/Goblin/Goblin_Idle1.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Idle2.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Idle3.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Idle4.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Idle5.ogg", 1.0f },
        };

        static const Audio::SoundPathVector GobboDeathSounds =
        {
            { "Audio/vocalizations/Goblin/Goblin_Death1.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Death2.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Death3.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Death4.ogg", 1.0f },
            { "Audio/vocalizations/Goblin/Goblin_Death5.ogg", 1.0f },
        };

        static const std::map<Item::WeaponType, Audio::SoundPathVector> GobboAttackWooshSounds =
        {
            {
                Item::WeaponType_Ranged_Bow,
                {
                    { "Audio/Items/Weapons/Bow_Shot_1.ogg", 1.0f },
                    { "Audio/Items/Weapons/Bow_Shot_2.ogg", 1.0f },
                    { "Audio/Items/Weapons/Bow_Shot_3.ogg", 1.0f },
                    { "Audio/Items/Weapons/Bow_Shot_4.ogg", 1.0f },
                }
            },
        };

        static const std::string GobboDamageParticlePath = "Particles/blood_splat.partsys";
        static const std::string GobboRunParticlesPath = "Particles/dust_02.partsys";

        Gobbo::Gobbo(const CharacterParameters& parameters)
            : BasicCharacter(parameters, GobboSkeletonPath, GobboMatsetPath)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            float size = Random::RandomBetween(0.9f, 1.1f);

            SetMoveType(MoveType_Walk);
            SetPathableEdges(static_cast<Pathfinding::EdgeType>(Pathfinding::EdgeType_Walk | Pathfinding::EdgeType_Jump | Pathfinding::EdgeType_Climb));
            SetMoveSpeed(Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, 400.0f);
            SetMoveSpeed(Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, 250.0f);
            SetMoveSpeed(Pathfinding::EdgeType_Jump, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, 2500.0f);
            SetMaxHealth(20.0f * size);
            SetHealthRegeneration(0.0f);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(size);
            SetSkeletonColor(Color::White);

            const float sitIdleChance = 0.3f;
            if (Random::RandomBetween(0.0f, 1.0f) < sitIdleChance)
            {
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_sit", 1.0f);
            }
            else
            {
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle1", 1.0f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle2", 0.02f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle3", 0.02f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle4", 0.02f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle5", 0.02f);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle6", 0.02f);
            }

            std::vector<std::string> runAnimations =
            {
                "run_1",
                "run_2",
                "run3",
            };
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, Random::RandomItem(runAnimations), 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Jump, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "fall_0", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "ladder_climb0", 1.0f);

            AddWeaponAttachment("handa", "weapona_0", "weapona_1", { { Item::WeaponType_Melee_1H, "handa" },
                                                                     { Item::WeaponType_Ranged_Thrown, "handa" }, });
            AddWeaponAttachment("handb", "weaponb_0", "weaponb_1", { { Item::WeaponType_Melee_1H, "handb" },
                                                                     { Item::WeaponType_Ranged_Bow, "handb" }, });

            AddAttackAnimation("attack1", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, {} } } });
            AddAttackAnimation("attack2", 1.0f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, {} } } });
            AddAttackAnimation("attack4", 0.5f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, {} } } });

            AddAttackAnimation("attack3", 0.5f, { { "handa", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, {} } },
                                                  { "handb", { Item::WeaponType_Melee_1H, { { "dmg_start", "dmg_end" } }, {}, { "attack_sound0", }, {} } } });

            AddAttackAnimation("throw", 1.0f, { { "handa", { Item::WeaponType_Ranged_Thrown, { { "throw_0", "throw_0" } },{}, { "attack_sound0", }, {} } } });
            AddAttackAnimation("range_attack1", 1.0f, { { "handb", { Item::WeaponType_Ranged_Bow, { { "fire_arrow", "fire_arrow" } }, {}, { "attack_sound0", }, { "fire_arrow", } } } });

            AddAttackWooshSounds(GobboAttackWooshSounds);

            SetHeadName("head");
            SetJawJoint("Jaw");
            AddFootJoint("foota", "foota");
            AddFootJoint("footb", "footb");
            SetRunParticleSystem(GobboRunParticlesPath);
            AddRunParticleTag("dust_0");
            AddRunParticleTag("dust_1");

            for (auto interchangeMat : GobboInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }

            SetAggroRange(1000.0f);

            AddDamagedParticleSystem(DamageType_Element_Physical, GobboDamageParticlePath);

            SetBodyMaterial(MaterialType_Type_Flesh);
            AddDamageSounds(Audio::GetStandardOnFleshDamageSoundPaths());

            AddIdleSounds(GobboIdleSounds);
            SetIdleSoundInterval(10.0f, 40.0f);

            AddAttackSounds(GobboAttackSounds);
            AddAggroSounds(GobboAggroSounds);
            AddDeathSounds(GobboDeathSounds);

            AddBaseFootstepSounds(Audio::GetStandardBasicFootstepSoundPaths());
            for (const auto& terrainFootsteps : Audio::GetStandardTerrainFootstepSoundPaths())
            {
                AddTerrainFootstepSounds(terrainFootsteps.first, terrainFootsteps.second);
            }
        }

        GobboThrower::GobboThrower(const CharacterParameters& parameters)
            : Gobbo(parameters)
        {
            ClearMaterialGroup("body");
            for (auto interchangeMat : GobboThrowerInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }
            for (auto interchangeMat : RangedGobboInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }
        }

        GobboBowman::GobboBowman(const CharacterParameters& parameters)
            : Gobbo(parameters)
        {
            ClearMaterialGroup("body");
            for (auto interchangeMat : GobboBowmanInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }
            for (auto interchangeMat : RangedGobboInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }

            SetBodyMaterial(MaterialType_Type_Cloth);
            AddDamageSounds(Audio::GetStandardOnClothDamageSoundPaths());
        }
    }

    namespace Item
    {
        class BombHoldWeapon : public Weapon
        {
        public:
            BombHoldWeapon(const ItemParameters& parameters, Character::CharacterID bomb)
                : Weapon(parameters, "null", WeaponType_Melee_1H)
            {
            }

            virtual const Rectanglef& GetBounds() const override
            {
                const Physics::Collision* collision = GetCollision();
                if (collision)
                {
                    return collision->GetBounds();
                }
                else
                {
                    return _bounds;
                }
            }

            virtual const Character::Damage& GetDPS() const override
            {
                static const Character::Damage dps(0, 0.0f);
                return dps;
            }

            virtual float GetRange() const override
            {
                return 0.0f;
            }

            virtual Physics::Collision* GetCollision() const override
            {
                Character::Explosive* bomb = GetLevelLayer()->GetCharacter<Character::Explosive>(_bomb);
                if (bomb)
                {
                    return bomb->GetCollision();
                }
                else
                {
                    return nullptr;
                }
            }

            virtual void OnUpdate(double totalTime, float dt) override
            {
                _bounds.Position = GetPosition() - Vector2f(0.5f);
                _bounds.Size = Vector2f(1.0f);
            }

        private:
            Character::CharacterID _bomb;

            Rectanglef _bounds;
        };
    }

    namespace Character
    {
        static const std::string GobboBombSkeletonPath = "Skeletons/Items/Weapons/GobWeapons/gobbomb.skel";
        static const std::string GobboBombMatsetPath = "Skeletons/Items/Weapons/GobWeapons/gobbomb.polymatset";

        const float GobboBombDuration = 3.5f;
        const float GobboBombRadius = 1000.0f;
        const float GobboBombDmg = 20.0f;

        class GobboBomb : public Bomb
        {
        public:
            GobboBomb(const CharacterParameters& parameters, CharacterID owner)
                : Bomb(parameters, GobboBombSkeletonPath, GobboBombMatsetPath, "fuse", "origin", owner, GobboBombDuration, GobboBombRadius, GobboBombDmg)
            {
            }
        };

        GobboBomber::GobboBomber(const CharacterParameters& parameters)
            : Gobbo(parameters)
        {
            SetMoveSpeed(500.0f);
            AddCharacterAttachPoint("handb");

            ClearTerrainAnimations(AnimationType_Move, Pathfinding::EdgeType_Walk);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "suicide_run", 1.0f);

            ClearMaterialGroup("body");
            for (auto interchangeMat : GobboBomberInterchangeMatsetPaths)
            {
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }
        }

        void GobboBomber::OnSpawn()
        {
            Gobbo::OnSpawn();

            CharacterConstructor<GobboBomb> constructor = BindCharacterConstructor<GobboBomb>(GetID());
            GobboBomb* bomb = GetLevelLayer()->SpawnCharacter(GetPosition(), "bomb", nullptr, constructor);
            bomb->Stop();
            bomb->AttachToCharacter(this);
            bomb->DisablePhysics();
            _bomb = bomb->GetID();

            GiveItem(Item::BindItemConstructor<Item::BombHoldWeapon>(bomb->GetID()));
        }

        void GobboBomber::OnStateChange(CharacterState state)
        {
            Gobbo::OnStateChange(state);

            if (!IsAlive())
            {
                GobboBomb* bomb = GetLevelLayer()->GetCharacter<GobboBomb>(_bomb);
                if (bomb != nullptr)
                {
                    bomb->Start();
                    bomb->EnablePhysics(true);
                    bomb->DetachFromParent();
                }
            }
        }

        void GobboBomber::OnUpdate(double totalTime, float dt)
        {
            Gobbo::OnUpdate(totalTime, dt);

            Character* attackTarget = GetAttackTarget();
            if (attackTarget != nullptr)
            {
                GobboBomb* bomb = GetLevelLayer()->GetCharacter<GobboBomb>(_bomb);
                if (bomb != nullptr)
                {
                    bomb->Start();
                }
            }
        }

        GobboChief::GobboChief(const CharacterParameters& parameters)
            : Gobbo(parameters)
        {
            float size = Random::RandomBetween(1.4f, 1.6f);
            SetMaxHealth(30.0f * size);
            SetSkeletonScale(size);

            for (auto interchangeMat : GobboChiefInterchangeMatsetPaths)
            {
                ClearMaterialGroup(interchangeMat.first);
                for (auto mat : interchangeMat.second)
                {
                    AddMaterial(interchangeMat.first, mat.first, mat.second);
                }
            }

            SetAggroRange(1200.0f);

            SetBodyMaterial(MaterialType_Type_Metal);
            AddDamageSounds(Audio::GetStandardOnMetalDamageSoundPaths());

            ClearAttackAnimations();
            AddAttackAnimation("chief_attack0", 1.0f, { { "handa",{ Item::WeaponType_Melee_1H,{ { "dmg_start", "dmg_end" } },{},{ "attack_sound0", },{} } } });
            AddAttackAnimation("chief_attack1", 1.0f, { { "handa",{ Item::WeaponType_Melee_1H,{ { "dmg_start", "dmg_end" } },{},{ "attack_sound0", },{} } } });

            ClearTerrainAnimations(AnimationType_Move, Pathfinding::EdgeType_Walk);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "chief_run", 1.0f);

            ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_Walk);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "chief_idle1", 1.0f);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "chief_idle2", 1.0f);
        }

        static const std::string GobboTowerSkeletonPath = "Skeletons/Characters/gobtower/gobtower.skel";
        static const std::string GobboTowerMatsetPath = "Skeletons/Characters/gobtower/gobtower.polymatset";

        static const std::vector<std::vector<std::string>> GobboTowerBreakableMaterials = 
        {
            { "plate_4", },
            { "plate_3", },
            { "plate_2", },
            { "plate_1", },
            { "plate_0", },
            { "platform_2", },
            { "tip_1", "upper_rope", },
            { "tip_4", "top_rope", "tip_3", },
        };

        static const Audio::SoundPathVector GobboTowerCollisionSounds =
        {
            { "Audio/Characters/Debris/Debris_MiscWood_Tower_1.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_MiscWood_Tower_2.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_MiscWood_Tower_3.wav", 1.0f },
        };

        GobboTower::GobboTower(const CharacterParameters& parameters)
            : BasicCharacter(parameters, GobboTowerSkeletonPath, GobboTowerMatsetPath)
            , _remainingBreakableMaterials(GobboTowerBreakableMaterials)
        {
            SetEntityMask(CharacterMask_Usable);
            SetMoveType(MoveType_None);
            DisablePhysics();
            SetSkeletonJointStrength(0.0f);

            AddCharacterAttachPoint("platform_0");
            AddCharacterAttachPoint("platform_1");
            AddCharacterAttachPoint("platform_3");
            AddCharacterAttachPoint("platform_4");

            SetBodyMaterial(MaterialType_Type_Metal);
            AddDamageSounds(Audio::GetStandardOnMetalDamageSoundPaths());

            SetMaterialCollisionSound(GobboTowerCollisionSounds);

            _onRecieveDamageAnimations.AddAnimation("take_damage", 1.0f);
        }

        void GobboTower::OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg)
        {
            BasicCharacter::OnPostRecieveDamage(source, position, dmg);

            uint32_t numBreakIntervals = GobboTowerBreakableMaterials.size() + 2;
            uint32_t curInterval = numBreakIntervals * GetHealth().GetPercent();
            if (curInterval > 0 && _remainingBreakableMaterials.size() > curInterval - 1)
            {
                uint32_t nextBreakIdx = Random::RandomBetween(0, _remainingBreakableMaterials.size() - 1);
                for (auto breakMaterial : _remainingBreakableMaterials[nextBreakIdx])
                {
                    BreakMaterial(breakMaterial);
                }
                _remainingBreakableMaterials.erase(_remainingBreakableMaterials.begin() + nextBreakIdx);
            }

            PlayAnimationSet(_onRecieveDamageAnimations, false, 0.0f, 0.0f);
        }

        void GobboTower::OnStateChange(CharacterState newState)
        {
            if (newState == CharacterState_Dead)
            {
                EnablePhysics(true);
            }

            BasicCharacter::OnStateChange(newState);
        }
    }

    namespace Item
    {
        class NULLWeapon : public Weapon
        {
        public:
            NULLWeapon(const ItemParameters& parameters)
                : Weapon(parameters, "null", WeaponType_Melee_1H)
            {
            }

            virtual const Rectanglef& GetBounds() const override
            {
                return _bounds;
            }

            virtual const Character::Damage& GetDPS() const override
            {
                static const Character::Damage dps(0, 0.0f);
                return dps;
            }

            virtual float GetRange() const override
            {
                return 0.0f;
            }

            virtual void OnUpdate(double totalTime, float dt) override
            {
                _bounds.Position = GetPosition() - Vector2f(0.5f);
                _bounds.Size = Vector2f(1.0f);
            }

        private:
            Rectanglef _bounds;
        };
    }

    namespace Character
    {
        static const std::string BrambleSkeletonPath = "Skeletons/Characters/Bramble/bramble.skel";
        static const std::string BrambleMatsetPath = "Skeletons/Characters/Bramble/bramble.polymatset";

        static const float BrambleTickInterval = 1.0f;
        static const float BrambleDPS = 1.0f;
        static const float BrambleSlow = 0.4f;

        class BrambleDebuff : public BasicBuff
        {
        public:
            BrambleDebuff(const BuffParameters& parameters, CharacterID owner, float duration)
                : BasicBuff(parameters, "buff_bramble_name")
            {
                SetTotalDuration(duration);
                AddComponent<SpeedBoost>(BrambleSlow);
                AddComponent<DamageOverTime>(owner, Damage(DamageType_Type_Melee | DamageType_Source_Slashing | DamageType_Element_Physical, BrambleDPS), BrambleTickInterval);
            }
        };

        GobboBramble::GobboBramble(const CharacterParameters& parameters)
            : BasicCharacter(parameters, BrambleSkeletonPath, BrambleMatsetPath)
            , _dmgTick(0.0f)
            , _attackAnimations()
        {
            _attackAnimations.AddAnimation("do_dmg", 1.0f);

            //SetEntityMask(CharacterMask_Normal | CharacterMask_Constructed);
            SetMoveType(MoveType_None);
            DisablePhysics();
            SetSkeletonJointStrength(0.0f);
            SetMaxHealth(20.0f);

            SetItemSlotCount(Item::ItemSlot::Weapon, 1);
        }

        void GobboBramble::OnSpawn()
        {
            BasicCharacter::OnSpawn();

            GiveItem(Item::BindItemConstructor<Item::NULLWeapon>());
        }

        void GobboBramble::OnUpdate(double totalTime, float dt)
        {
            BasicCharacter::OnUpdate(totalTime, dt);

            if (_dmgTick <= 0.0f)
            {
                // Search for hit characters
                std::vector<Vector2f> hitPts;
                std::vector<Character*> hitCharacters =
                    GetLevelLayer()->FindIntersections<Character>(GetCollision(), hitPts,
                        [this](const Character* character)
                        {
                            return IsCharacterAttackable(character, false);
                        });
                for (uint32_t i = 0; i < hitCharacters.size(); i++)
                {
                    hitCharacters[i]->ApplyBuff(BindBuffConstructor<BrambleDebuff>(GetID(), BrambleTickInterval));
                }

                if (!hitCharacters.empty())
                {
                    _dmgTick += BrambleTickInterval;
                    PlayAnimationSet(_attackAnimations, false, 0.0f, 0.0f);
                }
            }
            else
            {
                _dmgTick -= dt;
            }
        }

        void GobboBramble::OnStateChange(CharacterState newState)
        {
            if (newState == CharacterState_Dead)
            {
                EnablePhysics(true);
            }

            BasicCharacter::OnStateChange(newState);
        }
    }

    template <>
    void EnumeratePreloads<Character::GobboBomb>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Bomb>(preloads);

        preloads.insert(Character::GobboBombSkeletonPath);
        preloads.insert(Character::GobboBombMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::Gobbo>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert(Character::GobboSkeletonPath);
        preloads.insert(Character::GobboMatsetPath);

        for (auto interchangeMat : Character::GobboInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }

        preloads.insert(Character::GobboDamageParticlePath);
        preloads.insert(Character::GobboRunParticlesPath);

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnFleshDamageSoundPaths());

        EnumerateSoundVectorPreloads(preloads, Character::GobboAggroSounds);
        EnumerateSoundVectorPreloads(preloads, Character::GobboAttackSounds);
        EnumerateSoundVectorPreloads(preloads, Character::GobboIdleSounds);
        EnumerateSoundVectorPreloads(preloads, Character::GobboDeathSounds);

        EnumerateSoundVectorPreloads(preloads, Audio::GetStandardBasicFootstepSoundPaths());
        for (const auto& terrainFootsteps : Audio::GetStandardTerrainFootstepSoundPaths())
        {
            EnumerateSoundVectorPreloads(preloads, terrainFootsteps.second);
        }

        for (const auto& wooshSound : Character::GobboAttackWooshSounds)
        {
            EnumerateSoundVectorPreloads(preloads, wooshSound.second);
        }
    }

    template <>
    void EnumeratePreloads<Character::GobboThrower>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Gobbo>(preloads);

        for (auto interchangeMat : Character::GobboThrowerInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }

        for (auto interchangeMat : Character::RangedGobboInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::GobboBowman>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Gobbo>(preloads);

        for (auto interchangeMat : Character::RangedGobboInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }

        for (auto interchangeMat : Character::GobboBowmanInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnClothDamageSoundPaths());
    }

    template <>
    void EnumeratePreloads<Character::GobboBomber>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Gobbo>(preloads);
        EnumeratePreloads<Character::GobboBomb>(preloads);
        EnumeratePreloads<Item::BombHoldWeapon>(preloads);

        for (auto interchangeMat : Character::GobboBomberInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::GobboChief>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Gobbo>(preloads);

        for (auto interchangeMat : Character::GobboChiefInterchangeMatsetPaths)
        {
            for (auto mat : interchangeMat.second)
            {
                preloads.insert(mat.first);
            }
        }

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnMetalDamageSoundPaths());
    }

    template <>
    void EnumeratePreloads<Character::GobboTower>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert(Character::GobboTowerSkeletonPath);
        preloads.insert(Character::GobboTowerMatsetPath);

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnMetalDamageSoundPaths());

        EnumerateSoundVectorPreloads(preloads, Character::GobboTowerCollisionSounds);
    }

    template <>
    void EnumeratePreloads<Item::NULLWeapon>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Weapon>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::BrambleDebuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        EnumeratePreloads<Character::SpeedBoost>(preloads);
        EnumeratePreloads<Character::DamageOverTime>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::GobboBramble>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert(Character::BrambleSkeletonPath);
        preloads.insert(Character::BrambleMatsetPath);

        EnumeratePreloads<Item::NULLWeapon>(preloads);

        EnumeratePreloads<Character::BrambleDebuff>(preloads);
    }
}
