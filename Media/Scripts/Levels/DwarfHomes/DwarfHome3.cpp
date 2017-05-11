#include "Levels/DwarfHomes/DwarfHome3.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/GrappleRope.hpp"
#include "Characters/Portrait.hpp"

#include "Characters/Worm.hpp"
#include "Characters/Ork.hpp"
#include "Characters/Gobbo.hpp"
#include "Characters/Gnome.hpp"

#include "Items/Weapons/OrkWeapons.hpp"

#include "Abilities/ShootFlare.hpp"
#include "Abilities/PlantDynamite.hpp"

#include "Levels/GameLevels.hpp"

#include "Audio/LavaSound.hpp"

#include "CutsceneUtility.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const Audio::MusicTrackMap DwarfHome3MusicTracks = Audio::MusicTrackMap
        {
        };

        static const auto EntrySecretChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Builder(),
                        {
                            { Item::Weapons::GoldHammer(), 1 }
                        }
                    },
                    {
                        Character::Dwarves::Miner(),
                        {
                            { Item::Weapons::GoldPickAxe(), 1 }
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

        static const auto HiddenNookChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Brewer(),
                        {
                            { Item::Weapons::BlunderbussRifle(), 1 }
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

        static const auto OverlookChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Cook(),
                        {
                            { Item::Weapons::ServingTools(), 1 }
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
                    {
                        Character::Dwarves::Brewer(),
                        {
                            { Item::Trinkets::BriarPipe(), 1 }
                        }
                    },
                },
            };
        };

        static const auto BottomNookChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Navigator(),
                        {
                            { Item::Weapons::FingerCannonPistol(), 1 }
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
                    {
                        Character::Dwarves::Navigator(),
                        {
                            { Item::Trinkets::Flashlight(), 1 }
                        }
                    },
                },
            };
        };

        static const auto EndChestItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Fighter(),
                        {
                            { Item::Weapons::TowerShield(), 1 }
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
                Character::Dwarves::Miner(),
                Character::Dwarves::Builder(),
            };
        };

        static const auto FindableAbilites = []()
        {
            return Ability::FindableAbilitySet
            {
                {
                    Character::Dwarves::Brewer(),
                    {
                        Ability::Abilities::PlantDynamite(),
                    },
                },
            };
        };

        DwarfHome3Checkpoint::DwarfHome3Checkpoint()
            : _dynamiteCutscenePlayed(false)
            , _bossAreaLightTurnedOn(false)
        {
        }

        DwarfHome3Checkpoint::~DwarfHome3Checkpoint()
        {
        }

        bool DwarfHome3Checkpoint::WasDynamiteCutscenePlayed() const
        {
            return _dynamiteCutscenePlayed;
        }

        void DwarfHome3Checkpoint::SetDynamiteCutscenePlayed(bool played)
        {
            _dynamiteCutscenePlayed = played;
        }

        bool DwarfHome3Checkpoint::WasBossAreaLightTurnedOn() const
        {
            return _bossAreaLightTurnedOn;
        }

        void DwarfHome3Checkpoint::SetBossAreaLightTurnedOn(bool turnedOn)
        {
            _bossAreaLightTurnedOn = turnedOn;
        }

        static const float BossAreaLightTurnOnTime = 2.0f;
        static const Color BossAreaLightColor = Color::FromBytes(255, 255, 255, 255);
        static const std::string BossAreaLightLayerName = "paralax_1";
        static const std::string FlameLightLayerName = "foreground";

        DwarfHome3::DwarfHome3(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : DwarfHomeLevel(parameters, campaignParameters)
            , _fighterPortraitID(0)
            , _playedDynamiteCutscene(false)
            , _bossAreaLightOn(false)
            , _bossAreaLightOnTime(0.0f)
            , _bossAreaLight()
        {
            AddMusicTracks(DwarfHome3MusicTracks);
        }

        DwarfHome3::~DwarfHome3()
        {
        }

        CampaignLevelInfo DwarfHome3::GetCampaignLevelInfo()
        {
            CampaignLevelInfo info;
            info.NameCode = "dwarfhome3_name";
            info.MaxTotalCharacters = 4;
            info.MinTypedCharacters[typeid(Character::NavigatorDwarf)] = 1;
            //info.MinTypedCharacters[typeid(Character::FighterDwarf)] = 1;
            //info.MinTypedCharacters[typeid(Character::CookDwarf)] = 1;
            info.MinTypedCharacters[typeid(Character::BrewerDwarf)] = 1;
            info.StartingResources = Item::Resources(0);

            info.FindableCharacters = FindableCharacters();
            info.FindableItems = Item::MergeFindableItems(EntrySecretChestItems(), HiddenNookChestItems(), OverlookChestItems(), BottomNookChestItems(), EndChestItems());
            info.FindableAbilities = FindableAbilites();

            return info;
        }

        static void giveBrewerPlantDynamite(const Character::Controller* playerController)
        {
            Ability::AbilityConstructor<> abilityConstructor = Ability::GameAbility::GetConstructor(Ability::Abilities::PlantDynamite());
            std::vector<Character::BrewerDwarf*> brewers = playerController->GetCharacters<Character::BrewerDwarf>(std::string());
            for (Character::BrewerDwarf* brewer : brewers)
            {
                if (brewer->GetAbility<Ability::PlantDynamite>() == nullptr)
                {
                    brewer->AddAbility(abilityConstructor);
                }
            }
        }

        void DwarfHome3::OnCreate()
        {
            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            SpawnCheckpoint<DwarfHome3Checkpoint>(primaryLayer->GetSpline("dwarf_spawn"), "dwarfhome3_spawn_checkpoint_name", true);
            SpawnCheckpoint<DwarfHome3Checkpoint>(primaryLayer->GetSpline("hidden_nook_checkpoint"), "dwarfhome3_hidden_nook_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome3Checkpoint>(primaryLayer->GetSpline("roundabout_checkpoint"), "dwarfhome3_roundabout_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome3Checkpoint>(primaryLayer->GetSpline("blocked_passage_checkpoint"), "dwarfhome3_blocked_passage_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome3Checkpoint>(primaryLayer->GetSpline("burning_tracks_checkpoint"), "dwarfhome3_burning_tracks_checkpoint_name", false);

            DwarfHomeLevel::OnCreate();

            SetDefaultEnvironmenType(Audio::EnvironmentType::None);

            std::shared_ptr<const DwarfHome3Checkpoint> checkpoint = GetCheckpoint<DwarfHome3Checkpoint>();

            // Spawn worms
            Character::CharacterConstructor<Character::Worm> wormConstructor = Character::BindCharacterConstructor<Character::Worm>();
            SpawnTrackedCharactersAtTriggers("worm_spawn", wormConstructor, "werm", GetWildlifeController(), checkpoint);

            // Spawn bramble
            Character::CharacterConstructor<Character::GobboBramble> brambleConstructor = Character::BindCharacterConstructor<Character::GobboBramble>();
            SpawnTrackedCharactersAtTriggers("bramble_spawn", brambleConstructor, "bramble", GetMonsterController(), checkpoint);

            // Spawn orks
            std::vector<Item::ItemConstructor<>> orkWeaps
            {
                Item::BindItemConstructor<Item::OrkChopper>(),
                Item::BindItemConstructor<Item::OrkClub>(),
                Item::BindItemConstructor<Item::OrkHook>(),
                Item::BindItemConstructor<Item::OrkMace>(),
            };

            Character::CharacterConstructor<Character::OrkWarrior> orkConstructor = Character::BindCharacterConstructor<Character::OrkWarrior>();
            std::vector<Character::OrkWarrior*> orks = SpawnTrackedCharactersAtTriggers("ork_spawn", orkConstructor, "ork", GetMonsterController(), checkpoint);
            for (auto ork : orks)
            {
                ork->GiveItem(Random::RandomItem(orkWeaps));
                ork->GiveItem(Random::RandomItem(orkWeaps));
            }

            // Spawn gnomes
            std::vector<Item::ItemConstructor<>> gnomeWeaps
            {
                Item::BindItemConstructor<Item::GnomeWrench>(),
            };

            Character::CharacterConstructor<Character::Gnome> gnomeConstructor = Character::BindCharacterConstructor<Character::Gnome>();
            std::vector<Character::Gnome*> gnomes = SpawnTrackedCharactersAtTriggers("gnome_spawn", gnomeConstructor, "gnome", GetMonsterController(), checkpoint);
            for (auto gnome : gnomes)
            {
                gnome->GiveItem(Random::RandomItem(gnomeWeaps));
            }

            // Spawn boss
            Character::CharacterConstructor<Character::ShieldOrk> bossConstructor = Character::BindCharacterConstructor<Character::ShieldOrk>();
            std::vector<Character::ShieldOrk*> bosses = SpawnTrackedCharactersAtTriggers("boss_spawn", bossConstructor, "boss", GetMonsterController(), checkpoint);
            if (bosses.empty())
            {
                _playedBossCutscene = true;
            }
            else
            {
                assert(bosses.size() == 1);
                Character::ShieldOrk* boss = bosses.front();

                const Vector2f& bossLookDir = primaryLayer->GetTriggerPosition("boss_look_dir");
                boss->SetRotation(Rotatorf(boss->GetPosition() - bossLookDir));

                _boss = boss->GetID();
            }

            // Spawn flames
            const Polygonf& flameSizer = primaryLayer->GetTriggerArea("flame_sizer");
            float flameRadius = flameSizer.Bounds().H;

            std::vector<Vector2f> flameSpawns = primaryLayer->GetTriggerPositions("flame_spawn");
            for (auto flameSpawn : flameSpawns)
            {
                Graphics::Flame* flame = new Graphics::Flame(BaseFireColor, false, true, false, false, GetSoundManager());
                flame->SetPosition(flameSpawn);
                flame->SetRadius(flameRadius);
                flame->SetBrightnessRange(0.5f, 1.0f);
                _lavaFlames.push_back(std::unique_ptr<Graphics::Flame>(flame));
            }

            _playedDynamiteCutscene = checkpoint != nullptr && checkpoint->WasDynamiteCutscenePlayed();
            if (_playedDynamiteCutscene)
            {
                giveBrewerPlantDynamite(GetPlayerController());
            }
            _bossAreaLightOn = checkpoint != nullptr && checkpoint->WasBossAreaLightTurnedOn();

            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Medium, "entry_rocks_spawn", "entry_rocks_collision", nullptr, checkpoint);
            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Medium, "entry_secret_rocks_spawn", "entry_secret_rocks_collision", nullptr, checkpoint);
            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Large, "rocks0_spawn", "rocks0_collision", nullptr, checkpoint);
            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Large, "ladder_rocks_spawn", "ladder_rocks_collision", [this]() { _bossAreaLightOn = true; _lavaSound->Play(BossAreaLightTurnOnTime); }, checkpoint);
            SpawnTrackedRubble(primaryLayer, Character::RubbleSize_Large, "bottom_nook_rocks_spawn", "bottom_nook_rocks_collision", nullptr, checkpoint);

            LevelLayerInstance* bossLightLayer = GetLayer(BossAreaLightLayerName);
            _bossAreaLight.Area = bossLightLayer->GetTriggerArea("boss_area_light");
            _bossAreaLight.LightColor = Color::FromBytes(0, 0, 0, 255);

            Chainf lavaSoundChain(primaryLayer->GetSpline("lava_sound").Points());
            const Vector2f& lavaSoundBase = primaryLayer->GetTriggerPosition("lava_sound_base");
            const Vector2f& lavaSoundMin = primaryLayer->GetTriggerPosition("lava_sound_min");
            const Vector2f& lavaSoundMax = primaryLayer->GetTriggerPosition("lava_sound_max");
            Audio::LavaSound* lavaSound = new Audio::LavaSound(GetContentManager(), GetSoundManager(), lavaSoundChain, Vector2f::Distance(lavaSoundBase, lavaSoundMin), Vector2f::Distance(lavaSoundBase, lavaSoundMax));
            _lavaSound = std::unique_ptr<Audio::LavaSound>(lavaSound);
            if (_bossAreaLightOn)
            {
                _lavaSound->Play(0.0f);
            }

            // Spawn chests
            SpawnTrackedChest(primaryLayer, "entry_secret_chest_spawn", false, EntrySecretChestItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "hidden_nook_chest_spawn", false, HiddenNookChestItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "overlook_chest_spawn", true, OverlookChestItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "bottom_nook_chest_spawn", false, BottomNookChestItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "end_chest_spawn", true, EndChestItems(), checkpoint);

            // Spawn portraits
            const Vector2f& portraitDir = primaryLayer->GetTriggerPosition("portrait_dir");
            const Vector2f& builderPortraitSpawnPos = primaryLayer->GetTriggerPosition("portrait0_spawn");
            const Vector2f& minerPortraitSpawnPos = primaryLayer->GetTriggerPosition("portrait1_spawn");

            Character::CharacterConstructor<Character::Portrait> builderPortraitConstructor =
                Character::BindCharacterConstructor<Character::Portrait>(std::type_index(typeid(Character::BuilderDwarf)), [this](Character::Character* character)
            {
                _builderPortraitUsed = true;
                if (_minerPortraitUsed)
                {
                    CompleteLevel(true);
                }
                return true;
            });
            Character::Portrait* builderPortrait = primaryLayer->SpawnCharacter(builderPortraitSpawnPos, "portrait0", nullptr, builderPortraitConstructor);
            builderPortrait->SetRotation(Rotatorf(builderPortraitSpawnPos - portraitDir));

            Character::CharacterConstructor<Character::Portrait> minerPortraitConstructor =
                Character::BindCharacterConstructor<Character::Portrait>(std::type_index(typeid(Character::MinerDwarf)), [this](Character::Character* character)
            {
                _minerPortraitUsed = true;
                if (_builderPortraitUsed)
                {
                    CompleteLevel(true);
                }
                return true;
            });
            Character::Portrait* minerPortrait = primaryLayer->SpawnCharacter(minerPortraitSpawnPos, "portrait1", nullptr, minerPortraitConstructor);
            minerPortrait->SetRotation(Rotatorf(minerPortraitSpawnPos - portraitDir));
        }

        static bool meetsDynamiteCutsceneCriteria(LevelLayerInstance* layer, const Character::Controller* playerController)
        {
            return !playerController->GetCharacters<Character::BrewerDwarf>().empty() && !playerController->GetCharacters<Character::NavigatorDwarf>().empty();
        }

        static std::shared_ptr<Cutscene> createDynamiteCutscene(LevelLayerInstance* layer, const Character::Controller* playerController)
        {
            assert(meetsDynamiteCutsceneCriteria(layer, playerController));

            Character::CharacterID brewerID = playerController->GetCharacters<Character::BrewerDwarf>().front()->GetID();
            Character::CharacterID navigatorID = playerController->GetCharacters<Character::NavigatorDwarf>().front()->GetID();

            std::vector<Character::CharacterID> otherCharacterIDs;
            for (auto character : playerController->GetCharacters<Character::Character>())
            {
                auto id = character->GetID();
                if (id != brewerID && id != navigatorID)
                {
                    otherCharacterIDs.push_back(id);
                }
            }

            // Camera focus event
            auto focusCameraEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.Pan(layer->GetTriggerPosition("dynamite_cutscene_camera_pos"), 2.0f);
            };

            auto plantDynamiteEvent = [=]()
            {
                Character::Character* brewer = layer->GetCharacter<>(brewerID);
                Ability::PlantDynamite* dynamiteAbility = brewer->GetAbility<Ability::PlantDynamite>();
                dynamiteAbility->SetPrimaryTarget(layer->GetTriggerPosition("dynamite_cutscene_drop_pos"));
                assert(dynamiteAbility->IsInValidState());
                brewer->PushAction(dynamiteAbility->GenerateAction(), false);
                dynamiteAbility->ClearState();
            };
            auto plantDynamiteFinishedEvent = [=]()
            {
                Character::Character* brewer = layer->GetCharacter<>(brewerID);
                return brewer->GetCurrentState() == Character::CharacterState_Idle;
            };

            auto aimFlareEvent = [=]()
            {
                Character::Character* navigator = layer->GetCharacter<>(navigatorID);
                Ability::ShootFlare* flareAbility = navigator->GetAbility<Ability::ShootFlare>();
                flareAbility->SetPrimaryTarget(layer->GetTriggerPosition("dynamite_cutscene_flare_aim_pos"));
            };

            auto shootFlareEvent = [=]()
            {
                Character::Character* navigator = layer->GetCharacter<>(navigatorID);
                Ability::ShootFlare* flareAbility = navigator->GetAbility<Ability::ShootFlare>();
                flareAbility->SetPrimaryTarget(layer->GetTriggerPosition("dynamite_cutscene_flare_aim_pos"));
                assert(flareAbility->IsInValidState());
                navigator->PushAction(flareAbility->GenerateAction(), false);
                flareAbility->ClearState();
            };

            std::vector<std::shared_ptr<CutsceneEvent>> moveEvents
            {
                std::make_shared<MoveCharacterEvent>(layer, navigatorID, "dynamite_cutscene_move_pos"),
            };

            const Splinef& otherCharacterMovePos = layer->GetSpline("dynamite_cutscene_other_character_move_pos");
            for (uint32_t i = 0; i < otherCharacterIDs.size(); i++)
            {
                float perc = i / static_cast<float>(otherCharacterIDs.size() - 1);
                moveEvents.push_back(std::make_shared<MoveCharacterEvent>(layer, otherCharacterIDs[i], otherCharacterMovePos.Evalulate(perc)));
            }

            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<FunctionalEvent>(focusCameraEvent, nullptr, nullptr, nullptr),
                std::make_shared<SimultaneousEvents>(moveEvents),
                std::make_shared<FunctionalEvent>(plantDynamiteEvent, plantDynamiteFinishedEvent, nullptr, nullptr),
                std::make_shared<FunctionalEvent>(aimFlareEvent, nullptr, nullptr, nullptr),
                std::make_shared<MoveCharacterEvent>(layer, brewerID, "dynamite_cutscene_retreat_pos"),
                std::make_shared<CharactersLookEvent>(layer, std::vector<Character::CharacterID>{ brewerID }, layer->GetTriggerPosition("dynamite_cutscene_drop_pos"), 0.0f),
                std::make_shared<FunctionalEvent>(shootFlareEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(3.0f),
            };

            return std::make_shared<Cutscene>(events);
        }

        static bool meetsBossCutsceneCriteria(LevelLayerInstance* layer, const Character::Controller* playerController, Character::CharacterID bossId)
        {
            if (layer->GetCharacter(bossId) == nullptr)
            {
                return false;
            }

            const auto& poly = layer->GetTriggerArea("boss_cutscene_start");
            for (auto dwarf : playerController->GetCharacters<Character::Dwarf>())
            {
                if (Polygonf::Contains(poly, dwarf->GetPosition()))
                {
                    return true;
                }
            }

            return false;
        }

        static std::shared_ptr<Cutscene> createBossCutscene(LevelLayerInstance* layer, const Character::Controller* playerController, Character::CharacterID bossId)
        {
            assert(meetsBossCutsceneCriteria(layer, playerController, bossId));

            auto stopDwarves = [=]()
            {
                for (auto dwarf : playerController->GetCharacters<Character::Dwarf>())
                {
                    dwarf->StopMoving();
                }
            };

            // Camera focus event
            auto focusCameraEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.Pan(layer->GetTriggerPosition("boss_cutscene_camera_target"), 2.0f);
            };

            auto teleportDwarves = [=]()
            {
                for (auto dwarf : playerController->GetCharacters<Character::Dwarf>())
                {
                    const auto& poly = layer->GetTriggerArea("boss_cutscene_battle_area");
                    const Vector2f& teleportTarget = layer->GetTriggerPosition("boss_cutscene_dwarf_teleport_area");
                    if (!Polygonf::Contains(poly, dwarf->GetPosition()))
                    {
                        dwarf->SetPosition(teleportTarget);
                    }
                }
            };

            auto killGrapples = [=]()
            {
                const auto& poly = layer->GetTriggerArea("boss_cutscene_grapple_kill_area");
                for (auto grapple : layer->GetCharacters<Character::GrappleRope>([&](const Character::GrappleRope* rope) { return !Polygonf::Contains(poly, rope->GetPosition()); }))
                {
                    grapple->Kill(nullptr);
                    grapple->Terminate();
                }
            };

            auto awakenBossEvent = [=]()
            {
                Character::ShieldOrk* boss = layer->GetCharacter<Character::ShieldOrk>(bossId);
                assert(boss);

                boss->StandUp();
            };

            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<FunctionalEvent>(stopDwarves, nullptr, nullptr, nullptr),
                std::make_shared<FunctionalEvent>(focusCameraEvent, nullptr, nullptr, nullptr),
                std::make_shared<FunctionalEvent>(teleportDwarves, nullptr, nullptr, nullptr),
                std::make_shared<FunctionalEvent>(killGrapples, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(1.0f),
                std::make_shared<FunctionalEvent>(awakenBossEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(8.0f),
            };

            return std::make_shared<Cutscene>(events);
        }

        void DwarfHome3::OnUpdate(double totalTime, float dt)
        {
            DwarfHomeLevel::OnUpdate(totalTime, dt);

            if (!_playedDynamiteCutscene)
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();
                const Polygonf& triggerPoly = primaryLayer->GetTriggerArea("dynamite_cutscene_start_area");

                Character::Controller* playerController = GetPlayerController();

                std::vector<Character::Character*> charactersInBossArea =
                    playerController->GetCharacters<Character::Character>([&](const Character::Character* character)
                {
                    return Polygonf::Contains(triggerPoly, character->GetBounds().Middle());
                });

                if (!charactersInBossArea.empty() && meetsDynamiteCutsceneCriteria(primaryLayer, playerController))
                {
                    giveBrewerPlantDynamite(GetPlayerController());
                    PlayCutscene(createDynamiteCutscene(primaryLayer, playerController));
                    _playedDynamiteCutscene = true;
                }
            }

            if (_bossAreaLightOn)
            {
                _bossAreaLightOnTime += dt;
                float bossAreaLightPerc = Clamp(_bossAreaLightOnTime / BossAreaLightTurnOnTime, 0.0f, 1.0f);
                _bossAreaLight.LightColor = BossAreaLightColor * bossAreaLightPerc;
                _bossAreaLight.LightColor.A = 255;

                for (auto &flame : _lavaFlames)
                {
                    flame->Update(totalTime, dt);
                }
            }

            if (!_playedBossCutscene)
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();
                Character::Controller* playerController = GetPlayerController();
                if (meetsBossCutsceneCriteria(primaryLayer, playerController, _boss))
                {
                    PlayCutscene(createBossCutscene(primaryLayer, playerController, _boss));
                    _playedBossCutscene = true;
                }
            }

            _lavaSound->Update(totalTime, dt, GetCameraController().GetCamera());
        }

        void DwarfHome3::OnSpawnCampaignCharacter(Character::Character* character)
        {
            DwarfHomeLevel::OnSpawnCampaignCharacter(character);

            character->SetRotation(GetPrimaryLayer()->GetTriggerPosition("spawn_character_look_dir") - character->GetPosition());
        }

        void DwarfHome3::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile)
        {
            DwarfHomeLevel::OnLevelSuccessfullyCompleted(profile);
            UnlockCharacters(profile, FindableCharacters());
            UnlockAbilities(profile, FindableAbilites());
            profile->UnlockCampaignLevel(Campaign::DwarfHomes4());
        }

        void DwarfHome3::PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> baseCheckpoint)
        {
            std::shared_ptr<DwarfHome3Checkpoint> checkpoint = AsA<DwarfHome3Checkpoint>(baseCheckpoint);

            checkpoint->SetDynamiteCutscenePlayed(_playedDynamiteCutscene);
            checkpoint->SetBossAreaLightTurnedOn(_bossAreaLightOn);

            DwarfHomeLevel::PopulateCheckpoint(checkpoint);
        }

        void DwarfHome3::OnLoadContent(Content::ContentManager* contentManager)
        {
            DwarfHomeLevel::OnLoadContent(contentManager);

            for (auto &flame : _lavaFlames)
            {
                flame->LoadContent(contentManager);
            }
        }

        void DwarfHome3::OnUnloadContent()
        {
            DwarfHomeLevel::OnUnloadContent();

            for (auto &flame : _lavaFlames)
            {
                flame->UnloadContent();
            }
        }

        void DwarfHome3::OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            DwarfHomeLevel::OnDraw(layer, levelRenderer);

            if (_bossAreaLightOn)
            {
                if (layer->GetName() == BossAreaLightLayerName)
                {
                    levelRenderer->AddLight(_bossAreaLight);
                }


                if (layer->GetName() == FlameLightLayerName)
                {
                    for (auto &flame : _lavaFlames)
                    {
                        levelRenderer->AddLight(flame->GetMainLight());
                        levelRenderer->AddLight(flame->GetCenterLight());
                    }
                }
            }
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome3>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::DwarfHomeLevel>(preloads);

        EnumeratePreloads<Character::Portrait>(preloads);

        EnumeratePreloads<Character::Gnome>(preloads);
        EnumeratePreloads<Item::GnomeWrench>(preloads);

        EnumeratePreloads<Character::Worm>(preloads);

        EnumeratePreloads<Character::OrkWarrior>(preloads);
        EnumeratePreloads<Item::OrkChopper>(preloads);
        EnumeratePreloads<Item::OrkClub>(preloads);
        EnumeratePreloads<Item::OrkHook>(preloads);
        EnumeratePreloads<Item::OrkMace>(preloads);

        EnumeratePreloads<Character::ShieldOrk>(preloads);

        EnumeratePreloads<Character::GobboBramble>(preloads);

        EnumeratePreloads<Graphics::Flame>(preloads);

        EnumerateFindableAbilitySetPreloads(preloads, Level::FindableAbilites());
        EnumerateFindableItemSetPreloads(preloads, Level::EntrySecretChestItems());
        EnumerateFindableItemSetPreloads(preloads, Level::HiddenNookChestItems());
        EnumerateFindableItemSetPreloads(preloads, Level::OverlookChestItems());
        EnumerateFindableItemSetPreloads(preloads, Level::BottomNookChestItems());
        EnumerateFindableItemSetPreloads(preloads, Level::EndChestItems());

        EnumerateMusicTrackMapPreloads(preloads, Level::DwarfHome3MusicTracks);
    }
}
