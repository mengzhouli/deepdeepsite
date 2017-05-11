#pragma once

#include "Audio/Sound.hpp"
#include "Audio/SoundInstance.hpp"
#include "Audio/SoundManager.hpp"
#include "Content/IHasContent.hpp"
#include "IUpdateable.hpp"
#include "SoundSet.hpp"
#include "HUD/Debugger.hpp"
#include "TypedEnums.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class MusicManagerDebuggerElement;
    }

    namespace Audio
    {
        struct MusicTrack
        {
            std::string Path;
            std::pair<float, float> VolumeRange;
        };
        using MusicTrackMap = std::map<std::string, MusicTrack>;

        struct MusicSwell
        {
            std::string Path;
            float Volume;
        };
        using MusicSwellMap = std::map<std::string, MusicSwell>;

        class MusicManager : public NonCopyable, public IUpdateable, public Content::IHasContent
        {
        public:
            MusicManager(SoundManager* soundManager);

            void AddTrack(const std::string& name, const MusicTrack& musicTrack);
            void AddTracks(const MusicTrackMap& tracks);
            void AddSwell(const std::string& name, const MusicSwell& musicSwell);
            void AddSwells(const MusicSwellMap& musicSwells);

            void SetVolumeInterpolationSpeed(float mainSpeed, float toZeroSpeed);
            void SetTrackDistanceThresholds(float minDistance, float maxDistance);
            void SetMasterVolume(float volume);

            void Play();
            void Stop();

            void LoadContent(Content::ContentManager* contentManager) override;
            void UnloadContent() override;

            void SetTrackWeightings(const std::string& name, float distance);

            void PlaySwell(const std::string& type);

            void InitializeDebugger(HUD::Debugger* debugger);

            void Update(double totalTime, float dt) override;

        private:
            friend class HUD::MusicManagerDebuggerElement;

            SoundManager* _soundManager = nullptr;

            float _masterVolume = 1.0f;
            bool _paused = false;

            float _volumeIntrpolationSpeed = 0.25f;
            float _finalFalloffVolumeIntrpolationSpeed = 0.25f;

            std::pair<float, float> _distanceThresholds = { 1000.0f, 3000.0f };
            struct track
            {
                float volume = 0.0f;
                float distance = 0.0f;
                std::pair<float, float> volumeRange = { 0.0f, 1.0f };
                std::string path = "";
                const Audio::Sound* sound = nullptr;
                std::shared_ptr<ManagedSoundInstance> instance = nullptr;
            };
            std::map<std::string, track> _tracks;

            struct swell
            {
                std::string path = "";
                const Sound* sound = nullptr;
                float volume = 0.0f;
            };
            std::map<std::string, swell> _swells;
        };
    }

    void EnumerateMusicTrackMapPreloads(PreloadSet& preloads, const Audio::MusicTrackMap& tracks);
    void EnumerateMusicSwellMapPreloads(PreloadSet& preloads, const Audio::MusicSwellMap& tracks);
}
