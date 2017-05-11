#include "Levels/TrailerScenes/TrailerScenes.hpp"

#include "Levels/BasicLevel.hpp"

#include "CutsceneUtility.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/BrewerDwarf.hpp"

#include "Characters/Barricade.hpp"

#include "Characters/Gobbo.hpp"
#include "Items/Weapons/GobboWeapons.hpp"

#include "Characters/Ork.hpp"
#include "Items/Weapons/OrkWeapons.hpp"

#include "Items/Weapons/Gun.hpp"
#include "Items/Weapons/Sword.hpp"
#include "Items/Weapons/Shield.hpp"
#include "Items/Weapons/Kitchenware.hpp"
#include "Items/Trinkets/Headlamp.hpp"
#include "Items/Trinkets/Pipe.hpp"

#include "Abilities/Grapple.hpp"
#include "Abilities/ChannelingFoodHeal.hpp"

#include "NavigationUtility.hpp"

#include "Characters/GrappleRope.hpp"

#include "RainEffect.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const std::string LevelPath = "Levels/dwarfhomes_2.lvl";

        static const std::string DwarfHome2RainLayer = "paralax_3";
        static const uint32_t DwarfHome2RainDropCount = 2048;
        static const Rotatorf DwarfHome2RainDir = Rotatorf(-PiOver2 + PiOver8);
        static const std::pair<float, float> DwarfHome2RainSpeed = { 3500.0f, 3500.0f };

        static const bool DwarfHome2LightningEnabled = true;
        static const std::pair<float, float> DwarfHome2LightningStrikeInterval = { 1.0f, 25.0f };
        static const float DwarfHome2LightningDoubleStrikeChance = 0.2f;

        enum DwarfHome2TrailerScene
        {
            ApproachStatue,
            GrappleScene,
            BridgeBattle,
        };

        class DwarfHome2TrailerScenes : public BasicLevel
        {
        public:
            DwarfHome2TrailerScenes(const LevelParameters& parameters, DwarfHome2TrailerScene scene)
                : BasicLevel(parameters)
                , _scene(scene)
            {
                _rain.reset(new RainEffect(GetLayer(DwarfHome2RainLayer), GetSoundManager()));
            }

        protected:
            void OnCreate() override
            {
                BasicLevel::OnCreate();

                SetTargetCameraViewSize(2500.0f, 16.0f / 9.0f);

                switch (_scene)
                {
                case ApproachStatue:
                    setupStatueApproach();
                    break;
                case GrappleScene:
                    setupGrappleScene();
                    break;
                case BridgeBattle:
                    setupBridgeBattleScene();
                    break;
                }

                _rain->SetRainDirection(DwarfHome2RainDir);
                _rain->SetRainSpeed(DwarfHome2RainSpeed.first, DwarfHome2RainSpeed.second);
                _rain->SetDropCount(DwarfHome2RainDropCount);
                _rain->SetLightningEnabled(DwarfHome2LightningEnabled);
                _rain->SetLightningStrikeInterval(DwarfHome2LightningStrikeInterval.first, DwarfHome2LightningStrikeInterval.second, DwarfHome2LightningDoubleStrikeChance);

            }

        private:
            void setupBridgeBattleScene()
            {
                auto primaryLayer = GetPrimaryLayer();

                auto dwarfController = CreateController("dwarf_controller", Character::BindControllerConstructor<Character::Controller>());
                auto monsterController = CreateController("monster_controller", Character::BindControllerConstructor<Character::Controller>());

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("bridge_trailer_navigator_spawn"), "nav", dwarfController, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicPistol>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                navigator->SetMaxHealth(10000.0f);

                auto fighter = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("bridge_trailer_fighter_spawn"), "fighter", dwarfController, Character::BindCharacterConstructor<Character::FighterDwarf>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IronShortSword>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IronHeater>());
                fighter->SetMaxHealth(10000.0f);

                auto cook = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("bridge_trailer_cook_spawn"), "cook", dwarfController, Character::BindCharacterConstructor<Character::CookDwarf>());
                cook->GiveItem(Item::BindItemConstructor<Item::Cutlery>());
                cook->AddAbility(Ability::BindAbilityConstructor<Ability::ChannelingFoodHeal>());
                cook->SetMaxHealth(10000.0f);

                auto brewer = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("bridge_trailer_brewer_spawn"), "brew", dwarfController, Character::BindCharacterConstructor<Character::BrewerDwarf>());
                brewer->GiveItem(Item::BindItemConstructor<Item::BasicRifle>());
                brewer->GiveItem(Item::BindItemConstructor<Item::BriarPipe>());
                brewer->SetMaxHealth(10000.0f);

                primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("bridge_trailer_barricade_spawn"), "barricade", dwarfController, Character::BindCharacterConstructor<Character::Barricade>(1000));

                primaryLayer->SetTerrainCollisionEnabled("bridge_right_door_collision", false);
                primaryLayer->SetTerrainCollisionEnabled("bridge_left_door_collision", false);

                // Spawn gobbos event
                enum gobboType
                {
                    regular,
                    thrower,
                    bow,
                    chief,
                    ork,
                };

                static const float fastSpawnRate = 2.0f;
                static const std::vector<gobboType> fastSideGobboOrder =
                {
                    regular,
                    thrower,
                    regular,
                    bow,
                    chief,
                };

                static const float slowSpawnRate = 2.5f;
                static const std::vector<gobboType> upperSlowSideGobboOrder =
                {
                    regular,
                    thrower,
                    regular,
                    chief,
                };
                static const std::vector<gobboType> lowerSlowSideGobboOrder =
                {
                    regular,
                    thrower,
                    bow,
                    ork,
                };

                struct gobboSpawnerEvent : public TimerEvent
                {
                    gobboSpawnerEvent(LevelLayerInstance* layer, Character::Controller* controller, const std::string& spawnTrigger, const std::string& attackTarget, float spawnRate, const std::vector<gobboType>& order)
                        : TimerEvent((spawnRate * order.size()) + (spawnRate * 0.5f))
                        , _timer(0.0f)
                        , _layer(layer)
                        , _controller(controller)
                        , _order(order)
                        , _spawnRate(spawnRate)
                        , _spawnPos(layer->GetTriggerPosition(spawnTrigger))
                        , _attackPos(layer->GetTriggerPosition(attackTarget))
                    {
                    }

                    void Update(double totalTime, float dt) override
                    {
                        _timer -= dt;
                        if (_timer < 0.0f && _spawnCount < _order.size())
                        {
                            uint32_t gobType = _order[_spawnCount];

                            Character::BasicCharacter* gob = nullptr;
                            switch (gobType)
                            {
                            case regular:
                            {
                                Character::CharacterConstructor<Character::Gobbo> gobboConstructor = Character::BindCharacterConstructor<Character::Gobbo>();
                                gob = _layer->SpawnCharacter(_spawnPos, "regular_gob", _controller, gobboConstructor);
                                gob->GiveItem(Random::RandomItem(_meleeGobboWeapons));
                            }
                            break;

                            case thrower:
                            {
                                Character::CharacterConstructor<Character::GobboThrower> gobboConstructor = Character::BindCharacterConstructor<Character::GobboThrower>();
                                gob = _layer->SpawnCharacter(_spawnPos, "thrower_gob", _controller, gobboConstructor);
                                gob->GiveItem(Random::RandomItem(_throwGobboWeapons));
                            }
                            break;

                            case chief:
                            {
                                Character::CharacterConstructor<Character::GobboChief> gobboConstructor = Character::BindCharacterConstructor<Character::GobboChief>();
                               gob = _layer->SpawnCharacter(_spawnPos, "cheif_gob", _controller, gobboConstructor);
                                gob->GiveItem(Random::RandomItem(_meleeGobboChiefWeapons));
                            }
                            break;

                            case bow:
                            {
                                Character::CharacterConstructor<Character::GobboBowman> gobboConstructor = Character::BindCharacterConstructor<Character::GobboBowman>();
                                gob = _layer->SpawnCharacter(_spawnPos, "bow_gob", _controller, gobboConstructor);
                                gob->GiveItem(Random::RandomItem(_rangedGobboWeapons));
                            }
                            break;

                            case ork:
                            {
                                Character::CharacterConstructor<Character::OrkWarrior> orkConstructor = Character::BindCharacterConstructor<Character::OrkWarrior>();
                                gob = _layer->SpawnCharacter(_spawnPos, "ork", _controller, orkConstructor);
                                gob->GiveItem(Random::RandomItem(_orkWeapons));
                                gob->GiveItem(Random::RandomItem(_orkWeapons));
                            }
                            break;
                            }

                            gob->PushAction(Character::CreateAttackMoveAction(_attackPos), false);
                            gob->SetAggroRange(100000.0f);

                            _spawnCount++;
                            _timer += _spawnRate;
                        }

                        TimerEvent::Update(totalTime, dt);
                    }

                    const std::vector< Item::ItemConstructor<Item::Weapon> > _meleeGobboWeapons =
                    {
                        Item::BindItemConstructor<Item::GobBlade>(),
                        Item::BindItemConstructor<Item::GobDagger>(),
                        Item::BindItemConstructor<Item::GobFork>(),
                        Item::BindItemConstructor<Item::GobShank>(),
                        Item::BindItemConstructor<Item::GobTrident>(),
                        Item::BindItemConstructor<Item::GobSword>(),
                    };
                    const std::vector< Item::ItemConstructor<Item::Weapon> > _meleeGobboChiefWeapons =
                    {
                        Item::BindItemConstructor<Item::GobJawBlade>(),
                        Item::BindItemConstructor<Item::GobSkullClub>(),
                        Item::BindItemConstructor<Item::GobOrkAxe>(),
                        Item::BindItemConstructor<Item::GobOrkSickle>(),
                    };
                    const std::vector< Item::ItemConstructor<Item::Weapon> > _rangedGobboWeapons =
                    {
                        Item::BindItemConstructor<Item::GobBow>(),
                    };
                    const std::vector< Item::ItemConstructor<Item::Weapon> > _throwGobboWeapons =
                    {
                        Item::BindItemConstructor<Item::GobThrowingRocks>(),
                    };
                    const std::vector< Item::ItemConstructor<Item::Weapon> > _orkWeapons =
                    {
                        Item::BindItemConstructor<Item::OrkChopper>(),
                        Item::BindItemConstructor<Item::OrkClub>(),
                        Item::BindItemConstructor<Item::OrkHook>(),
                        Item::BindItemConstructor<Item::OrkMace>(),
                    };

                    uint32_t _spawnCount = 0;

                    float _timer;
                    const float _spawnRate;
                    const std::vector<gobboType> _order;

                    LevelLayerInstance* _layer;
                    Character::Controller* _controller;
                    Vector2f _spawnPos;
                    Vector2f _attackPos;
                };

                // Channel food heal event
                auto channelFoodHealEvent = [=]()
                {
                    Ability::ChannelingFoodHeal* ability = cook->GetAbility<Ability::ChannelingFoodHeal>();
                    assert(ability);
                    ability->SetPrimaryTarget(cook->GetPosition());
                    cook->PushAction(ability->GenerateAction(), false);
                }; 

                auto holdPositionEvent = [=]()
                {
                    navigator->PushAction(Character::CreateHoldPositionAction(), false);
                    //fighter->PushAction(Character::CreateHoldPositionAction(), false);
                    brewer->PushAction(Character::CreateHoldPositionAction(), false);
                };

                float startZoom = GetCameraController().GetCamera().GetZoom();

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<FocusCameraEvent>(primaryLayer, "bridge_trailer_camera_pos", 0.0f),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<ZoomCameraEvent>(this, startZoom * 2.0f, 0.0f),
                            std::make_shared<TimerEvent>(5.0f),
                            std::make_shared<ZoomCameraEvent>(this, startZoom * 0.75f, primaryLayer->GetTriggerPosition("bridge_trailer_zoom_focus"), 12.0f),
                        }),
                        std::make_shared<FunctionalEvent>(channelFoodHealEvent, nullptr, nullptr, nullptr),
                        std::make_shared<FunctionalEvent>(holdPositionEvent, nullptr, nullptr, nullptr),
                        std::make_shared<gobboSpawnerEvent>(primaryLayer, monsterController, "bridge_trailer_lower_right_spawn", "bridge_trailer_monster_attack_target", slowSpawnRate, lowerSlowSideGobboOrder),
                        std::make_shared<gobboSpawnerEvent>(primaryLayer, monsterController, "bridge_trailer_upper_right_spawn", "bridge_trailer_monster_attack_target", slowSpawnRate, upperSlowSideGobboOrder),
                        std::make_shared<gobboSpawnerEvent>(primaryLayer, monsterController, "bridge_trailer_left_spawn", "bridge_trailer_monster_attack_target", fastSpawnRate, fastSideGobboOrder),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            void setupStatueApproach()
            {
                auto primaryLayer = GetPrimaryLayer();

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("statue_trailer_navigator_spawn"), "nav", nullptr, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicPistol>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());

                auto fighter = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("statue_trailer_fighter_spawn"), "fighter", nullptr, Character::BindCharacterConstructor<Character::FighterDwarf>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IronShortSword>());
                fighter->GiveItem(Item::BindItemConstructor<Item::IronHeater>());

                auto cook = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("statue_trailer_cook_spawn"), "cook", nullptr, Character::BindCharacterConstructor<Character::CookDwarf>());
                cook->GiveItem(Item::BindItemConstructor<Item::Cutlery>());

                float startZoom = GetCameraController().GetCamera().GetZoom();

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<PanCameraEvent>(primaryLayer, "statue_trailer_camera_spline", 14.0f),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(3.0f),
                            std::make_shared<ZoomCameraEvent>(this, startZoom * 0.65f, 9.0f),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(0.2f),
                            std::make_shared<MoveCharacterEvent>(primaryLayer, navigator->GetID(), "statue_trailer_navigator_move_target"),
                            std::make_shared<CharactersLookEvent>(primaryLayer, std::vector<Character::CharacterID>{ navigator->GetID() }, primaryLayer->GetSpline("statue_trailer_navigator_look_spline"), 2.0f),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(0.2f),
                            std::make_shared<MoveCharacterEvent>(primaryLayer, fighter->GetID(), "statue_trailer_fighter_move_target"),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(0.2f),
                            std::make_shared<MoveCharacterEvent>(primaryLayer, cook->GetID(), "statue_trailer_cook_move_target"),
                        }),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            void setupGrappleScene()
            {
                auto primaryLayer = GetPrimaryLayer();

                primaryLayer->SpawnCharacter(Vector2f::Zero, "grapple", nullptr, BindGrappleConstructor(primaryLayer, primaryLayer->GetSpline("grapple_trailer_grapple")));

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("grapple_trailer_navigator_spawn"), "nav", nullptr, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicPistol>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                navigator->AddAbility(Ability::BindAbilityConstructor<Ability::Grapple>(false));
                navigator->SetRotation(Rotatorf::Zero);

                auto fighter = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("grapple_trailer_fighter_spawn"), "fighter", nullptr, Character::BindCharacterConstructor<Character::FighterDwarf>());

                auto cook = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("grapple_trailer_cook_spawn"), "cook", nullptr, Character::BindCharacterConstructor<Character::CookDwarf>());
                cook->GiveItem(Item::BindItemConstructor<Item::Cutlery>());

                // Aim event
                static const float aimDuration = 3.0f;
                struct aimEvent : public CutsceneEvent
                {
                    aimEvent(LevelLayerInstance* layer, Character::CharacterID navigatorID, const Splinef& aimSpline)
                        : _timer(aimDuration)
                        , _layer(layer)
                        , _navigatorID(navigatorID)
                        , _aimSpline(aimSpline)
                        , _finished(false)
                    {
                    }

                    bool IsFinished() const override
                    {
                        return _finished;
                    }

                    void Update(double totalTime, float dt) override
                    {
                        _timer -= dt;
                        float perc = 1.0f - Clamp(_timer / aimDuration, 0.0f, 1.0f);

                        Vector2f target = _aimSpline.Evalulate(perc);
                        Character::NavigatorDwarf* character = _layer->GetCharacter<Character::NavigatorDwarf>(_navigatorID);
                        character->LookAt(target);

                        Ability::Grapple* ability = character->GetAbility<Ability::Grapple>();
                        assert(ability);
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
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<PanCameraEvent>(primaryLayer, "grapple_trailer_camera_spline", 13.0f),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(1.0f),
                            std::make_shared<MoveCharacterEvent>(primaryLayer, fighter->GetID(), "grapple_trailer_fighter_move_target"),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(1.0f),
                            std::make_shared<MoveCharacterEvent>(primaryLayer, cook->GetID(), "grapple_trailer_cook_move_target"),
                            std::make_shared<TimerEvent>(1.5f),
                            std::make_shared<CharactersLookEvent>(primaryLayer, std::vector<Character::CharacterID>{ cook->GetID() }, fighter->GetID(), 5.0f),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(4.0f),
                            std::make_shared<aimEvent>(primaryLayer, navigator->GetID(), primaryLayer->GetSpline("grapple_trailer_grapple_target")),
                        }),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            };

            void OnLoadContent(Content::ContentManager* contentManager) override
            {
                BasicLevel::OnLoadContent(contentManager);

                _rain->LoadContent(contentManager);
            }

            void OnUnloadContent() override
            {
                BasicLevel::OnUnloadContent();

                _rain->UnloadContent();
            }

            void OnUpdate(double totalTime, float dt) override
            {
                BasicLevel::OnUpdate(totalTime, dt);

                _rain->SetCurrentRainEnvironment(RainEnvironment::Open);
                _rain->Update(totalTime, dt);
            }

            void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override
            {
                BasicLevel::OnDraw(layer, levelRenderer);

                if (layer->GetName() == DwarfHome2RainLayer)
                {
                    levelRenderer->AddDrawable(_rain.get(), false);
                    levelRenderer->AddLight(_rain->GetLight());
                }
            }

            DwarfHome2TrailerScene _scene;
            std::unique_ptr<RainEffect> _rain = nullptr;
        };

        GameState::LevelConstructor<> CreateTowerStatueApproachScene()
        {
            return GameState::BindLevelConstructor<DwarfHome2TrailerScenes>(LevelPath, ApproachStatue);
        }

        GameState::LevelConstructor<> CreateTowerGrappleScene()
        {
            return GameState::BindLevelConstructor<DwarfHome2TrailerScenes>(LevelPath, GrappleScene);
        }

        GameState::LevelConstructor<> CreateBridgeBattleScene()
        {
            return GameState::BindLevelConstructor<DwarfHome2TrailerScenes>(LevelPath, BridgeBattle);
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome2TrailerScenes>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);
        preloads.insert(Level::LevelPath);

        EnumeratePreloads<RainEffect>(preloads);

        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Character::FighterDwarf>(preloads);
        EnumeratePreloads<Character::CookDwarf>(preloads);
        EnumeratePreloads<Character::BrewerDwarf>(preloads);
        EnumeratePreloads<Character::GrappleRope>(preloads);
        EnumeratePreloads<Character::Barricade>(preloads);

        EnumeratePreloads<Item::BasicHeadlamp>(preloads);
        EnumeratePreloads<Item::BasicPistol>(preloads);
        EnumeratePreloads<Item::Cutlery>(preloads);
        EnumeratePreloads<Item::BasicRifle>(preloads);
        EnumeratePreloads<Item::BriarPipe>(preloads);

        EnumeratePreloads<Ability::Grapple>(preloads);
        EnumeratePreloads<Ability::ChannelingFoodHeal>(preloads);

        EnumeratePreloads<Character::Gobbo>(preloads);
        EnumeratePreloads<Character::GobboThrower>(preloads);
        EnumeratePreloads<Character::GobboBowman>(preloads);
        EnumeratePreloads<Character::GobboChief>(preloads);

        EnumeratePreloads<Item::GobBlade>(preloads);
        EnumeratePreloads<Item::GobDagger>(preloads);
        EnumeratePreloads<Item::GobFork>(preloads);
        EnumeratePreloads<Item::GobShank>(preloads);
        EnumeratePreloads<Item::GobSword>(preloads);
        EnumeratePreloads<Item::GobTrident>(preloads);
        EnumeratePreloads<Item::GobJawBlade>(preloads);
        EnumeratePreloads<Item::GobSkullClub>(preloads);
        EnumeratePreloads<Item::GobOrkAxe>(preloads);
        EnumeratePreloads<Item::GobOrkSickle>(preloads);
        EnumeratePreloads<Item::GobBow>(preloads);
        EnumeratePreloads<Item::GobThrowingRocks>(preloads);

        EnumeratePreloads<Character::OrkWarrior>(preloads);
        EnumeratePreloads<Item::OrkChopper>(preloads);
        EnumeratePreloads<Item::OrkClub>(preloads);
        EnumeratePreloads<Item::OrkHook>(preloads);
        EnumeratePreloads<Item::OrkMace>(preloads);
    }

}
