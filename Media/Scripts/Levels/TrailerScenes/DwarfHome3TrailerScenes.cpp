#include "Levels/TrailerScenes/TrailerScenes.hpp"

#include "Levels/BasicLevel.hpp"
#include "Drawables/Flame.hpp"
#include "Lights/PolygonLight.hpp"

#include "CutsceneUtility.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/DestructableRocks.hpp"
#include "Characters/Dynamite.hpp"
#include "Characters/Torch.hpp"

#include "Characters/Chest.hpp"

#include "Characters/Gobbo.hpp"
#include "Items/Weapons/GobboWeapons.hpp"

#include "Characters/Ork.hpp"
#include "Items/Weapons/OrkWeapons.hpp"

#include "Items/Weapons/Gun.hpp"
#include "Items/Weapons/Sword.hpp"
#include "Items/Weapons/Shield.hpp"
#include "Items/Weapons/Kitchenware.hpp"
#include "Items/Armors/FighterArmor.hpp"
#include "Items/Trinkets/Headlamp.hpp"
#include "Items/Trinkets/Pipe.hpp"
#include "Items/Trinkets/Cookbook.hpp"

#include "Abilities/ShootFlare.hpp"
#include "Abilities/PlantDynamite.hpp"

#include "NavigationUtility.hpp"

#include "Characters/GrappleRope.hpp"

