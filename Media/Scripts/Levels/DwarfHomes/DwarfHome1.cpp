#include "Levels/DwarfHomes/DwarfHome1.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Portrait.hpp"

#include "Abilities/GameAbilities.hpp"
#include "Abilities/Grapple.hpp"

#include "Items/GameItems.hpp"

#include "CutsceneUtility.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const Audio::MusicTrackMap DwarfHome1MusicTracks = Audio::MusicTrackMap
        {
            {
                "music_shuffle",
                {
                    "Music/DwarfHome/MusicTrack_FirstDig_shuffle.ogg",
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
            {
                "music_bass_bells",
                {
                    "Music/DwarfHome/MusicTrack_FirstDig_bassBells.ogg",
                    { 0.05f, 0.3f },
                },
            },
            {
                "music_pianos",
                {
                    "Music/DwarfHome/MusicTrack_FirstDig_pianos.ogg",
                    { 0.05f, 0.3f },
                },
            },
        };

        static const char* GoodSwellName = "good_swell";
        static const char* BadSwellName = "bad_swell";
        static const Audio::MusicSwellMap DwarfHome1MusicSwells = Audio::MusicSwellMap
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

        static const auto WorkshopItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Fighter(),
                        {
                            { Item::Weapons::RustyIronShortSword(), 1 }
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

        static const auto KitchenItems = []()
        {
            return Item::FindableItemSet
            {
                // Weapons
                Item::FindableItems
                {
                    {
                        Character::Dwarves::Cook(),
                        {
                            { Item::Weapons::PrepTools(), 1 },
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
                        Character::Dwarves::Cook(),
                        {
                            { Item::Trinkets::DeVermisCoctioneCookbook(), 1 },
                        }
                    },
                },
            };
        };

        static const auto FindableCharacters = []()
        {
            return Character::FindableCharacterSet
            {
                Character::Dwarves::Cook(),
            };
        };

        static const auto FindableAbilites = []()
        {
            return Ability::FindableAbilitySet
            {
                {
                    Character::Dwarves::Navigator(),
                    {
                        Ability::Abilities::Grapple(),
                    }
                },
                {
                    Character::Dwarves::Cook(),
                    {
                        Ability::Abilities::ChannelingFoodHeal(),
                    },
                }
            };
        };

        DwarfHome1Checkpoint::DwarfHome1Checkpoint()
            : _grapplePickedUp(false)
        {
        }

        DwarfHome1Checkpoint::~DwarfHome1Checkpoint()
        {
        }

        bool DwarfHome1Checkpoint::WasGrapplePickedUp() const
        {
            return _grapplePickedUp;
        }

        void DwarfHome1Checkpoint::SetGrapplePickedUp(bool pickedUp)
        {
            _grapplePickedUp = pickedUp;
        }

        DwarfHome1::DwarfHome1(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : DwarfHomeLevel(parameters, campaignParameters)
            , _cookPortraitID(0)
            , _grapplePickedUp(false)
            , _grapplePickupID(0)
        {
            AddMusicTracks(DwarfHome1MusicTracks);
            AddMusicSwells(DwarfHome1MusicSwells);
        }

        DwarfHome1::~DwarfHome1()
        {
        }

        CampaignLevelInfo DwarfHome1::GetCampaignLevelInfo()
        {
            CampaignLevelInfo info;
            info.NameCode = "dwarfhome1_name";
            info.MaxTotalCharacters = 2;
            info.MinTypedCharacters[typeid(Character::NavigatorDwarf)] = 1;
            info.MinTypedCharacters[typeid(Character::FighterDwarf)] = 1;
            info.StartingResources = Item::Resources(0);

            info.FindableCharacters = FindableCharacters();
            info.FindableItems = Item::MergeFindableItems(WorkshopItems(), KitchenItems());
            info.FindableAbilities = FindableAbilites();

            return info;
        }

        static void giveNavigatorsGrapple(const Character::Controller* playerController)
        {
            Ability::AbilityConstructor<> abilityConstructor = Ability::GameAbility::GetConstructor(Ability::Abilities::Grapple());
            std::vector<Character::NavigatorDwarf*> navigators = playerController->GetCharacters<Character::NavigatorDwarf>(std::string());
            for (Character::NavigatorDwarf* navigator : navigators)
            {
                if (navigator->GetAbility<Ability::Grapple>() == nullptr)
                {
                    navigator->AddAbility(abilityConstructor);
                }
            }
        }

        void DwarfHome1::OnCreate()
        {
            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            SpawnCheckpoint<DwarfHome1Checkpoint>(primaryLayer->GetSpline("dwarf_spawn"), "dwarfhome1_spawn_checkpoint_name", true);
            SpawnCheckpoint<DwarfHome1Checkpoint>(primaryLayer->GetSpline("workshop_checkpoint"), "dwarfhome1_workshop_checkpoint_name", false);
            SpawnCheckpoint<DwarfHome1Checkpoint>(primaryLayer->GetSpline("assembly_hall_checkpoint"), "dwarfhome1_assembly_hall_checkpoint_name", false);

            DwarfHomeLevel::OnCreate();

            std::shared_ptr<const DwarfHome1Checkpoint> checkpoint = GetCheckpoint<DwarfHome1Checkpoint>();

            using namespace std::placeholders;

            std::type_index cookType = typeid(Character::CookDwarf);
            Character::OnPortraitUseCallback portraitCallback = std::bind(&DwarfHome1::onPortraitUsed, this, std::placeholders::_1);
            Character::CharacterConstructor<Character::Portrait> portraitConstructor =
                Character::BindCharacterConstructor<Character::Portrait>(cookType, portraitCallback);
            const Vector2f& portraitPos = primaryLayer->GetTriggerPosition("portrait_spawn");
            const Vector2f& portraitDir = primaryLayer->GetTriggerPosition("portrait_dir");
            Character::Portrait* cookPortrait = primaryLayer->SpawnCharacter(portraitPos, "portrait", nullptr, portraitConstructor);
            cookPortrait->SetRotation(Rotatorf(portraitPos - portraitDir));
            _cookPortraitID = cookPortrait->GetID();

            bool grappledPickedUp = checkpoint != nullptr && checkpoint->WasGrapplePickedUp();
            if (grappledPickedUp)
            {
                giveNavigatorsGrapple(GetPlayerController());
            }

            Character::OnPortraitUseCallback grappleCallback = std::bind(&DwarfHome1::onGrappleUsed, this, std::placeholders::_1);
            Character::CharacterConstructor<Character::GrapplePickup> abilityPickupConstructor =
                Character::BindCharacterConstructor<Character::GrapplePickup>(grappledPickedUp, grappleCallback);
            const Vector2f& grapplePos = primaryLayer->GetTriggerPosition("grapple_spawn");
            Character::GrapplePickup* grapplePickup = primaryLayer->SpawnCharacter(grapplePos, "grapple", nullptr, abilityPickupConstructor);
            _grapplePickupID = grapplePickup->GetID();

            // Swells
            AddTrackedTriggeredMusicSwell("swell_cave_exit", BadSwellName, checkpoint);
            AddTrackedTriggeredMusicSwell("swell_workshop_chest", GoodSwellName, checkpoint);
            AddTrackedTriggeredMusicSwell("swell_kitchen_chest", GoodSwellName, checkpoint);

            // Spawn chests
            SpawnTrackedChest(primaryLayer, "workshop_chest_spawn", true, WorkshopItems(), checkpoint);
            SpawnTrackedChest(primaryLayer, "kitchen_chest_spawn", false, KitchenItems(), checkpoint);
        }

        void DwarfHome1::OnUpdate(double totalTime, float dt)
        {
            DwarfHomeLevel::OnUpdate(totalTime, dt);
        }

        void DwarfHome1::OnSpawnCampaignCharacter(Character::Character* character)
        {
            DwarfHomeLevel::OnSpawnCampaignCharacter(character);

            character->SetRotation(GetPrimaryLayer()->GetTriggerPosition("spawn_character_look_dir") - character->GetPosition());
        }

        void DwarfHome1::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile)
        {
            DwarfHomeLevel::OnLevelSuccessfullyCompleted(profile);
            UnlockCharacters(profile, FindableCharacters());
            UnlockAbilities(profile, FindableAbilites());
            profile->UnlockCampaignLevel(Campaign::DwarfHomes2());
        }

        void DwarfHome1::PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> baseCheckpoint)
        {
            std::shared_ptr<DwarfHome1Checkpoint> checkpoint = AsA<DwarfHome1Checkpoint>(baseCheckpoint);

            checkpoint->SetGrapplePickedUp(_grapplePickedUp);

            DwarfHomeLevel::PopulateCheckpoint(checkpoint);
        }

        bool DwarfHome1::onPortraitUsed(Character::Character* character)
        {
            CompleteLevel(true);
            return true;
        }

        static std::shared_ptr<Cutscene> createGrappleCutscene(LevelLayerInstance* layer, Character::CharacterID navigatorID)
        {
            // Camera focus event
            auto focusCameraEvent = [=]()
            {
                CameraController& camController = layer->GetLevel()->GetCameraController();
                camController.Pan(layer->GetTriggerPosition("grapple_cutscene_camera_pos"), 2.0f);
            };

            // Move event
            Vector2f moveTarget = layer->GetTriggerPosition("grapple_cutscene_move_pos");
            auto moveStartFunc = [=]()
            {
                Character::NavigatorDwarf* character = layer->GetCharacter<Character::NavigatorDwarf>(navigatorID);
                character->PushAction(Character::CreateMoveAction(moveTarget), false);
            };
            auto moveFinishedFunc = [=]()
            {
                Character::NavigatorDwarf* character = layer->GetCharacter<Character::NavigatorDwarf>(navigatorID);
                return !character->IsMoving();
            };

            // Aim event
            const Splinef& aimSpline = layer->GetSpline("grapple_cutscene_aim_spline");
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

                    Ability::Grapple* ability = character->GetAbility<Ability::Grapple>();
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


            std::vector<std::shared_ptr<CutsceneEvent>> events
            {
                std::make_shared<FunctionalEvent>(focusCameraEvent, nullptr, nullptr, nullptr),
                std::make_shared<TimerEvent>(0.5f),
                std::make_shared<FunctionalEvent>(moveStartFunc, moveFinishedFunc, nullptr, nullptr),
                std::make_shared<TimerEvent>(0.5f),
                std::make_shared<aimEvent>(layer, navigatorID, aimSpline),
                std::make_shared<TimerEvent>(1.5f),
            };
            return std::make_shared<Cutscene>(events);
        }

        bool DwarfHome1::onGrappleUsed(Character::Character* character)
        {
            LevelLayerInstance* primaryLayer = GetPrimaryLayer();

            Character::AbilityPickup* grapplePickup = primaryLayer->GetCharacter<Character::AbilityPickup>(_grapplePickupID);

            // Find the closest navigator to do the cutscene
            Character::NavigatorDwarf* closestNavigator = nullptr;
            float closestDist = std::numeric_limits<float>::max();

            const Character::Controller* playerController = GetPlayerController();
            std::vector<Character::NavigatorDwarf*> navigators = playerController->GetCharacters<Character::NavigatorDwarf>(std::string());
            for (Character::NavigatorDwarf* navigator : navigators)
            {
                float dist = Vector2f::Distance(grapplePickup->GetPosition(), navigator->GetPosition());
                if (dist < closestDist)
                {
                    closestNavigator = navigator;
                    closestDist = dist;
                }
            }

            giveNavigatorsGrapple(playerController);

            assert(closestNavigator);

            PlayCutscene(createGrappleCutscene(primaryLayer, closestNavigator->GetID()));

            _grapplePickedUp = true;

            return true;
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome1>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::DwarfHomeLevel>(preloads);

        EnumeratePreloads<Character::Portrait>(preloads);
        EnumeratePreloads<Character::GrapplePickup>(preloads);

        EnumerateFindableAbilitySetPreloads(preloads, Level::FindableAbilites());
        EnumerateFindableItemSetPreloads(preloads, Level::WorkshopItems());
        EnumerateFindableItemSetPreloads(preloads, Level::KitchenItems());

        EnumerateMusicTrackMapPreloads(preloads, Level::DwarfHome1MusicTracks);
        EnumerateMusicSwellMapPreloads(preloads, Level::DwarfHome1MusicSwells);
    }
}
