#include "Levels/DwarfHomes/DwarfHome2.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Abilities/Leap.hpp"
#include "Abilities/GroundSlam.hpp"
#include "Abilities/CookFood.hpp"
#include "Abilities/ChannelingFoodHeal.hpp"

#include "Levels/GameLevels.hpp"

#include "Characters/Torch.hpp"
#include "Characters/Portrait.hpp"
#include "Characters/Ork.hpp"
#include "Items/Weapons/OrkWeapons.hpp"

#include "CutsceneUtility.hpp"
#include "RainEffect.hpp"

#include "HUD/Debugger.hpp"
#include "HUD/DebuggerElements.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const Audio::MusicTrackMap DwarfHome2MusicTracks = Audio::MusicTrackMap
        {
        };

        static const std::string DwarfHome2RainLayer = "paralax_3";
        static const uint32_t DwarfHome2RainDropCount = 2048;
        static const Rotatorf DwarfHome2RainDir = Rotatorf(-PiOver2 + PiOver8);
        static const std::pair<float, float> DwarfHome2RainSpeed = { 3500.0f, 3500.0f };

        static const bool DwarfHome2LightningEnabled = true;
        static const std::pair<float, float> DwarfHome2LightningStrikeInterval = { 1.0f, 25.0f };
        static const float DwarfHome2LightningDoubleStrikeChance = 0.2f;

        static const auto CaveItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Builder(),
                        {
                            { Item::Weapons::IronHammer(), 1 },
                        }
                    },
                    {
                        Character::Dwarves::Miner(),
                        {
                            { Item::Weapons::IronPickAxe(), 1 },
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

        static const auto SecretBossItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Brewer(),
                        {
                            { Item::Weapons::HandCannonRifle(), 1 },
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

        static const auto BossItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Fighter(),
                        {
                            { Item::Weapons::IronShortSword(), 1 },
                            { Item::Weapons::WoodenHeater(), 1 },
                        }
                    },
                    {
                        Character::Dwarves::Navigator(),
                        {
                            { Item::Weapons::BlunderbussPistol(), 1 }
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
        
        static const auto UpperCaveChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                },

                // Armors
                Item::FindableItems
                {
                },

                // Trinkets
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Brewer(),
                        {
                            { Item::Trinkets::MeerschaumPipe(), 1 },
                        }
                    },
                },
            };
        };
        static const auto FindableCharacters = []()
        {
            return Character::FindableCharacterSet
            {
                Character::Dwarves::Brewer(),
            };
        };

        static const auto FindableAbilites = []()
        {
            return Ability::FindableAbilitySet
            {
                {
                    Character::Dwarves::Fighter(),
                    {
                        Ability::Abilities::Leap(),
                    },
                },
                {
                    Character::Dwarves::Cook(),
                    {
                        Ability::Abilities::ChannelingFoodHeal(),
                    },
                },
            };
        };

        DwarfHome2Checkpoint::DwarfHome2Checkpoint()
            : _bossKilled(false)
            , _leapPickedUp(false)
            , _playedEatingCutscene(false)
        {
        }

        DwarfHome2Checkpoint::~DwarfHome2Checkpoint()
        {
        }

        bool DwarfHome2Checkpoint::DidPlayEatingCutscene() const
        {
            return _playedEatingCutscene;
        }

        void DwarfHome2Checkpoint::SetPlayedEatingCutscene(bool played)
        {
            _playedEatingCutscene = played;
        }

        bool DwarfHome2Checkpoint::WasBossKilled() const
        {
            return _bossKilled;
        }

        void DwarfHome2Checkpoint::SetBossKilled(bool killed)
        {
            _bossKilled = killed;
        }

        bool DwarfHome2Checkpoint::WasLeapPickedUp() const
        {
            return _leapPickedUp;
        }

        void DwarfHome2Checkpoint::SetLeapPickedUp(bool pickedUp)
        {
            _leapPickedUp = pickedUp;
        }

        DwarfHome2::DwarfHome2(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : DwarfHomeLevel(parameters, campaignParameters)
            , _playedEatingCutscene(false)
            , _playedBossCutscene(false)
            , _bossLadderID(0)
            , _bossID(0)
            , _bossBrazierID(0)
            , _playedLeapCutscene(false)
            , _leapPickupID(0)
        {
            AddMusicTracks(DwarfHome2MusicTracks);

            _rain.reset(new RainEffect(GetLayer(DwarfHome2RainLayer), GetSoundManager()));
        }

        DwarfHome2::~DwarfHome2()
        {
            _rain.reset();
        }

        CampaignLevelInfo DwarfHome2::GetCampaignLevelInfo()
        {
            CampaignLevelInfo info;
            info.NameCode = "dwarfhome2_name";
            info.MaxTotalCharacters = 3;
            info.MinTypedCharacters[typeid(Character::NavigatorDwarf)] = 1;
            info.MinTypedCharacters[typeid(Character::FighterDwarf)] = 1;
            //info.MinTypedCharacters[typeid(Character::CookDwarf)] = 1;
            info.StartingResources = Item::Resources(0);

            info.FindableCharacters = FindableCharacters();
            info.FindableItems = Item::MergeFindableItems(CaveItems(), SecretBossItems(), BossItems(), UpperCaveChestItems());
            info.FindableAbilities = FindableAbilites();

            return info;
        }

        void DwarfHome2::InitializeDebugger(HUD::Debugger* debugger)
        {
            DwarfHomeLevel::InitializeDebugger(debugger);

            {
                {
                    auto rainDebuggerElement = std::make_shared<HUD::RainEffectDebuggerElement>(_rain.get());
                    debugger->AddElement("Weather", "Rain", rainDebuggerElement);
                }
            }
        }

        static void giveChefChannelingHeal(const Character::Controller* playerController)
        {
            Ability::AbilityConstructor<> abilityConstructor = Ability::GameAbility::GetConstructor(Ability::Abilities::ChannelingFoodHeal());
            std::vector<Character::CookDwarf*> cooks = playerController->GetCharacters<Character::CookDwarf>(std::string());
            for (Character::CookDwarf* cook : cooks)
            {
                if (cook->GetAbility<Ability::ChannelingFoodHeal>() == nullptr)
                {
                    cook->AddAbility(abilityConstructor);
                }
            }
        }

        static std::shared_ptr<Cutscene> createLeapCutscene(LevelLayerInstance* layer, const Character::Controller* monsterController, Character::CharacterID fighterID)
        {
            // Camera focus event
            auto focusCameraEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.Pan(layer->GetTriggerPosition("leap_cutscene_camera_pos"), 2.0f);
            };

            // Move event
            Vector2f moveTarget = layer->GetTriggerPosition("leap_cutscene_move_pos");
            auto moveStartFunc = [=]()
            {
                Character::FighterDwarf* character = layer->GetCharacter<Character::FighterDwarf>(fighterID);
                character->PushAction(Character::CreateMoveAction(moveTarget), false);
            };
            auto moveFinishedFunc = [=]()
            {
                Character::FighterDwarf* character = layer->GetCharacter<Character::FighterDwarf>(fighterID);
                return !character->IsMoving();
            };

            // Lower monster health
            auto lowerMonsterHealthEvent = [=]()
            {
                const Polygonf& monsterArea = layer->GetTriggerArea("leap_cutscene_monster_area");
                std::vector<Character::Character*> monsters = monsterController->GetCharacters<Character::Character>([=](const Character::Character* character)
                {
                    return Polygonf::Contains(monsterArea, character->GetBounds().Middle());
                });

                for (auto& monster : monsters)
                {
                    monster->SetMaxHealth(1.0f);
                }
            };

            // Leap event
            auto leapEvent = [=]()
            {
                Character::FighterDwarf* character = layer->GetCharacter<Character::FighterDwarf>(fighterID);
                assert(character != nullptr);
                Ability::Leap* ability = character->GetAbility<Ability::Leap>();
                assert(ability != nullptr);

                ability->SetPrimaryTarget(layer->GetTriggerPosition("leap_cutscene_jump_pos"));
                character->PushAction(ability->GenerateAction(), false);
                ability->ClearState();
            };

            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<FunctionalEvent>(focusCameraEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(0.5f),
                std::make_shared<FunctionalEvent>(moveStartFunc, moveFinishedFunc, nullptr, nullptr),
                std::make_shared<TimerEvent>(0.5f),
                std::make_shared<FunctionalEvent>(lowerMonsterHealthEvent, nullptr, nullptr, nullptr),
                std::make_shared<FunctionalEvent>(leapEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(2.0f),
            };
            return std::make_shared<Cutscene>(events);
        }

        static std::shared_ptr<Cutscene> createBossEnterCutscene(LevelLayerInstance* layer, const Character::Controller* playerController,
                                                                 Character::CharacterID bossID, Character::CharacterID bossLadderID,
                                                                 Character::CharacterID bossBrazierID)
        {
            // Teleport event
            auto teleportEvent = [=]()
            {
                const Vector2f& movePos = layer->GetTriggerPosition("boss_cutscene_dwarf_move_pos");
                const Vector2f& teleportPos = layer->GetTriggerPosition("boss_cutscene_dwarf_teleport_pos");

                std::shared_ptr<Pathfinding::Path> teleportPath =
                    layer->ComputePath(teleportPos, Pathfinding::EdgeType_All, movePos, Pathfinding::EdgeType_All, 500.0f, Pathfinding::EdgeType_All ,false);
                assert(teleportPath != nullptr);

                for (auto dwarf : playerController->GetCharacters<Character::Dwarf>(std::string(".*")))
                {
                    std::shared_ptr<Pathfinding::Path> directPath = dwarf->ComputePath(movePos, 500.0f);
                    if (directPath == nullptr || directPath->GetLength() > teleportPath->GetLength())
                    {
                        dwarf->SetPosition(teleportPos);
                    }

                    dwarf->PushAction(Character::CreateMoveAction(movePos), false);
                }
            };

            // Turn on brazier event
            auto brazierOnEvent = [=]()
            {
                Character::FlameHolder* brazier = layer->GetCharacter<Character::FlameHolder>(bossBrazierID);
                assert(brazier != nullptr);

                brazier->TurnOn();
            };

            // Leap event
            auto leapEvent = [=]()
            {
                Character::Character* boss = layer->GetCharacter<>(bossID);
                assert(boss != nullptr);

                Ability::Leap* ability = boss->AddAbility(Ability::BindAbilityConstructor<Ability::Leap>());
                assert(ability != nullptr);

                ability->SetPrimaryTarget(layer->GetTriggerPosition("boss_cutscene_leap_pos"));
                boss->PushAction(ability->GenerateAction(), false);
                ability->ClearState();
            };

            // Kill the ladder
            auto ladderKillEvent = [=]()
            {
                Character::Character* ladder = layer->GetCharacter(bossLadderID);
                if (ladder != nullptr)
                {
                    ladder->PushAction(Character::CreateDeathAction(), false);
                }
            };

            std::vector<std::shared_ptr<CutsceneEvent>> events1
            {
                std::make_shared<FunctionalEvent>(teleportEvent, nullptr, nullptr, nullptr),
                std::make_shared<FocusCameraEvent>(layer, "boss_cutscene_camera_pos", 1.0f),
                std::make_shared<FunctionalEvent>(brazierOnEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(0.5f),
                std::make_shared<MoveCharacterEvent>(layer, bossID, "boss_cutscene_move_pos"),
                std::make_shared<TimerEvent>(1.0f),
                std::make_shared<FunctionalEvent>(leapEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(1.0f),
                std::make_shared<FunctionalEvent>(ladderKillEvent, nullptr, nullptr, nullptr),
            };

            auto characterLookatBossEvent = [=](double, float)
            {
                Character::Character* boss = layer->GetCharacter<>(bossID);
                assert(boss != nullptr);
                Vector2f lookPos = boss->GetBounds().Middle();

                for (auto dwarf : playerController->GetCharacters<Character::Dwarf>(std::string(".*")))
                {
                    dwarf->SetRotation(Rotatorf(lookPos - dwarf->GetPosition()));
                    dwarf->LookAt(lookPos);
                }
            };

            std::vector<std::shared_ptr<CutsceneEvent>> events2
            {
                std::make_shared<TimerEvent>(1.0f),
                std::make_shared<TimedFunctionalEvent>(5.0f, nullptr, characterLookatBossEvent, nullptr),
            };

            auto events =
            {
                events1,
                events2,
            };

            return std::make_shared<Cutscene>(events);
        }

        static void giveFightersLeap(const Character::Controller* playerController)
        {
            Ability::AbilityConstructor<> abilityConstructor = Ability::GameAbility::GetConstructor(Ability::Abilities::Leap());
            std::vector<Character::FighterDwarf*> fighters = playerController->GetCharacters<Character::FighterDwarf>(std::string());
            for (Character::FighterDwarf* fighter : fighters)
            {
                if (fighter->GetAbility<Ability::Leap>() == nullptr)
                {
                    fighter->AddAbility(abilityConstructor);
                }
            }
        }

        static std::shared_ptr<Cutscene> createEatingCutscene(LevelLayerInstance* layer, const Character::Controller* playerController)
        {
            Character::CharacterID navigatorID = playerController->GetCharacters<Character::NavigatorDwarf>(std::string()).front()->GetID();
            Character::CharacterID cookID = playerController->GetCharacters<Character::CookDwarf>(std::string()).front()->GetID();
            Character::CharacterID fighterID = playerController->GetCharacters<Character::FighterDwarf>(std::string()).front()->GetID();

            // Camera focus event
            auto focusCameraEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.Pan(layer->GetTriggerPosition("eating_cutscene_camera_start"));
            };

            auto navFaceDirectionEvent = [=]()
            {
                Character::Character* navigator = layer->GetCharacter<>(navigatorID);
                const Vector2f& dirTarget = layer->GetTriggerPosition("camera_start");

                navigator->SetRotation(dirTarget - navigator->GetPosition());
            };

            auto dropFighterEvent = [=]()
            {
                Character::Character* fighter = layer->GetCharacter<>(fighterID);
                const Vector2f& dropSource = layer->GetTriggerPosition("eating_cutscene_fighter_drop_target");

                Character::Damage dmg(Character::DamageType_Type_Fall, fighter->GetHealth().GetCurrent() * 0.9f);
                fighter->ApplyDamage(nullptr, fighter->GetPosition(), dmg);

                fighter->SetPosition(dropSource);
                fighter->ApplyLinearImpulse(Vector2f::Zero);
            };

            auto fighterStartMovingEvent = [=]()
            {
                Character::Character* fighter = layer->GetCharacter<>(fighterID);

                const Vector2f& target = layer->GetTriggerPosition("eating_cutscene_chef_food_target");
                fighter->PushAction(Character::CreateMoveAction(target), false);
            };

            auto cookFoodEventBegin = [=]()
            {
                Character::Character* cook = layer->GetCharacter<>(cookID);
                Ability::Ability* ability = cook->GetAbility<Ability::CookFood>();
                assert(ability);

                const Vector2f& target = layer->GetTriggerPosition("eating_cutscene_chef_food_target");
                ability->SetPrimaryTarget(target);

                assert(ability->IsInValidState());
                cook->PushAction(ability->GenerateAction(), false);
                ability->ClearState();
            };

            auto cookFoodEventEnd = [=]()
            {
                Character::Character* cook = layer->GetCharacter<>(cookID);
                return cook->GetCurrentState() == Character::CharacterState_Idle;
            };

            auto fighterEatFoodEventBegin = [=]()
            {
                Character::Character* fighter = layer->GetCharacter<>(fighterID);

                std::vector<Character::Edible*> food = layer->GetCharacters<Character::Edible>(CharacterFilterFunction<Character::Edible>());
                assert(!food.empty());

                //assert(fighter->CanMoveTo(food.front()->GetPosition()), 200.0f);
                fighter->PushAction(Character::CreateInteractCharacterAction(AsA<Character::Character>(food.front())->GetID()), false);
            };

            auto fighterEatFoodEventEnd = [=]()
            {
                Character::FighterDwarf* fighter = layer->GetCharacter<Character::FighterDwarf>(fighterID);
                return !fighter->IsDowned();
            };

            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                {
                    std::make_shared<FunctionalEvent>(focusCameraEvent, nullptr, nullptr, nullptr),
                    std::make_shared<MoveCharacterEvent>(layer, navigatorID, "eating_cutscene_start_move_target"),
                    std::make_shared<MoveCharacterEvent>(layer, cookID, "eating_cutscene_start_move_target"),
                }),
                std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                {
                    std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<MoveCharacterEvent>(layer, navigatorID, "eating_cutscene_nav_move_target"),
                        std::make_shared<CharactersLookEvent>(layer,  std::vector<Character::CharacterID>{ navigatorID }, cookID, 2.0f),
                        std::make_shared<CharactersLookEvent>(layer, std::vector<Character::CharacterID>{ navigatorID }, fighterID, 3.0f),
                    }),
                    std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<TimerEvent>(3.0f),
                        std::make_shared<MoveCharacterEvent>(layer, cookID, "eating_cutscene_chef_move_target"),
                        std::make_shared<CharactersLookEvent>(layer, std::vector<Character::CharacterID>{ cookID }, fighterID, 1.0f),
                    }),
                    std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<MoveCharacterEvent>(layer, fighterID, "eating_cutscene_fighter_move_target"),
                        std::make_shared<TimerEvent>(9.0f),
                        std::make_shared<FunctionalEvent>(dropFighterEvent, nullptr, nullptr, nullptr),
                    }),
                }),
                std::make_shared<FunctionalEvent>(fighterStartMovingEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(2.0f),
                std::make_shared<FunctionalEvent>(cookFoodEventBegin, cookFoodEventEnd, nullptr, nullptr),
                std::make_shared<TimerEvent>(0.5f),
                std::make_shared<FunctionalEvent>(fighterEatFoodEventBegin, fighterEatFoodEventEnd, nullptr, nullptr),
                std::make_shared<TimerEvent>(3.0f),
            };
            return std::make_shared<Cutscene>(events);
        }

        static std::shared_ptr<Cutscene> createRevivingCutscene(LevelLayerInstance* layer, const Character::Controller* playerController)
        {
            Character::CharacterID navigatorID = playerController->GetCharacters<Character::NavigatorDwarf>(std::string()).front()->GetID();
            Character::CharacterID cookID = playerController->GetCharacters<Character::CookDwarf>(std::string()).front()->GetID();
            Character::CharacterID dropCharacterID = playerController->GetCharacters<Character::Character>([=](const Character::Character* character)
            {
                return character->GetID() != navigatorID && character->GetID() != cookID;
            }).front()->GetID();

            // Camera focus event
            auto focusCameraEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.Pan(layer->GetTriggerPosition("eating_cutscene_camera_start"));
            };

            auto navFaceDirectionEvent = [=]()
            {
                Character::Character* navigator = layer->GetCharacter<>(navigatorID);
                const Vector2f& dirTarget = layer->GetTriggerPosition("camera_start");

                navigator->SetRotation(dirTarget - navigator->GetPosition());
            };

            auto dropDwarfEvent = [=]()
            {
                Character::Character* fighter = layer->GetCharacter<>(dropCharacterID);
                const Vector2f& dropSource = layer->GetTriggerPosition("eating_cutscene_fighter_drop_target");

                Character::Damage dmg(Character::DamageType_Type_Fall, fighter->GetHealth().GetCurrent() * 0.9f);
                fighter->ApplyDamage(nullptr, fighter->GetPosition(), dmg);

                fighter->SetPosition(dropSource);
                fighter->ApplyLinearImpulse(Vector2f::Zero);
            };

            auto cookFoodEvent = [=]()
            {
                Character::Character* cook = layer->GetCharacter<>(cookID);
                Ability::Ability* ability = cook->GetAbility<Ability::ChannelingFoodHeal>();
                assert(ability);

                const Vector2f& target = layer->GetTriggerPosition("eating_cutscene_chef_channel_food_target");
                ability->SetPrimaryTarget(target);

                assert(ability->IsInValidState());
                cook->PushAction(ability->GenerateAction(), false);
                ability->ClearState();
            };

            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<FunctionalEvent>(focusCameraEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(1.0f),
                std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                {
                    std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<MoveCharacterEvent>(layer, navigatorID, "eating_cutscene_nav_move_target"),
                        std::make_shared<CharactersLookEvent>(layer, std::vector<Character::CharacterID>{ navigatorID }, cookID, 2.0f),
                        std::make_shared<CharactersLookEvent>(layer, std::vector<Character::CharacterID>{ navigatorID }, dropCharacterID, 3.0f),
                    }),
                    std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<TimerEvent>(2.0f),
                        std::make_shared<MoveCharacterEvent>(layer, cookID, "eating_cutscene_chef_move_target"),
                        std::make_shared<CharactersLookEvent>(layer, std::vector<Character::CharacterID>{ cookID }, dropCharacterID, 1.0f),
                    }),
                    std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<MoveCharacterEvent>(layer, dropCharacterID, "eating_cutscene_fighter_move_target"),
                        std::make_shared<TimerEvent>(8.0f),
                        std::make_shared<FunctionalEvent>(dropDwarfEvent, nullptr, nullptr, nullptr),
                    }),
                }),
                std::make_shared<MoveCharacterEvent>(layer, dropCharacterID, "eating_cutscene_fighter_downed_move_target"),
                std::make_shared<TimerEvent>(3.0f),
                std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                {
                    std::make_shared<InteractCharacterEvent>(layer, navigatorID, dropCharacterID),
                    std::make_shared<FunctionalEvent>(cookFoodEvent, nullptr, nullptr, nullptr),
                }),
                std::make_shared<TimerEvent>(2.0f),
            };
            return std::make_shared<Cutscene>(events);
        }

        void DwarfHome2::OnCreate()
        {
            LevelLayerInstance* primaryLayer = GetPrimaryLayer();

            SpawnCheckpoint<DwarfHome2Checkpoint>(primaryLayer->GetSpline("dwarf_spawn"), "dwarfhome2_spawn_checkpoint_name", true);
            SpawnCheckpoint<DwarfHome2Checkpoint>(primaryLayer->GetSpline("munitions_depot_checkpoint"), "dwarfhome2_munitions_depot_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome2Checkpoint>(primaryLayer->GetSpline("lab_checkpoint"), "dwarfhome2_lab_checkpoint_name", false);

            DwarfHomeLevel::OnCreate();

            SetDefaultEnvironmenType(Audio::EnvironmentType::None);

            std::shared_ptr<const DwarfHome2Checkpoint> checkpoint = GetCheckpoint<DwarfHome2Checkpoint>();

            SpawnTrackedDoor(primaryLayer, "main_door_lever_spawn", "main_door_door_spawn", "main_door_collision", "", Character::DoorSize_Medium, false, false, nullptr, checkpoint);
            SpawnTrackedDoor(primaryLayer, "bridge_left_lever_spawn", "bridge_left_door_spawn", "bridge_left_door_collision", "", Character::DoorSize_Medium, false, true, nullptr, checkpoint);
            SpawnTrackedDoor(primaryLayer, "bridge_right_lever_spawn", "bridge_right_door_spawn", "bridge_right_door_collision", "", Character::DoorSize_Medium, false, false, nullptr, checkpoint);
            SpawnTrackedDoor(primaryLayer, "bridge_bottom_left_lever_spawn", "bridge_bottom_left_door_spawn", "bridge_bottom_left_door_collision", "", Character::DoorSize_Medium, true, true, nullptr, checkpoint);
            SpawnTrackedDoor(primaryLayer, "bridge_bottom_right_lever_spawn", "bridge_bottom_right_door_spawn", "bridge_bottom_right_door_collision", "", Character::DoorSize_Medium, true, false, nullptr, checkpoint);

            Character::OnPortraitUseCallback leapCallback = [this](Character::Character*)
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();

                Character::AbilityPickup* leapPickup = primaryLayer->GetCharacter<Character::AbilityPickup>(_leapPickupID);

                // Find the closest fighter to do the cutscene
                Character::FighterDwarf* closestFighter = nullptr;
                float closestDist = std::numeric_limits<float>::max();

                const Character::Controller* playerController = GetPlayerController();
                std::vector<Character::FighterDwarf*> fighters = playerController->GetCharacters<Character::FighterDwarf>(std::string());
                for (Character::FighterDwarf* fighter : fighters)
                {
                    float dist = Vector2f::Distance(leapPickup->GetPosition(), fighter->GetPosition());
                    if (dist < closestDist)
                    {
                        closestFighter = fighter;
                        closestDist = dist;
                    }
                }

                assert(closestFighter);

                giveFightersLeap(playerController);

                PlayCutscene(createLeapCutscene(primaryLayer, GetMonsterController(), closestFighter->GetID()));

                _playedLeapCutscene = true;

                return true;
            };

            bool leapPickedUp = checkpoint != nullptr && checkpoint->WasLeapPickedUp();
            if (leapPickedUp)
            {
                giveFightersLeap(GetPlayerController());
            }

            Character::CharacterConstructor<Character::AbilityPickup> abilityPickupConstructor =
                Character::BindCharacterConstructor<Character::LeapPickup>(leapPickedUp, leapCallback);
            const Vector2f& leapPos = primaryLayer->GetTriggerPosition("leap_spawn");
            Character::AbilityPickup* leapPickup = primaryLayer->SpawnCharacter(leapPos, "leap", nullptr, abilityPickupConstructor);
            _leapPickupID = leapPickup->GetID();

            std::type_index brewerType = typeid(Character::BrewerDwarf);
            Character::OnPortraitUseCallback portraitCallback = [this](Character::Character*)
            {
                CompleteLevel(true);
                return true;
            };

            Character::CharacterConstructor<Character::Portrait> portraitConstructor =
                Character::BindCharacterConstructor<Character::Portrait>(brewerType, portraitCallback);
            const Vector2f& portraitPos = primaryLayer->GetTriggerPosition("portrait_spawn");
            const Vector2f& portraitDir = primaryLayer->GetTriggerPosition("portrait_dir");
            Character::Portrait* brewerPortrait = primaryLayer->SpawnCharacter(portraitPos, "portrait", nullptr, portraitConstructor);
            brewerPortrait->SetRotation(Rotatorf(portraitPos - portraitDir));

            bool bossAlive = (!checkpoint || !checkpoint->WasBossKilled());

            std::vector<Character::PotBrazier*> bossBrazier = SpawnCharactersAtTriggers("boss_brazier_spawn", Character::BindCharacterConstructor<Character::PotBrazier>(BaseFireColor, true, !bossAlive, true), "brazier", nullptr);
            assert(bossBrazier.size() == 1);
            _bossBrazierID = bossBrazier[0]->GetID();

            if (bossAlive)
            {
                Character::CharacterConstructor<Character::OrkWarrior> orkConstructor = Character::BindCharacterConstructor<Character::OrkWarrior>();
                std::vector<Character::OrkWarrior*> bosses = SpawnCharactersAtTriggers("boss_spawn", orkConstructor, "monster", GetMonsterController());
                assert(bosses.size() == 1);

                Character::OrkWarrior* boss = bosses[0];

                boss->AddAbility(Ability::BindAbilityConstructor<Ability::GroundSlam>());

                std::vector<Item::ItemConstructor<>> bossWeaps
                {
                    Item::BindItemConstructor<Item::OrkChopper>(),
                    Item::BindItemConstructor<Item::OrkClub>(),
                    Item::BindItemConstructor<Item::OrkHook>(),
                    Item::BindItemConstructor<Item::OrkMace>(),
                };
                boss->GiveItem(Random::RandomItem(bossWeaps));
                boss->GiveItem(Random::RandomItem(bossWeaps));
                _bossID = boss->GetID();
            }
            else
            {
                _bossID = 0;
                _playedBossCutscene = true;
            }

            std::vector<Character::CharacterID> bossLadder = SpawnLadders(primaryLayer, "boss_ladder_spawn");
            assert(bossLadder.size() == 1);
            _bossLadderID = bossLadder[0];

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

            // Spawn chests
            SpawnTrackedChest(primaryLayer, "secret_cave_chest_spawn", true, CaveItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "boss_area_chest_spawn", false, BossItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "secret_boss_area_chest_spawn", false, SecretBossItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "upper_cave_chest_spawn", true, UpperCaveChestItems(), checkpoint);

            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Small, "boss_area_rocks_spawn", "boss_area_rocks_collision", nullptr, checkpoint);
            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Large, "upper_cave_rocks_spawn", "upper_cave_rocks_collision", nullptr, checkpoint);

            giveChefChannelingHeal(GetPlayerController());
            _playedEatingCutscene = checkpoint != nullptr && checkpoint->DidPlayEatingCutscene();
            if (!_playedEatingCutscene)
            {
                // Need a chef and warrior to play the eating cutscene
                const Character::Controller* playerController = GetPlayerController();
                std::vector<Character::NavigatorDwarf*> navigators = playerController->GetCharacters<Character::NavigatorDwarf>(std::string());
                std::vector<Character::CookDwarf*> cooks = playerController->GetCharacters<Character::CookDwarf>(std::string());
                std::vector<Character::Character*> allCharacters = playerController->GetCharacters<Character::Character>(std::string());
                if (!navigators.empty() && !cooks.empty() && allCharacters.size() >= 3)
                {
                    PlayCutscene(createRevivingCutscene(primaryLayer, playerController));
                }

                _playedEatingCutscene = true;
            }

            _rain->SetRainDirection(DwarfHome2RainDir);
            _rain->SetRainSpeed(DwarfHome2RainSpeed.first, DwarfHome2RainSpeed.second);
            _rain->SetDropCount(DwarfHome2RainDropCount);
            _rain->SetLightningEnabled(DwarfHome2LightningEnabled);
            _rain->SetLightningStrikeInterval(DwarfHome2LightningStrikeInterval.first, DwarfHome2LightningStrikeInterval.second, DwarfHome2LightningDoubleStrikeChance);
        }

        void DwarfHome2::OnDestroy()
        {
            DwarfHomeLevel::OnDestroy();
        }

        void DwarfHome2::OnUpdate(double totalTime, float dt)
        {
            DwarfHomeLevel::OnUpdate(totalTime, dt);

            if (!_playedBossCutscene)
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();
                const Polygonf& triggerPoly = primaryLayer->GetTriggerArea("boss_cutscene_start_area");

                Character::Controller* playerController = GetPlayerController();

                std::vector<Character::Character*> charactersInBossArea =
                    playerController->GetCharacters<Character::Character>([&](const Character::Character* character)
                {
                    return Polygonf::Contains(triggerPoly, character->GetBounds().Middle());
                });

                if (!charactersInBossArea.empty())
                {
                    PlayCutscene(createBossEnterCutscene(primaryLayer, playerController, _bossID, _bossLadderID, _bossBrazierID));
                    _playedBossCutscene = true;
                }
            }

            RainEnvironment rainEnv =
                GetSoundManager()->GetCurrentEnvironment() == +Audio::EnvironmentType::None ? RainEnvironment::Open : RainEnvironment::Muffled;
            _rain->SetCurrentRainEnvironment(rainEnv);
            _rain->Update(totalTime, dt);
        }

        void DwarfHome2::OnLoadContent(Content::ContentManager* contentManager)
        {
            DwarfHomeLevel::OnLoadContent(contentManager);

            _rain->LoadContent(contentManager);
        }

        void DwarfHome2::OnUnloadContent()
        {
            DwarfHomeLevel::OnUnloadContent();

            _rain->UnloadContent();
        }

        void DwarfHome2::OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            DwarfHomeLevel::OnDraw(layer, levelRenderer);

            if (layer->GetName() == DwarfHome2RainLayer)
            {
                levelRenderer->AddDrawable(_rain.get(), false);
                levelRenderer->AddLight(_rain->GetLight());
            }
        }

        void DwarfHome2::OnSpawnCampaignCharacter(Character::Character* character)
        {
            DwarfHomeLevel::OnSpawnCampaignCharacter(character);

            character->SetRotation(GetPrimaryLayer()->GetTriggerPosition("spawn_character_look_dir") - character->GetPosition());
        }

        void DwarfHome2::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile)
        {
            DwarfHomeLevel::OnLevelSuccessfullyCompleted(profile);
            UnlockCharacters(profile, FindableCharacters());
            UnlockAbilities(profile, FindableAbilites());
            profile->UnlockCampaignLevel(Campaign::DwarfHomes3());
        }

        void DwarfHome2::PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> baseCheckpoint)
        {
            std::shared_ptr<DwarfHome2Checkpoint> checkpoint = AsA<DwarfHome2Checkpoint>(baseCheckpoint);

            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            checkpoint->SetBossKilled(_bossID == 0 || primaryLayer->GetCharacter(_bossID) == nullptr);
            checkpoint->SetLeapPickedUp(_playedLeapCutscene);
            checkpoint->SetPlayedEatingCutscene(_playedEatingCutscene);

            DwarfHomeLevel::PopulateCheckpoint(checkpoint);
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome2>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::DwarfHomeLevel>(preloads);

        EnumeratePreloads<RainEffect>(preloads);

        EnumeratePreloads<Character::OrkWarrior>(preloads);
        EnumeratePreloads<Item::OrkChopper>(preloads);
        EnumeratePreloads<Item::OrkClub>(preloads);
        EnumeratePreloads<Item::OrkHook>(preloads);
        EnumeratePreloads<Item::OrkMace>(preloads);

        EnumeratePreloads<Character::Portrait>(preloads);

        EnumeratePreloads<Character::LeapPickup>(preloads);

        EnumerateFindableAbilitySetPreloads(preloads, Level::FindableAbilites());
        EnumerateFindableItemSetPreloads(preloads, Level::CaveItems());
        EnumerateFindableItemSetPreloads(preloads, Level::BossItems());
        EnumerateFindableItemSetPreloads(preloads, Level::SecretBossItems());
        EnumerateFindableItemSetPreloads(preloads, Level::UpperCaveChestItems());

        EnumerateMusicTrackMapPreloads(preloads, Level::DwarfHome2MusicTracks);
    }
}
