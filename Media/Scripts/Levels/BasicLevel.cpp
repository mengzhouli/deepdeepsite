#include "Levels/BasicLevel.hpp"

namespace Dwarf
{
    namespace Level
    {
        const Color BasicLevel::BaseFireColor = Color::FromBytes(255, 185, 130, 255);

        BasicLevel::BasicLevel(const LevelParameters& parameters)
            : LevelInstance(parameters)
            , _musicManager(GetSoundManager())
            , _ambientSound(GetSoundManager())
        {
        }

        void BasicLevel::InitializeDebugger(HUD::Debugger* debugger)
        {
            _musicManager.InitializeDebugger(debugger);
        }

        BasicLevel::~BasicLevel()
        {
        }

        void BasicLevel::AddMusicSwells(const Audio::MusicSwellMap& swells)
        {
            _musicManager.AddSwells(swells);
        }

        void BasicLevel::PlayMusicSwell(const std::string& name)
        {
            _musicManager.PlaySwell(name);
        }

        void BasicLevel::SetMusicVolumeInterpolationSpeed(float mainSpeed, float toZeroSpeed)
        {
            _musicManager.SetVolumeInterpolationSpeed(mainSpeed, toZeroSpeed);
        }

        void BasicLevel::SetMusicTrackDistanceThresholds(float minDistance, float maxDistance)
        {
            _musicManager.SetTrackDistanceThresholds(minDistance, maxDistance);
        }

        void BasicLevel::AddAmbientSounds(Audio::EnvironmentType environmentType, const Audio::SoundPathVector& sounds, float volume)
        {
            _ambientSound.AddSounds(environmentType, sounds, volume);
        }

        void BasicLevel::SetTargetCameraViewSize(const Vector2f& size)
        {
            CameraController& cameraController = GetCameraController();
            Vector2f viewportSize = cameraController.GetCamera().GetViewport().Size;

            float targetZoom = Min(viewportSize.X / size.X, viewportSize.Y / size.Y);
            cameraController.Zoom(targetZoom);
        }

        void BasicLevel::SetTargetCameraViewSize(float height, float aspectRatio)
        {
            SetTargetCameraViewSize(Vector2f(height * aspectRatio, height));
        }

        void BasicLevel::OnCreate()
        {
            _musicManager.Play();
        }

        void BasicLevel::OnUpdate(double totalTime, float dt)
        {
            _musicManager.SetMasterVolume(GetProfile()->GetMusicVolume());
            _musicManager.Update(totalTime, dt);
            _ambientSound.Update(totalTime, dt);
        }

        void BasicLevel::OnLoadContent(Content::ContentManager* contentManager)
        {
            _musicManager.LoadContent(contentManager);
            _ambientSound.LoadContent(contentManager);
        }

        void BasicLevel::OnUnloadContent()
        {
            _musicManager.UnloadContent();
            _ambientSound.UnloadContent();
        }

        void BasicLevel::SetDefaultEnvironmenType(Audio::EnvironmentType type)
        {
            auto soundManager = GetSoundManager();
            soundManager->SetDefaultEnvironmentType(type);
        }

        void BasicLevel::SetTriggerAreaEnvironmenType(const std::string& areaNames, Audio::EnvironmentType type)
        {
            auto soundManager = GetSoundManager();
            for (uint32_t i = 0; i < GetLayerCount(); i++)
            {
                const auto& layer = GetLayer(i);
                const auto& areas = layer->GetTriggerAreas(areaNames);
                for (const auto& area : areas)
                {
                    soundManager->AddEnvironmentArea(type, area);
                }
            }
        }

        Audio::MusicManager& BasicLevel::GetMusicManager()
        {
            return _musicManager;
        }

        Audio::AmbientSoundManager& BasicLevel::GetAmbientSoundManager()
        {
            return _ambientSound;
        }
    }

    template <>
    void EnumeratePreloads<Level::BasicLevel>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::LevelInstance>(preloads);
    }
}
