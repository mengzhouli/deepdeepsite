#include "Levels/DwarfHomes/DwarfHome4.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/Dwarves/BuilderDwarf.hpp"

#include "Characters/Worm.hpp"
#include "Characters/Ork.hpp"
#include "Characters/Gobbo.hpp"

#include "Items/Weapons/OrkWeapons.hpp"

#include "Abilities/BuildBridge.hpp"

#include "Drawables/Flame.hpp"
#include "Drawables/BasketRope.hpp"

#include "Audio/LavaSound.hpp"

#include "Levels/GameLevels.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const Audio::MusicTrackMap DwarfHome4MusicTracks = Audio::MusicTrackMap
        {
        };

        static const auto TowerChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Miner(),
                        {
                            { Item::Weapons::IronPickAxe(), 1 }
                        }
                    },
                },

                // Armors
                Item::FindableItems
                {
                },

                // Trinkets
                Item::FindableItems
                {
                },
            };
        };

        static const auto WorkshopChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Navigator(),
                        {
                            { Item::Weapons::BlackPrincessPistol(), 1 }
                        }
                    },
                },

                // Armors
                Item::FindableItems
                {
                },

                // Trinkets
                Item::FindableItems
                {
                },
            };
        };

        static const auto KitchenChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Cook(),
                        {
                            { Item::Weapons::ButcherTools(), 1 }
                        }
                    },
                },

                // Armors
                Item::FindableItems
                {
                },

                // Trinkets
                Item::FindableItems
                {
                },
            };
        };

        static const auto FindableCharacters = []()
        {
            return Character::FindableCharacterSet
            {
            };
        };

        static const auto FindableAbilites = []()
        {
            return Ability::FindableAbilitySet
            {
                {
                    Character::Dwarves::Miner(),
                    {
                        Ability::Abilities::Sprint(),
                    },
                },
                {
                    Character::Dwarves::Builder(),
                    {
                        Ability::Abilities::BuildBridge(),
                        Ability::Abilities::BuildBarricade(),
                    },
                },
            };
        };

        DwarfHome4Checkpoint::DwarfHome4Checkpoint()
        {
        }

        DwarfHome4Checkpoint::~DwarfHome4Checkpoint()
        {
        }

        bool DwarfHome4Checkpoint::WasForgeDoorOpened() const
        {
            return _forgeDoorOpened;
        }

        void DwarfHome4Checkpoint::SetForgeDoorOpened(bool opened)
        {
            _forgeDoorOpened = opened;
        }

        bool DwarfHome4Checkpoint::WasBossKilled() const
        {
            return _bossKilled;
        }

        void DwarfHome4Checkpoint::SetBossKilled(bool killed)
        {
            _bossKilled = killed;
        }

        static const float ForgeAreaLightTurnOnTime = 2.0f;

        static const Color LavaLightColor = Color::FromBytes(192, 192, 192, 255);

        static const std::string AreaLightLayerName = "far_background";
        static const std::string FlameLightLayerName = "background";
        static const std::string PillarLightLayerName = "background";
        static const std::string ThroneLightLayerName = "foreground";
        static const std::string BasketRopeLayerName = "buckets";

        DwarfHome4::DwarfHome4(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : DwarfHomeLevel(parameters, campaignParameters)
            , _forgeAreaLightOn(false)
            , _forgeAreaLightOnTime(0.0f)
            , _forgeAreaLight()
            , _mainAreaLight()
            , _flameRadius(0.0f)
            , _mainFlames()
            , _forgeFlames()
        {
            AddMusicTracks(DwarfHome4MusicTracks);
        }

        DwarfHome4::~DwarfHome4()
        {
        }

        CampaignLevelInfo DwarfHome4::GetCampaignLevelInfo()
        {
            CampaignLevelInfo info;
            info.NameCode = "dwarfhome4_name";
            info.MaxTotalCharacters = 6;
            info.MinTypedCharacters[typeid(Character::NavigatorDwarf)] = 1;
            info.MinTypedCharacters[typeid(Character::MinerDwarf)] = 1;
            info.MinTypedCharacters[typeid(Character::BuilderDwarf)] = 1;
            info.StartingResources = Item::Resources(0);

            info.FindableCharacters = FindableCharacters();
            info.FindableItems = Item::MergeFindableItems(TowerChestItems(), WorkshopChestItems(), KitchenChestItems());
            info.FindableAbilities = FindableAbilites();

            return info;
        }

        static void giveBuilderBuildBridge(const Character::Controller* playerController)
        {
            Ability::AbilityConstructor<> abilityConstructor = Ability::GameAbility::GetConstructor(Ability::Abilities::BuildBridge());
            std::vector<Character::BuilderDwarf*> builders = playerController->GetCharacters<Character::BuilderDwarf>(std::string());
            for (Character::BuilderDwarf* builder : builders)
            {
                if (builder->GetAbility<Ability::BuildBridge>() == nullptr)
                {
                    builder->AddAbility(abilityConstructor);
                }
            }
        }

        void DwarfHome4::OnCreate()
        {
            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            SpawnCheckpoint<DwarfHome4Checkpoint>(primaryLayer->GetSpline("dwarf_spawn"), "dwarfhome4_spawn_checkpoint_name", true);
            //SpawnCheckpoint<DwarfHome4Checkpoint>(primaryLayer->GetSpline("workshop_checkpoint"), "dwarfhome4_workshop_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome4Checkpoint>(primaryLayer->GetSpline("assembly_hall_checkpoint"), "dwarfhome4_assembly_hall_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome4Checkpoint>(primaryLayer->GetSpline("tower_checkpoint"), "dwarfhome4_tower_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome4Checkpoint>(primaryLayer->GetSpline("mines_checkpoint"), "dwarfhome4_mines_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome4Checkpoint>(primaryLayer->GetSpline("bridge_checkpoint"), "dwarfhome4_bridge_checkpoint_name", false);

            DwarfHomeLevel::OnCreate();

            SetDefaultEnvironmenType(Audio::EnvironmentType::None);

            std::shared_ptr<const DwarfHome4Checkpoint> checkpoint = GetCheckpoint<DwarfHome4Checkpoint>();

            SpawnTrackedOres(primaryLayer, "small_ore_spawn", Item::Resources(25), checkpoint);

            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Medium, "rocks1_spawn", "rocks1_collision", nullptr, checkpoint);
            SpawnTrackedDestructableWall(primaryLayer, "workshop_wall_spawn", "workshop_wall_collision", nullptr, checkpoint);
            SpawnTrackedDestructableWall(primaryLayer, "tower_wall_spawn", "tower_wall_collision", nullptr, checkpoint);
            SpawnTrackedDestructableWall(primaryLayer, "assembly_back_door_spawn", "assembly_back_door_collision", nullptr, checkpoint);

            SpawnTrackedDoor(primaryLayer, "tower_left_lever_spawn", "tower_left_door_spawn", "tower_left_door_collision", "", Character::DoorSize_Medium, false, true, nullptr, checkpoint);
            SpawnTrackedDoor(primaryLayer, "tower_right_lever_spawn", "tower_right_door_spawn", "tower_right_door_collision", "", Character::DoorSize_Medium, true, false, nullptr, checkpoint);
            SpawnTrackedDoor(primaryLayer, "tower_enterance_lever_spawn", "tower_enterance_door_spawn", "tower_enterance_door_collision", "", Character::DoorSize_Medium, false, true, nullptr, checkpoint);

            // Spawn orks
            std::vector<Item::ItemConstructor<>> orkWeaps
            {
                Item::BindItemConstructor<Item::OrkChopper>(),
                Item::BindItemConstructor<Item::OrkClub>(),
                Item::BindItemConstructor<Item::OrkHook>(),
                Item::BindItemConstructor<Item::OrkMace>(),
            };

            Character::CharacterConstructor<Character::OrkWarrior> orkConstructor = Character::BindCharacterConstructor<Character::OrkWarrior>();
            std::vector<Character::OrkWarrior*> orks = SpawnCharactersAtTriggers("ork_spawn", orkConstructor, "ork", GetMonsterController());
            for (auto ork : orks)
            {
                ork->GiveItem(Random::RandomItem(orkWeaps));
                ork->GiveItem(Random::RandomItem(orkWeaps));
            }

            // Spawn cannon ork
            Character::CharacterConstructor<Character::OrkCannoneer> cannonOrkConstructor = Character::BindCharacterConstructor<Character::OrkCannoneer>();
            SpawnTrackedCharactersAtTriggers("cannon_ork_spawn", cannonOrkConstructor, "cannon_ork", GetMonsterController(), checkpoint);

            // Spawn worms
            Character::CharacterConstructor<Character::Worm> wormConstructor = Character::BindCharacterConstructor<Character::Worm>();
            SpawnTrackedCharactersAtTriggers("worm_spawn", wormConstructor, "werm", GetWildlifeController(), checkpoint);

            // Spawn bramble
            Character::CharacterConstructor<Character::GobboBramble> brambleConstructor = Character::BindCharacterConstructor<Character::GobboBramble>();
            SpawnTrackedCharactersAtTriggers("bramble_spawn", brambleConstructor, "bramble", GetMonsterController(), checkpoint);

            // Spawn chests
            SpawnTrackedChest(primaryLayer, "tower_chest_spawn", true, TowerChestItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "workshop_chest_spawn", true, WorkshopChestItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "kitchen_chest_spawn", true, KitchenChestItems(), checkpoint);

            // Spawn flames
            const Polygonf& flameSizer = primaryLayer->GetTriggerArea("flame_sizer");
            _flameRadius = flameSizer.Bounds().H;

            std::vector<Vector2f> flameSpawns = primaryLayer->GetTriggerPositions("flame_spawn");
            for (auto flameSpawn : flameSpawns)
            {
                Graphics::Flame* flame = new Graphics::Flame(BaseFireColor, false, true, false, false, GetSoundManager());
                flame->SetPosition(flameSpawn);
                flame->SetRadius(_flameRadius);
                flame->SetBrightnessRange(0.5f, 1.0f);
                flame->LoadContent(GetContentManager());
                _mainFlames.push_back(std::unique_ptr<Graphics::Flame>(flame));
            }

            std::vector<Vector2f> forgeFlameSpawns = primaryLayer->GetTriggerPositions("forge_flame_spawn");
            for (auto flameSpawn : flameSpawns)
            {
                Graphics::Flame* flame = new Graphics::Flame(BaseFireColor, false, true, false, false, GetSoundManager());
                flame->SetPosition(flameSpawn);
                flame->SetRadius(0.0f);
                flame->SetBrightnessRange(0.5f, 1.0f);
                flame->LoadContent(GetContentManager());
                _forgeFlames.push_back(std::unique_ptr<Graphics::Flame>(flame));
            }

            std::vector<Splinef> basketSpawns = primaryLayer->GetSplines("basket_rope_spawn");
            for (const auto& basketSpawn : basketSpawns)
            {
                Graphics::BasketRopeDrawable* basket = new Graphics::BasketRopeDrawable(GetContentManager(), basketSpawn);
                _basketRopes.push_back(std::unique_ptr<Graphics::BasketRopeDrawable>(basket));
            }

            Chainf lavaSoundChain(primaryLayer->GetSpline("lava_sound").Points());
            const Vector2f& lavaSoundBase = primaryLayer->GetTriggerPosition("lava_sound_base");
            const Vector2f& lavaSoundMin = primaryLayer->GetTriggerPosition("lava_sound_min");
            const Vector2f& lavaSoundMax = primaryLayer->GetTriggerPosition("lava_sound_max");
            Audio::LavaSound* lavaSound = new Audio::LavaSound(GetContentManager(), GetSoundManager(), lavaSoundChain, Vector2f::Distance(lavaSoundBase, lavaSoundMin), Vector2f::Distance(lavaSoundBase, lavaSoundMax));
            _lavaSound = std::unique_ptr<Audio::LavaSound>(lavaSound);
            _lavaSound->Play(0.0f);

            LevelLayerInstance* areaLightLayer = GetLayer(AreaLightLayerName);
            _mainAreaLight.Area = areaLightLayer->GetTriggerArea("main_area_light");
            _mainAreaLight.LightColor = LavaLightColor;

            _forgeAreaLight.Area = areaLightLayer->GetTriggerArea("forge_area_light");
            _forgeAreaLight.LightColor = Color::FromBytes(0, 0, 0, 255);

            LevelLayerInstance* pillarLightLayer = GetLayer(PillarLightLayerName);
            const auto& pillarLightAreas = pillarLightLayer->GetTriggerAreas("pillar_light_area");
            for (const auto& pillarLightArea : pillarLightAreas)
            {
                const Rectanglef& bounds = pillarLightArea.Bounds();

                Lights::LineLight light;
                light.Start = Vector2f(bounds.Middle().X, bounds.Top());
                light.End = Vector2f(bounds.Middle().X, bounds.Bottom());
                light.LightColor = Color::FromBytes(0, 0, 0, 255);
                light.ShadowPercent = 0.0f;
                light.StartRadius = bounds.W * 0.5f;
                light.EndRadius = bounds.W * 0.5f;

                _forgePillarLights.push_back(light);
            }

            LevelLayerInstance* throneLightLayer = GetLayer(ThroneLightLayerName);
            const auto& throneLightPositions = throneLightLayer->GetTriggerPositions("throne_light");
            for (const auto& throneLightPosition : throneLightPositions)
            {
                Lights::PointLight light;
                light.Position = throneLightPosition;
                light.LightColor = Color::FromBytes(0, 0, 0, 255);
                light.ShadowPercent = 0.0f;
                light.Radius = 1500.0f;

                _forgeThroneLights.push_back(light);
            }

            _forgeAreaLightOn = (checkpoint && checkpoint->WasForgeDoorOpened());
            auto doorOpenCallback = [this](bool newState)
            {
                if (newState)
                {
                    if (!_forgeAreaLightOn)
                    {
                        forgeGobboAttack();
                    }
                    _forgeAreaLightOn = true;
                    return true;
                }
                else if (_forgeAreaLightOn)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            };
            SpawnTrackedDoor(primaryLayer, "forge_door_lever_spawn", "forge_door_spawn", "forge_door_closed_collision", "forge_door_open_collision", Character::DoorSize_Huge, _forgeAreaLightOn, true, doorOpenCallback, checkpoint);
            if (_forgeAreaLightOn)
            {
                forgeGobboAttack();
            }

            bool bossAlive = (!checkpoint || !checkpoint->WasBossKilled());
            if (bossAlive)
            {
                Character::CharacterConstructor<Character::OrkLord> bossConstructor = Character::BindCharacterConstructor<Character::OrkLord>();
                std::vector<Character::OrkLord*> bosses = SpawnCharactersAtTriggers("ork_lord_spawn", bossConstructor, "boss", GetMonsterController());
                assert(bosses.size() == 1);
                _bossID = bosses.front()->GetID();
            }
            else
            {
                _bossID = 0;
            }

            SetTargetCameraViewSize(3500.0f, 16.0f / 9.0f);
        }

        void DwarfHome4::OnDestroy()
        {
            DwarfHomeLevel::OnDestroy();

            for (auto &flame : _forgeFlames)
            {
                flame->UnloadContent();
            }
            _forgeFlames.clear();

            for (auto &flame : _mainFlames)
            {
                flame->UnloadContent();
            }
            _mainFlames.clear();

            _basketRopes.clear();

            _lavaSound.reset();
        }

        void DwarfHome4::OnUpdate(double totalTime, float dt)
        {
            DwarfHomeLevel::OnUpdate(totalTime, dt);

            for (auto &basketRope : _basketRopes)
            {
                basketRope->Update(totalTime, dt);
            }

            if (_forgeAreaLightOn)
            {
                _forgeAreaLightOnTime += dt;
                float bossAreaLightPerc = Clamp(_forgeAreaLightOnTime / ForgeAreaLightTurnOnTime, 0.0f, 1.0f);
                _forgeAreaLight.LightColor = LavaLightColor * bossAreaLightPerc;
                _forgeAreaLight.LightColor.A = 255;

                for (auto &flame : _forgeFlames)
                {
                    flame->SetRadius(_flameRadius * bossAreaLightPerc);
                    flame->Update(totalTime, dt);
                }

                for (auto& pillarLight : _forgePillarLights)
                {
                    pillarLight.LightColor = Color::White * bossAreaLightPerc;
                    pillarLight.LightColor.A = 255;
                }

                for (auto& throneLight : _forgeThroneLights)
                {
                    throneLight.LightColor = Color::White * bossAreaLightPerc;
                    throneLight.LightColor.A = 255;
                }
            }

            for (auto &flame : _mainFlames)
            {
                flame->Update(totalTime, dt);
            }

            _lavaSound->Update(totalTime, dt, GetCameraController().GetCamera());
        }

        void DwarfHome4::OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            DwarfHomeLevel::OnDraw(layer, levelRenderer);

            if (layer->GetName() == AreaLightLayerName)
            {
                levelRenderer->AddLight(_mainAreaLight);
            }

            if (layer->GetName() == BasketRopeLayerName)
            {
                for (const auto &basketRope : _basketRopes)
                {
                    levelRenderer->AddDrawable(basketRope.get(), false);
                }
            }

            if (layer->GetName() == FlameLightLayerName)
            {
                for (const auto &flame : _mainFlames)
                {
                    levelRenderer->AddLight(flame->GetMainLight());
                    levelRenderer->AddLight(flame->GetCenterLight());
                }
            }

            if (_forgeAreaLightOn)
            {
                if (layer->GetName() == AreaLightLayerName)
                {
                    levelRenderer->AddLight(_forgeAreaLight);
                }

                if (layer->GetName() == FlameLightLayerName)
                {
                    for (const auto &flame : _forgeFlames)
                    {
                        levelRenderer->AddLight(flame->GetMainLight());
                        levelRenderer->AddLight(flame->GetCenterLight());
                    }
                }

                if (layer->GetName() == PillarLightLayerName)
                {
                    for (const auto& pillarLight : _forgePillarLights)
                    {
                        levelRenderer->AddLight(pillarLight);
                    }
                }

                if (layer->GetName() == ThroneLightLayerName)
                {
                    for (const auto& throneLight : _forgeThroneLights)
                    {
                        levelRenderer->AddLight(throneLight);
                    }
                }
            }
        }

        void DwarfHome4::OnSpawnCampaignCharacter(Character::Character* character)
        {
            DwarfHomeLevel::OnSpawnCampaignCharacter(character);

            character->SetRotation(GetPrimaryLayer()->GetTriggerPosition("spawn_character_look_dir") - character->GetPosition());
        }

        void DwarfHome4::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile)
        {
            DwarfHomeLevel::OnLevelSuccessfullyCompleted(profile);
            UnlockCharacters(profile, FindableCharacters());
            UnlockAbilities(profile, FindableAbilites());
        }

        void DwarfHome4::PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> baseCheckpoint)
        {
            std::shared_ptr<DwarfHome4Checkpoint> checkpoint = AsA<DwarfHome4Checkpoint>(baseCheckpoint);

            DwarfHomeLevel::PopulateCheckpoint(checkpoint);

            checkpoint->SetForgeDoorOpened(_forgeAreaLightOn);
            checkpoint->SetBossKilled(GetPrimaryLayer()->GetCharacter(_bossID) == nullptr);
        }

        void DwarfHome4::forgeGobboAttack()
        {
            auto callback = [this]
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();
                Polygonf forgeGobboArea = primaryLayer->GetTriggerArea("forge_goblin_area");

                std::vector<Vector2f> hitPts;
                auto forgeGobbos = primaryLayer->FindIntersections<Character::Character>(forgeGobboArea.Bounds(), hitPts, [&](const Character::Character* character) { return character->GetController() == GetMonsterController(); });
                std::vector<Character::Action> actions =
                {
                    Character::CreateAttackMoveAction(primaryLayer->GetTriggerPosition("forge_goblin_attack_target0")),
                    Character::CreateAttackMoveAction(primaryLayer->GetTriggerPosition("forge_goblin_attack_target1")),
                    Character::CreateAttackMoveAction(primaryLayer->GetTriggerPosition("forge_goblin_attack_target2")),
                    Character::CreateAttackMoveAction(primaryLayer->GetTriggerPosition("forge_goblin_attack_target3")),
                    Character::CreateAttackMoveAction(primaryLayer->GetTriggerPosition("forge_goblin_attack_target4")),
                    Character::CreateAttackMoveAction(primaryLayer->GetTriggerPosition("forge_goblin_attack_target5")),
                };
                for (auto gobbo : forgeGobbos)
                {
                    for (uint32_t i = 0; i < actions.size(); i++)
                    {
                        gobbo->PushAction(actions[i], i != 0);
                    }
                }
            };

            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            primaryLayer->GetPathSystem()->AddFinalizationCallback(callback);
        }

    }

    template <>
    void EnumeratePreloads<Level::DwarfHome4>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::DwarfHomeLevel>(preloads);

        EnumeratePreloads<Graphics::Flame>(preloads);
        EnumeratePreloads<Graphics::BasketRopeDrawable>(preloads);
        EnumeratePreloads<Audio::LavaSound>(preloads);

        EnumeratePreloads<Character::Worm>(preloads);
        EnumeratePreloads<Character::OrkCannoneer>(preloads);
        EnumeratePreloads<Character::OrkLord>(preloads);
        EnumeratePreloads<Character::OrkWarrior>(preloads);
        EnumeratePreloads<Character::GobboBramble>(preloads);

        EnumeratePreloads<Item::OrkChopper>(preloads);
        EnumeratePreloads<Item::OrkClub>(preloads);
        EnumeratePreloads<Item::OrkHook>(preloads);
        EnumeratePreloads<Item::OrkMace>(preloads);

        EnumerateFindableAbilitySetPreloads(preloads, Level::FindableAbilites());
        EnumerateFindableItemSetPreloads(preloads, Level::TowerChestItems());
        EnumerateFindableItemSetPreloads(preloads, Level::WorkshopChestItems());
        EnumerateFindableItemSetPreloads(preloads, Level::KitchenChestItems());

        EnumerateMusicTrackMapPreloads(preloads, Level::DwarfHome4MusicTracks);
    }
}
