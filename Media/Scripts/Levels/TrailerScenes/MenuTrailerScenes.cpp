#include "Levels/TrailerScenes/TrailerScenes.hpp"

#include "Levels/BasicLevel.hpp"
#include "CutsceneUtility.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Items/Trinkets/Headlamp.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const std::string LevelPath = "Levels/menu.lvl";

        enum MenuTrailerScene
        {
            TitleLight,
        };

        class MenuTrailerScenes : public BasicLevel
        {
        public:
            MenuTrailerScenes(const LevelParameters& parameters, MenuTrailerScene scene)
                : BasicLevel(parameters)
                , _scene(scene)
            {
            }

        protected:
            void OnCreate() override
            {
                BasicLevel::OnCreate();

                SetTargetCameraViewSize(2500.0f, 16.0f / 9.0f);

                switch (_scene)
                {
                case TitleLight:
                    setupTitleLightScene();
                    break;
                }
            }

        private:
            void setupTitleLightScene()
            {
                auto primaryLayer = GetPrimaryLayer();
                auto pathSystem = primaryLayer->GetPathSystem();

                Vector2f ropeTopRight = primaryLayer->GetTriggerPosition("rope_top_right");
                Vector2f ropeBotRight = primaryLayer->GetTriggerPosition("rope_bot_right");
                pathSystem->AddLine(ropeBotRight, ropeTopRight, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope);

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("menu_trailer_navigator_spawn"), "nav", nullptr, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());

                // Aim event
                static const float flickerDuration = 4.0f;
                struct flickerLightEvent : public TimerEvent
                {
                    flickerLightEvent(Character::Character* navigator)
                        : TimerEvent(flickerDuration)
                        , _navigator(navigator)
                        , _initialAmbientLight(navigator->GetLevel()->GetAmbientLightColor())
                    {
                    }

                    void Update(double totalTime, float dt) override
                    {
                        TimerEvent::Update(totalTime, dt);

                        _timer += dt;

                        uint32_t keyIdx = 0;
                        while (keyIdx + 1 < _lightIntensityMap.size() && _timer > _lightIntensityMap[keyIdx + 1].first)
                        {
                            keyIdx++;
                        }

                        float t = (_timer - _lightIntensityMap[keyIdx].first) / (_lightIntensityMap[keyIdx + 1].first - _lightIntensityMap[keyIdx].first);
                        float intensity = Lerp(_lightIntensityMap[keyIdx].second, _lightIntensityMap[keyIdx + 1].second, t);

                        auto lamp = _navigator->GetItems<Item::Headlamp>().front();
                        lamp->SetLightIntensity(intensity);

                        if (_timer >= _ambientLightFadeStartTime)
                        {
                            float ambientLightT = Saturate((_timer - _ambientLightFadeStartTime) / (flickerDuration - _ambientLightFadeStartTime));
                            Color ambientLight = Color::Lerp(_initialAmbientLight, Color::Black, ambientLightT);
                            _navigator->GetLevel()->SetAmbientLightColor(ambientLight);
                        }
                    }

                    Character::Character* _navigator;

                    float _timer = 0.0f;
                    const std::vector<std::pair<float, float>> _lightIntensityMap = 
                    {
                        { 0.0f, 1.0f },
                        { 0.5f, 1.0f },
                        { 1.0f, 0.5f },
                        { 1.1f, 1.0f },
                        { 1.4f, 0.75f },
                        { 2.0f, 1.0f },
                        { 3.0f, 1.0f },
                        { 3.1f, 0.0f },
                        { 3.2f, 1.0f },
                        { 3.95f, 1.0f },
                        { 4.0f, 0.0f },
                    };

                    const Color _initialAmbientLight;
                    const float _ambientLightFadeStartTime = 3.95f;
                };

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<PanCameraEvent>(primaryLayer, "menu_trailer_camera_spline", 10.0f),
                        std::make_shared<CharactersLookEvent>(primaryLayer, std::vector<Character::CharacterID>{ navigator->GetID() }, primaryLayer->GetTriggerPosition("menu_trailer_navigator_look_target"), 15.0f),
                        std::make_shared<MoveCharacterEvent>(primaryLayer, navigator->GetID(), "menu_trailer_navigator_move_pos"),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<TimerEvent>(11.0f),
                            std::make_shared<flickerLightEvent>(navigator),
                        }),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            MenuTrailerScene _scene;
        };

        GameState::LevelConstructor<> CreateTitleLightScene()
        {
            return GameState::BindLevelConstructor<MenuTrailerScenes>(LevelPath, TitleLight);
        }
    }

    template <>
    void EnumeratePreloads<Level::MenuTrailerScenes>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);
        preloads.insert(Level::LevelPath);

        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Item::BasicHeadlamp>(preloads);
    }
}
