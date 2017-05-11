#include "Levels/DwarfHomes/DwarfHome0.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Portrait.hpp"

#include "Levels/GameLevels.hpp"

#include "Abilities/ShootFlare.hpp"

#include "CutsceneUtility.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const bool EnableIntroCutscene = false;

        static const Audio::MusicTrackMap DwarfHome0MusicTracks = Audio::MusicTrackMap
        {
            {
                "music_bass_bells",
                {
                    "Music/DwarfHome/MusicTrack_FirstDig_bassBells.ogg",
                    { 0.05f, 0.3f },
                },
            },
            {
                "music_guitar_bells",
                {
                    "Music/DwarfHome/MusicTrack_FirstDig_guitarBells.ogg",
                    { 0.05f, 0.3f },
                },
            },
            {
                "music_whistling",
                {
                    "Music/DwarfHome/MusicTrack_FirstDig_whistling.ogg",
                    { 0.05f, 0.2f },
                },
            },
        };

        static const char* GoodSwellName = "good_swell";
        static const char* BadSwellName = "bad_swell";
        static const Audio::MusicSwellMap DwarfHome0MusicSwells = Audio::MusicSwellMap
        {
            {
                GoodSwellName,
                {
                    "Music/IntroSwell/MusicIncidental_IntroSwell_Dmaj.ogg",
                    0.3f,
                },
            },
            {
                BadSwellName,
                {
                    "Music/IntroSwell/MusicIncidental_IntroSwell_B.ogg",
                    0.3f,
                },
            },
        };

        static const auto StorageRoomItems = []()
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
                        Character::Dwarves::Navigator(),
                        {
                            { Item::Trinkets::Lantern(), 1 }
                        }
                    },
                },
            };
        };

        static const auto FindableAbilites = []()
        {
            return Ability::FindableAbilitySet
            {
                {
                    Character::Dwarves::Navigator(),
                    {
                        Ability::Abilities::ShootFlare(),
                    }
                },
            };
        };

        static const auto FindableCharacters = []()
        {
            return Character::FindableCharacterSet
            {
                Character::Dwarves::Fighter(),
            };
        };

        DwarfHome0Checkpoint::DwarfHome0Checkpoint()
            : _flareCutscenePlayed(false)
        {
        }

        DwarfHome0Checkpoint::~DwarfHome0Checkpoint()
        {
        }

        bool DwarfHome0Checkpoint::WasFlareCutscenePlayed() const
        {
            return _flareCutscenePlayed;
        }

        void DwarfHome0Checkpoint::SetFlareCutscenePlayed(bool played)
        {
            _flareCutscenePlayed = played;
        }

        DwarfHome0::DwarfHome0(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : DwarfHomeLevel(parameters, campaignParameters)
            , _playedFlareCutscene(false)
        {
            AddMusicTracks(DwarfHome0MusicTracks);
            AddMusicSwells(DwarfHome0MusicSwells);
        }

        DwarfHome0::~DwarfHome0()
        {
        }

        CampaignLevelInfo DwarfHome0::GetCampaignLevelInfo()
        {
            CampaignLevelInfo info;
            info.NameCode = "dwarfhome0_name";
            info.MaxTotalCharacters = 1;
            info.MinTypedCharacters[typeid(Character::NavigatorDwarf)] = 1;
            info.StartingResources = Item::Resources(0);

            info.FindableCharacters = FindableCharacters();
            info.FindableItems = StorageRoomItems();
            info.FindableAbilities = FindableAbilites();

            return info;
        }

        static std::shared_ptr<Cutscene> createIntroCutscene(LevelLayerInstance* layer)
        {
            const Splinef& panSpline = layer->GetSpline("camera_pan");
            const float splineFollowTime = 10.0f;
            const float originalZoom = layer->GetLevel()->GetCameraController().GetCamera().GetZoom();
            const float outterZoom = originalZoom * 0.25f;

            Splinef zoomSpline;
            zoomSpline.AddPoint(Vector2f(0.0f, originalZoom));
            zoomSpline.AddPoint(Vector2f(0.6f, outterZoom));
            zoomSpline.AddPoint(Vector2f(1.0f, originalZoom));

            // Camera focus event
            auto focusCameraEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.Pan(panSpline.Evalulate(0.0f));
            };

            // Spline follow event
            auto camFollowSplineEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.FollowSpline(panSpline, splineFollowTime);
            };

            auto camZoomEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.FollowZoomSpline(zoomSpline, splineFollowTime);
            };

            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<FunctionalEvent>(focusCameraEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(3.0f),
                std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                {
                    std::make_shared<TimedFunctionalEvent>(splineFollowTime, camFollowSplineEvent, nullptr, nullptr),
                    std::make_shared<TimedFunctionalEvent>(splineFollowTime, camZoomEvent, nullptr, nullptr),
                }),
            };
            return std::make_shared<Cutscene>(events);
        }

        static void giveNavigatorsShootFlare(const Character::Controller* playerController)
        {
            Ability::AbilityConstructor<> abilityConstructor = Ability::GameAbility::GetConstructor(Ability::Abilities::ShootFlare());
            std::vector<Character::NavigatorDwarf*> navigators = playerController->GetCharacters<Character::NavigatorDwarf>(std::string());
            for (Character::NavigatorDwarf* navigator : navigators)
            {
                if (navigator->GetAbility<Ability::ShootFlare>() == nullptr)
                {
                    navigator->AddAbility(abilityConstructor);
                }
            }
        }

        static std::shared_ptr<Cutscene> createFlareCutscene(LevelLayerInstance* layer, Character::CharacterID navigatorID,
                                                             Character::CharacterID flarePickupID, bool pickupFlares)
        {
            // Aim event
            const Splinef& aimSpline = layer->GetSpline("flare_cutscene_aim_spline");
            struct aimEvent : public CutsceneEvent
            {
                const float _aimDuration = 2.0f;

                aimEvent(LevelLayerInstance* layer, Character::CharacterID navigatorID, const Splinef& aimSpline)
                    : _timer(_aimDuration)
                    , _layer(layer)
                    , _navigatorID(navigatorID)
                    , _aimSpline(aimSpline)
                    , _finished(false)
                {
                }

                virtual bool IsFinished() const override
                {
                    return _finished;
                }

                virtual void Update(double totalTime, float dt) override
                {
                    _timer -= dt;
                    float perc = 1.0f - Clamp(_timer / _aimDuration, 0.0f, 1.0f);

                    Vector2f lookTarget = _aimSpline.Evalulate(1.0f);
                    Character::NavigatorDwarf* character = _layer->GetCharacter<Character::NavigatorDwarf>(_navigatorID);
                    character->LookAt(lookTarget);

                    Ability::ShootFlare* ability = character->GetAbility<Ability::ShootFlare>();
                    assert(ability);
                    Vector2f target = _aimSpline.Evalulate(perc);
                    ability->SetPrimaryTarget(target);

                    if (_timer < 0.0f)
                    {
                        assert(ability->IsInValidState());
                        character->PushAction(ability->GenerateAction(), false);
                        ability->ClearState();
                        _finished = true;
                    }
                }

                float _timer;

                LevelLayerInstance* _layer;
                Character::CharacterID _navigatorID;
                const Splinef& _aimSpline;

                bool _finished;
            };

            std::vector<std::shared_ptr<CutsceneEvent>> initalMovementEvents;
            if (pickupFlares)
            {
                initalMovementEvents.push_back(std::make_shared<InteractCharacterEvent>(layer, navigatorID, flarePickupID));
            }
            initalMovementEvents.push_back(std::make_shared<MoveCharacterEvent>(layer, navigatorID, "flare_cutscene_move_pos"));

            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                {
                    std::make_shared<FocusCameraEvent>(layer, "flare_cutscene_camera_focus_pos", 1.5f),
                    std::make_shared<SeriesEvent>(initalMovementEvents),
                }),
                std::make_shared<TimerEvent>(0.5f),
                std::make_shared<aimEvent>(layer, navigatorID, aimSpline),
                std::make_shared<TimerEvent>(2.0f),
            };
            return std::make_shared<Cutscene>(events);
        }

        void DwarfHome0::OnCreate()
        {
            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            SpawnCheckpoint<DwarfHome0Checkpoint>(primaryLayer->GetSpline("dwarf_spawn"), "dwarfhome0_spawn_checkpoint_name", true);

            DwarfHomeLevel::OnCreate();

            std::shared_ptr<const DwarfHome0Checkpoint> checkpoint = GetCheckpoint<DwarfHome0Checkpoint>();

            std::type_index fighterType = typeid(Character::FighterDwarf);
            Character::OnPortraitUseCallback portraitCallback = std::bind(&DwarfHome0::onPortraitUsed, this, std::placeholders::_1);
            Character::CharacterConstructor<Character::Portrait> portraitConstructor =
                Character::BindCharacterConstructor<Character::Portrait>(fighterType, portraitCallback);
            const Vector2f& portraitPos = primaryLayer->GetTriggerPosition("dwarf_portrait");

            primaryLayer->SpawnCharacter(portraitPos, "portrait", nullptr, portraitConstructor);

            if (checkpoint != nullptr)
            {
                _playedFlareCutscene = checkpoint->WasFlareCutscenePlayed();
                if (_playedFlareCutscene)
                {
                    giveNavigatorsShootFlare(GetPlayerController());
                }
            }
            else
            {
                if (EnableIntroCutscene)
                {
                    PlayCutscene(createIntroCutscene(primaryLayer));
                }
                PlayMusicSwell(GoodSwellName);
            }

            Character::CharacterConstructor<Character::FlarePickup> abilityPickupConstructor =
                Character::BindCharacterConstructor<Character::FlarePickup>(_playedFlareCutscene, [this](Character::Character* character)
            {
                assert(IsA<Character::NavigatorDwarf>(character));
                if (!_playedFlareCutscene)
                {
                    _playedFlareCutscene = true;
                    giveNavigatorsShootFlare(GetPlayerController());
                    PlayCutscene(createFlareCutscene(GetPrimaryLayer(), character->GetID(), _flarePickupID, false));
                }
                return true;
            });
            const Vector2f& flarePickupPos = primaryLayer->GetTriggerPosition("flare_pickup_spawn");
            Character::FlarePickup* flarePickup = primaryLayer->SpawnCharacter(flarePickupPos, "flare", nullptr, abilityPickupConstructor);
            _flarePickupID = flarePickup->GetID();

            // Swells
            AddTrackedTriggeredMusicSwell("swell_first_monster", BadSwellName, checkpoint);
            AddTrackedTriggeredMusicSwell("swell_chest", GoodSwellName, checkpoint);

            // Spawn chests
            SpawnTrackedChest(primaryLayer, "storage_room_chest_spawn", false, StorageRoomItems(), checkpoint);
        }

        void DwarfHome0::OnUpdate(double totalTime, float dt)
        {
            DwarfHomeLevel::OnUpdate(totalTime, dt);

            if (!_playedFlareCutscene)
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();
                const Polygonf& triggerPoly = primaryLayer->GetTriggerArea("flare_cutscene_start_area");

                Character::Controller* playerController = GetPlayerController();

                std::vector<Character::NavigatorDwarf*> charactersInShootFlareArea =
                    playerController->GetCharacters<Character::NavigatorDwarf>([&](const Character::NavigatorDwarf* character)
                {
                    return Polygonf::Contains(triggerPoly, character->GetBounds().Middle());
                });

                if (!charactersInShootFlareArea.empty())
                {
                    _playedFlareCutscene = true;
                    giveNavigatorsShootFlare(GetPlayerController());
                    PlayCutscene(createFlareCutscene(primaryLayer, charactersInShootFlareArea[0]->GetID(), _flarePickupID, true));
                }
            }
        }

        void DwarfHome0::OnSpawnCampaignCharacter(Character::Character* character)
        {
            DwarfHomeLevel::OnSpawnCampaignCharacter(character);

            character->SetRotation(GetPrimaryLayer()->GetTriggerPosition("spawn_character_look_dir") - character->GetPosition());
        }

        void DwarfHome0::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile)
        {
            DwarfHomeLevel::OnLevelSuccessfullyCompleted(profile);
            UnlockCharacters(profile, FindableCharacters());
            UnlockAbilities(profile, FindableAbilites());
            profile->UnlockCampaignLevel(Campaign::DwarfHomes1());
        }

        bool DwarfHome0::onPortraitUsed(Character::Character* character)
        {
            CompleteLevel(true);
            return true;
        }

        void DwarfHome0::PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> baseCheckpoint)
        {
            std::shared_ptr<DwarfHome0Checkpoint> checkpoint = AsA<DwarfHome0Checkpoint>(baseCheckpoint);

            checkpoint->SetFlareCutscenePlayed(_playedFlareCutscene);

            DwarfHomeLevel::PopulateCheckpoint(checkpoint);
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome0>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::DwarfHomeLevel>(preloads);

        EnumeratePreloads<Character::Portrait>(preloads);
        EnumeratePreloads<Character::FlarePickup>(preloads);

        EnumerateMusicTrackMapPreloads(preloads, Level::DwarfHome0MusicTracks);
        EnumerateMusicSwellMapPreloads(preloads, Level::DwarfHome0MusicSwells);

        EnumerateFindableAbilitySetPreloads(preloads, Level::FindableAbilites());
        EnumerateFindableItemSetPreloads(preloads, Level::StorageRoomItems());
    }
}
