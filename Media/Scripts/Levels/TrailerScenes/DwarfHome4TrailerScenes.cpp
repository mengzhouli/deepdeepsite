#include "Levels/TrailerScenes/TrailerScenes.hpp"

#include "Levels/BasicLevel.hpp"

#include "Lights/PolygonLight.hpp"
#include "CutsceneUtility.hpp"

#include "Drawables/Flame.hpp"
#include "Drawables/BasketRope.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const std::string LevelPath = "Levels/dwarfhomes_4.lvl";

        static const Color LavaLightColor = Color::FromBytes(192, 192, 192, 255);

        static const std::string AreaLightLayerName = "far_background";
        static const std::string FlameLightLayerName = "background";
        static const std::string PillarLightLayerName = "background";
        static const std::string ThroneLightLayerName = "foreground";
        static const std::string BasketRopeLayerName = "buckets";

        enum DwarfHome4TrailerScene
        {
            EndToEndFlythrough,
        };

        class DwarfHome4TrailerScenes : public BasicLevel
        {
        public:
            DwarfHome4TrailerScenes(const LevelParameters& parameters, DwarfHome4TrailerScene scene)
                : BasicLevel(parameters)
                , _scene(scene)
            {
            }

        protected:
            void OnCreate() override
            {
                BasicLevel::OnCreate();

                SetTargetCameraViewSize(3500.0f, 16.0f / 9.0f);

                auto primaryLayer = GetPrimaryLayer();

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
                    flame->SetRadius(_flameRadius);
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

                LevelLayerInstance* areaLightLayer = GetLayer(AreaLightLayerName);
                _mainAreaLight.Area = areaLightLayer->GetTriggerArea("main_area_light");
                _mainAreaLight.LightColor = LavaLightColor;

                _forgeAreaLight.Area = areaLightLayer->GetTriggerArea("forge_area_light");
                _forgeAreaLight.LightColor = LavaLightColor;

                LevelLayerInstance* pillarLightLayer = GetLayer(PillarLightLayerName);
                const auto& pillarLightAreas = pillarLightLayer->GetTriggerAreas("pillar_light_area");
                for (const auto& pillarLightArea : pillarLightAreas)
                {
                    const Rectanglef& bounds = pillarLightArea.Bounds();

                    Lights::LineLight light;
                    light.Start = Vector2f(bounds.Middle().X, bounds.Top());
                    light.End = Vector2f(bounds.Middle().X, bounds.Bottom());
                    light.LightColor = Color::White;
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
                    light.LightColor = Color::White;
                    light.ShadowPercent = 0.0f;
                    light.Radius = 1500.0f;

                    _forgeThroneLights.push_back(light);
                }

                switch (_scene)
                {
                case EndToEndFlythrough:
                    setupEndToEndFlythroughScene();
                    break;
                }
            }

            void OnUpdate(double totalTime, float dt) override
            {
                BasicLevel::OnUpdate(totalTime, dt);

                for (auto &basketRope : _basketRopes)
                {
                    basketRope->Update(totalTime, dt);
                }

                for (auto &flame : _forgeFlames)
                {
                    flame->Update(totalTime, dt);
                }

                for (auto &flame : _mainFlames)
                {
                    flame->Update(totalTime, dt);
                }
            }

            void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override
            {
                BasicLevel::OnDraw(layer, levelRenderer);

                if (layer->GetName() == AreaLightLayerName)
                {
                    levelRenderer->AddLight(_mainAreaLight);
                    levelRenderer->AddLight(_forgeAreaLight);
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

        private:
            void setupEndToEndFlythroughScene()
            {
                auto primaryLayer = GetPrimaryLayer();

                float startZoom = GetCameraController().GetCamera().GetZoom();

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<PanCameraEvent>(primaryLayer, "endtoend_trailer_camera_spline", 45.0f),
                        }),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<ZoomCameraEvent>(this, startZoom * 1.5f, 0.0f),
                            std::make_shared<ZoomCameraEvent>(this, startZoom, 6.0f),
                            std::make_shared<ZoomCameraEvent>(this, startZoom * 0.6f, 6.0f),
                            std::make_shared<TimerEvent>(1.0f),
                            std::make_shared<ZoomCameraEvent>(this, startZoom, 4.0f),
                            std::make_shared<TimerEvent>(3.0f),
                            //std::make_shared<ZoomCameraEvent>(this, startZoom * 0.5f, 3.0f),
                        }),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            Lights::PolygonLight _forgeAreaLight;
            Lights::PolygonLight _mainAreaLight;

            std::vector<std::unique_ptr<Graphics::BasketRopeDrawable>> _basketRopes;

            float _flameRadius;
            std::vector<std::unique_ptr<Graphics::Flame>> _mainFlames;
            std::vector<std::unique_ptr<Graphics::Flame>> _forgeFlames;
            std::vector<Lights::LineLight> _forgePillarLights;
            std::vector<Lights::PointLight> _forgeThroneLights;

            DwarfHome4TrailerScene _scene;
        };

        GameState::LevelConstructor<> CreateEndToEndFlythroughScene()
        {
            return GameState::BindLevelConstructor<DwarfHome4TrailerScenes>(LevelPath, EndToEndFlythrough);
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome4TrailerScenes>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);
        preloads.insert(Level::LevelPath);
    }
}