#include "Characters/GameCharacters.hpp"
#include "Items/GameItems.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const std::string LevelPath = "Levels/dwarfhomes_3.lvl";

        static const Color BossAreaLightColor = Color::FromBytes(255, 255, 255, 255);
        static const std::string BossAreaLightLayerName = "paralax_1";
        static const std::string FlameLightLayerName = "foreground";

        enum DwarfHome3TrailerScene
        {
            ApproachChest,
            BlowUpRocks,
            RubbleBossIntro,
        };

        class DwarfHome3TrailerScenes : public BasicLevel
        {
        public:
            DwarfHome3TrailerScenes(const LevelParameters& parameters, DwarfHome3TrailerScene scene)
                : BasicLevel(parameters)
                , _scene(scene)
            {
            }

        protected:
            void OnCreate() override
            {
                BasicLevel::OnCreate();

                SetTargetCameraViewSize(2500.0f, 16.0f / 9.0f);

                LevelLayerInstance* bossLightLayer = GetLayer(BossAreaLightLayerName);
                _bossAreaLight.Area = bossLightLayer->GetTriggerArea("boss_area_light");
                _bossAreaLight.LightColor = BossAreaLightColor;

                // Spawn flames
                auto primaryLayer = GetPrimaryLayer();
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

                switch (_scene)
                {
                case ApproachChest:
                    setupChestApproach();
                    break;
                case BlowUpRocks:
                    setupDynamiteScene();
                    break;
                case RubbleBossIntro:
                    setupBossIntroScene();
                    break;
                }
            }

            void OnLoadContent(Content::ContentManager* contentManager) override
            {
                BasicLevel::OnLoadContent(contentManager);

                for (auto &flame : _lavaFlames)
                {
                    flame->LoadContent(contentManager);
                }
            }

            void OnUnloadContent() override
            {
                BasicLevel::OnUnloadContent();

                for (auto &flame : _lavaFlames)
                {
                    flame->UnloadContent();
                }
            }

            void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override
            {
                BasicLevel::OnDraw(layer, levelRenderer);

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

        private:
            void setupChestApproach()
            {
                auto primaryLayer = GetPrimaryLayer();

                auto dwarfController = CreateController("dwarf_controller", Character::BindControllerConstructor<Character::Controller>());

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("chest_trailer_navigator_spawn"), "nav", dwarfController, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::FingerCannonPistol>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                navigator->SetMaxHealth(10000.0f);

                auto fighter = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("chest_trailer_fighter_spawn"), "fighter", dwarfController, Character::BindCharacterConstructor<Character::FighterDwarf>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IgnitionPinSword>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IronHeater>());
                fighter->GiveItem(Item::BindItemConstructor<Item::BristleNogginArmor>());
                fighter->SetMaxHealth(10000.0f);

                auto cook = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("chest_trailer_cook_spawn"), "cook", dwarfController, Character::BindCharacterConstructor<Character::CookDwarf>());
                cook->GiveItem(Item::BindItemConstructor<Item::ServingTools>());
                cook->GiveItem(Item::BindItemConstructor<Item::DeVermisCoctioneCookbook>());
                cook->SetMaxHealth(10000.0f);

                auto brewer = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("chest_trailer_brewer_spawn"), "brew", dwarfController, Character::BindCharacterConstructor<Character::BrewerDwarf>());
                brewer->GiveItem(Item::BindItemConstructor<Item::BlunderbussRifle>());
                brewer->GiveItem(Item::BindItemConstructor<Item::SteinPipe>());
                brewer->SetMaxHealth(10000.0f);

                Item::FindableItemSet chestItems = 
                {
                    // Weapons
                    Item::FindableItems
                    {
                        {
                            Character::Dwarves::Fighter(),
                            {
                                { Item::Weapons::LettersbaneSword(), 1 }
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
                        {
                            Character::Dwarves::Fighter(),
                            {
                                { Item::Armors::TwinFangArmor(), 1 }
                            }
                        },
                    },

                    // Trinkets
                    Item::FindableItems
                    {
                    },
                };
                auto chest = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("chest_trailer_chest_spawn"), "chest", nullptr, Character::BindCharacterConstructor<Character::IronChest>(chestItems, false, nullptr));
                chest->SetRotation(Rotatorf::Pi);

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<PanCameraEvent>(primaryLayer, "chest_trailer_camera_spline", 10.0f),
                        std::make_shared<InteractCharacterEvent>(primaryLayer, fighter->GetID(), chest->GetID()),
                        std::make_shared<MoveCharacterEvent>(primaryLayer, cook->GetID(), "chest_trailer_cook_move_target"),
                        std::make_shared<MoveCharacterEvent>(primaryLayer, brewer->GetID(), "chest_trailer_brewer_move_target"),
                        std::make_shared<MoveCharacterEvent>(primaryLayer, navigator->GetID(), "chest_trailer_navigator_move_target"),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            void setupDynamiteScene()
            {
                auto primaryLayer = GetPrimaryLayer();

                auto dwarfController = CreateController("dwarf_controller", Character::BindControllerConstructor<Character::Controller>());

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("dynamite_trailer_navigator_spawn"), "nav", dwarfController, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BlunderbussPistol>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                navigator->AddAbility(Ability::BindAbilityConstructor<Ability::ShootFlare>(false));
                navigator->SetMaxHealth(10000.0f);
                navigator->DisablePhysics();

                auto brewer = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("dynamite_trailer_brewer_spawn"), "brew", dwarfController, Character::BindCharacterConstructor<Character::BrewerDwarf>());
                brewer->GiveItem(Item::BindItemConstructor<Item::HandCannonRifle>());
                brewer->GiveItem(Item::BindItemConstructor<Item::MeerschaumPipe>());
                brewer->AddAbility(Ability::BindAbilityConstructor<Ability::PlantDynamite>(2.0f, 0.0f, 1000.0f, false));
                brewer->SetMaxHealth(10000.0f);
                brewer->DisablePhysics();

                auto campfire = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("dynamite_trailer_campfire_spawn"), "campfire", nullptr, Character::BindCharacterConstructor<Character::CampFire>(BaseFireColor, true, true, true));

                auto gob0 = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("dynamite_trailer_gobbo0_spawn"), "gob0", nullptr, Character::BindCharacterConstructor<Character::Gobbo>());
                gob0->SetRotation(Rotatorf::Pi);

                auto gob1 = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("dynamite_trailer_gobbo1_spawn"), "gob1", nullptr, Character::BindCharacterConstructor<Character::GobboThrower>());

                primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("entry_rocks_spawn"), "rocks", dwarfController, Character::BindCharacterConstructor<Character::DestructableRubble>(Character::RubbleSize_Medium, "entry_rocks_collision", nullptr));

                // Aim event
                auto aimFlareEvent = [=]()
                {
                    Ability::ShootFlare* flareAbility = navigator->GetAbility<Ability::ShootFlare>();
                    flareAbility->SetPrimaryTarget(primaryLayer->GetTriggerPosition("dynamite_trailer_flare_aim_pos"));
                };

                auto shootFlareEvent = [=]()
                {
                    Ability::ShootFlare* flareAbility = navigator->GetAbility<Ability::ShootFlare>();
                    flareAbility->SetPrimaryTarget(primaryLayer->GetTriggerPosition("dynamite_trailer_flare_aim_pos"));
                    assert(flareAbility->IsInValidState());
                    navigator->PushAction(flareAbility->GenerateAction(), false);
                    flareAbility->ClearState();
                };

                // Dyanmite event
                auto plantDynamiteEvent = [=]()
                {
                    Ability::PlantDynamite* dynamiteAbility = brewer->GetAbility<Ability::PlantDynamite>();
                    dynamiteAbility->SetPrimaryTarget(primaryLayer->GetTriggerPosition("dynamite_trailer_drop_pos"));
                    assert(dynamiteAbility->IsInValidState());
                    brewer->PushAction(dynamiteAbility->GenerateAction(), false);
                    dynamiteAbility->ClearState();
                };

                auto plantDynamiteFinishedEvent = [=]()
                {
                    return brewer->GetCurrentState() == Character::CharacterState_Idle;
                };

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<PanCameraEvent>(primaryLayer, "dynamite_trailer_camera_spline", 6.0f),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(0.1f),
                            std::make_shared<FunctionalEvent>(aimFlareEvent, nullptr, nullptr, nullptr),
                            std::make_shared<TimerEvent>(1.5f),
                            std::make_shared<FunctionalEvent>(shootFlareEvent, nullptr, nullptr, nullptr),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(0.1f),
                            std::make_shared<FunctionalEvent>(plantDynamiteEvent, plantDynamiteFinishedEvent, nullptr, nullptr),
                            std::make_shared<MoveCharacterEvent>(primaryLayer, brewer->GetID(), "dynamite_trailer_brewer_move_pos"),
                        }),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            void setupBossIntroScene()
            {
                auto primaryLayer = GetPrimaryLayer();

                auto dwarfController = CreateController("dwarf_controller", Character::BindControllerConstructor<Character::Controller>());

                Color minColor = Color::FromFloats(0.3f, 0.3f, 0.3f, 1.0f);

                auto grapple = primaryLayer->SpawnCharacter(Vector2f::Zero, "grapple", nullptr, BindGrappleConstructor(primaryLayer, primaryLayer->GetSpline("boss_trailer_grapple_spawn")));
                grapple->Update(0.0f, 0.0f);

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("boss_trailer_navigator_spawn"), "nav", dwarfController, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::FingerCannonPistol>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                navigator->SetMinimumColor(minColor);
                navigator->SetMaxHealth(10000.0f);

                auto fighter = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("boss_trailer_fighter_spawn"), "fighter", dwarfController, Character::BindCharacterConstructor<Character::FighterDwarf>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IgnitionPinSword>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IronHeater>());
                fighter->GiveItem(Item::BindItemConstructor<Item::BristleNogginArmor>());
                fighter->SetMinimumColor(minColor);
                fighter->SetMaxHealth(10000.0f);

                auto cook = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("boss_trailer_cook_spawn"), "cook", dwarfController, Character::BindCharacterConstructor<Character::CookDwarf>());
                cook->GiveItem(Item::BindItemConstructor<Item::ServingTools>());
                cook->GiveItem(Item::BindItemConstructor<Item::DeVermisCoctioneCookbook>());
                cook->SetMinimumColor(minColor);
                cook->SetMaxHealth(10000.0f);

                auto brewer = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("boss_trailer_brewer_spawn"), "brew", dwarfController, Character::BindCharacterConstructor<Character::BrewerDwarf>());
                brewer->GiveItem(Item::BindItemConstructor<Item::BlunderbussRifle>());
                brewer->GiveItem(Item::BindItemConstructor<Item::SteinPipe>());
                brewer->SetMinimumColor(minColor);
                brewer->SetMaxHealth(10000.0f);

                auto boss = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("boss_trailer_boss_spawn"), "boss", dwarfController, Character::BindCharacterConstructor<Character::ShieldOrk>());
                boss->SetRotation(Rotatorf::Pi);
                boss->SetMinimumColor(minColor);

                auto awakenBossEvent = [=]()
                {
                    boss->StandUp();
                };

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<PanCameraEvent>(primaryLayer, "boss_trailer_camera_spline", 10.0f),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(0.1f),
                            std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                            {
                                std::make_shared<MoveCharacterEvent>(primaryLayer, navigator->GetID(), "boss_trailer_navigator_move_pos"),
                                std::make_shared<MoveCharacterEvent>(primaryLayer, fighter->GetID(), "boss_trailer_fighter_move_pos"),
                                std::make_shared<MoveCharacterEvent>(primaryLayer, cook->GetID(), "boss_trailer_cook_move_pos"),
                                std::make_shared<MoveCharacterEvent>(primaryLayer, brewer->GetID(), "boss_trailer_brewer_move_pos"),
                            }),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(4.5f),
                            std::make_shared<FunctionalEvent>(awakenBossEvent, nullptr, nullptr, nullptr),
                        }),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            DwarfHome3TrailerScene _scene;

            std::vector<std::unique_ptr<Graphics::Flame>> _lavaFlames;
            Lights::PolygonLight _bossAreaLight;
        };

        GameState::LevelConstructor<> CreateChestApproachScene()
        {
            return GameState::BindLevelConstructor<DwarfHome3TrailerScenes>(LevelPath, ApproachChest);
        }

        GameState::LevelConstructor<> CreateDynamiteScene()
        {
            return GameState::BindLevelConstructor<DwarfHome3TrailerScenes>(LevelPath, BlowUpRocks);
        }

        GameState::LevelConstructor<> CreateShieldBossIntroScene()
        {
            return GameState::BindLevelConstructor<DwarfHome3TrailerScenes>(LevelPath, RubbleBossIntro);
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome3TrailerScenes>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);
        preloads.insert(Level::LevelPath);

        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Character::FighterDwarf>(preloads);
        EnumeratePreloads<Character::CookDwarf>(preloads);
        EnumeratePreloads<Character::BrewerDwarf>(preloads);
        EnumeratePreloads<Character::IronChest>(preloads);
        EnumeratePreloads<Character::Dynamite>(preloads);
        EnumeratePreloads<Character::CampFire>(preloads);
        EnumeratePreloads<Character::Gobbo>(preloads);
        EnumeratePreloads<Character::GobboThrower>(preloads);
        EnumeratePreloads<Character::ShieldOrk>(preloads);
        EnumeratePreloads<Character::GrappleRope>(preloads);

        EnumeratePreloads<Ability::ShootFlare>(preloads);
        EnumeratePreloads<Ability::PlantDynamite>(preloads);

        EnumeratePreloads<Item::BlunderbussPistol>(preloads);
        EnumeratePreloads<Item::FingerCannonPistol>(preloads);
        EnumeratePreloads<Item::BasicHeadlamp>(preloads);
        EnumeratePreloads<Item::IgnitionPinSword>(preloads);
        EnumeratePreloads<Item::IronHeater>(preloads);
        EnumeratePreloads<Item::BristleNogginArmor>(preloads);
        EnumeratePreloads<Item::ServingTools>(preloads);
        EnumeratePreloads<Item::DeVermisCoctioneCookbook>(preloads);
        EnumeratePreloads<Item::HandCannonRifle>(preloads);
        EnumeratePreloads<Item::BlunderbussRifle>(preloads);
        EnumeratePreloads<Item::SteinPipe>(preloads);
        EnumeratePreloads<Item::MeerschaumPipe>(preloads);

        EnumeratePreloads<Item::LettersbaneSword>(preloads);
        EnumeratePreloads<Item::BlunderbussPistol>(preloads);
        EnumeratePreloads<Item::TwinFangArmor>(preloads);

        EnumeratePreloads<Character::DestructableRubble>(preloads);

        EnumeratePreloads<Graphics::Flame>(preloads);
    }
}
