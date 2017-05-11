#pragma once

#include "Level/LevelInstance.hpp"
#include "Level/LevelLayerInstance.hpp"
#include "Geometry/Polygon.hpp"
#include "MusicManager.hpp"
#include "AmbientSoundManager.hpp"

#include <string>

namespace Dwarf
{
    namespace HUD
    {
        class Debugger;
    }

    namespace Level
    {
        class BasicLevel : public LevelInstance
        {
        public:
            BasicLevel(const LevelParameters& parameters);

            virtual void InitializeDebugger(HUD::Debugger* debugger);

        protected:
            virtual ~BasicLevel();

            void AddMusicSwells(const Audio::MusicSwellMap& swells);
            void PlayMusicSwell(const std::string& name);

            void SetMusicVolumeInterpolationSpeed(float mainSpeed, float toZeroSpeed);
            void SetMusicTrackDistanceThresholds(float minDistance, float maxDistance);

            void AddAmbientSounds(Audio::EnvironmentType environmentType, const Audio::SoundPathVector& sounds, float volume);

            void SetTargetCameraViewSize(const Vector2f& size);
            void SetTargetCameraViewSize(float height, float aspectRatio);

            void OnCreate() override;

            void OnUpdate(double totalTime, float dt) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            template <typename T>
            std::vector<T*> SpawnCharactersAtTriggers(const std::string& triggerLocations, Character::CharacterConstructor<T> constructor,
                                                      const std::string& nameBase, Character::Controller* controller);

            void SetDefaultEnvironmenType(Audio::EnvironmentType type);
            void SetTriggerAreaEnvironmenType(const std::string& areaNames, Audio::EnvironmentType type);

            static const Color BaseFireColor;

            Audio::MusicManager& GetMusicManager();
            Audio::AmbientSoundManager& GetAmbientSoundManager();

        private:
            Audio::MusicManager _musicManager;
            Audio::AmbientSoundManager _ambientSound;
        };
    }

    template <>
    void EnumeratePreloads<Level::BasicLevel>(PreloadSet& preloads);
}

#include "BasicLevel.inl"
