#include "MusicManager.hpp"

#include <imgui.h>

namespace Dwarf
{
    namespace HUD
    {
        class MusicManagerDebuggerElement : public DebuggerElemement
        {
        public:
            MusicManagerDebuggerElement(Audio::MusicManager* musicManager)
                : _musicManager(musicManager)
            {
            }

            bool Update(double totalTime, float dt) override
            {
                bool changed = false;

                changed |= ImGui::SliderFloat("Volume Interpolation Speed", &_musicManager->_volumeIntrpolationSpeed, 0.0f, 1.0f);
                changed |= ImGui::SliderFloat("Volume To Zero Interpolation Speed", &_musicManager->_finalFalloffVolumeIntrpolationSpeed, 0.0f, 0.2f);
                changed |= ImGui::DragFloatRange2("Distance Thresholds", &_musicManager->_distanceThresholds.first, &_musicManager->_distanceThresholds.second, 1.0f, 0.0f, 100000.0f);

                for (auto& nameTrack : _musicManager->_tracks)
                {
                    if (!ImGui::TreeNode(nameTrack.first.c_str()))
                    {
                        continue;
                    }

                    auto& track = nameTrack.second;

                    float volumeCopy = track.volume;
                    ImGui::SliderFloat("Volume", &volumeCopy, 0.0f, 1.0f);

                    float distanceCopy = Clamp(track.distance, _musicManager->_distanceThresholds.first, _musicManager->_distanceThresholds.second);
                    ImGui::SliderFloat("Distance", &distanceCopy, _musicManager->_distanceThresholds.first, _musicManager->_distanceThresholds.second);

                    ImGui::LabelText("File", "%s", FileSystem::GetFileName(track.path).c_str());

                    changed |= ImGui::DragFloatRange2("Volume Range", &track.volumeRange.first, &track.volumeRange.second, 0.01f, 0.0f, 1.0f);

                    ImGui::TreePop();
                }

                return changed;
            }

        private:
            Audio::MusicManager* _musicManager;
        };
    }

    namespace Audio
    {
        MusicManager::MusicManager(SoundManager* soundManager)
            : _soundManager(soundManager)
        {
        }

        void MusicManager::AddTrack(const std::string& name, const MusicTrack& musicTrack)
        {
            track t;
            t.distance = std::numeric_limits<float>::max();
            t.volumeRange = musicTrack.VolumeRange;
            t.path = musicTrack.Path;
            t.instance = nullptr;

            assert(_tracks.find(name) == _tracks.end());
            _tracks[name] = t;
        }

        void MusicManager::AddTracks(const MusicTrackMap& tracks)
        {
            for (const auto& track : tracks)
            {
                AddTrack(track.first, track.second);
            }
        }

        void MusicManager::AddSwell(const std::string& name, const MusicSwell& musicSwell)
        {
            swell s;
            s.path = musicSwell.Path;
            s.volume = musicSwell.Volume;

            assert(_swells.find(name) == _swells.end());
            _swells[name] = s;
        }

        void MusicManager::AddSwells(const MusicSwellMap& musicSwells)
        {
            for (const auto& swell : musicSwells)
            {
                AddSwell(swell.first, swell.second);
            }
        }

        void MusicManager::SetVolumeInterpolationSpeed(float mainSpeed, float toZeroSpeed)
        {
            _volumeIntrpolationSpeed = mainSpeed;
            _finalFalloffVolumeIntrpolationSpeed = toZeroSpeed;
        }

        void MusicManager::SetTrackDistanceThresholds(float minDistance, float maxDistance)
        {
            _distanceThresholds = std::make_pair(minDistance, maxDistance);
        }

        void MusicManager::SetMasterVolume(float volume)
        {
            _masterVolume = volume;
        }

        void MusicManager::Play()
        {
            _paused = false;
        }

        void MusicManager::Stop()
        {
            _paused = true;
        }

        void MusicManager::LoadContent(Content::ContentManager* contentManager)
        {
            for (auto& track : _tracks)
            {
                track.second.sound = contentManager->Load<Audio::Sound>(track.second.path);
                track.second.instance = _soundManager->PlayLoopingGlobalSound(track.second.sound, SoundPriority::High, track.second.volume);
            }

            for (auto& swell : _swells)
            {
                swell.second.sound = contentManager->Load<Audio::Sound>(swell.second.path);
            }
        }

        void MusicManager::UnloadContent()
        {
            for (auto& track : _tracks)
            {
                SafeRelease(track.second.sound);
                track.second.instance.reset();
            }
            for (auto& swell : _swells)
            {
                SafeRelease(swell.second.sound);
            }
        }

        void MusicManager::SetTrackWeightings(const std::string& name, float distance)
        {
            auto iter = _tracks.find(name);
            assert(iter != _tracks.end());
            iter->second.distance = distance;
        }

        void MusicManager::PlaySwell(const std::string& type)
        {
            if (_paused)
            {
                return;
            }

            const auto& swell = _swells.at(type);
            assert(swell.sound);
            _soundManager->PlaySingleGlobalSound(swell.sound, SoundPriority::High, swell.volume);
        }

        void MusicManager::InitializeDebugger(HUD::Debugger* debugger)
        {
            debugger->AddElement("Music", "MusicManager", std::make_shared<HUD::MusicManagerDebuggerElement>(this));
        }

        void MusicManager::Update(double totalTime, float dt)
        {
            for (auto& nameTrack : _tracks)
            {
                auto& track = nameTrack.second;

                float targetVolume = 0.0f;
                std::pair<float, float> volumeLimits(0.0f, 1.0f);
                float interpolationSpeed = 1.0f;

                if (_paused)
                {
                }
                else
                {
                    // A few billion units
                    const float MinVolumeDistanceThreshold = std::numeric_limits<uint32_t>::max();
                    if (track.distance < MinVolumeDistanceThreshold)
                    {
                        float distancePerc = 1.0f - Clamp((track.distance - _distanceThresholds.first) / (_distanceThresholds.second - _distanceThresholds.first), 0.0f, 1.0f);
                        targetVolume = Lerp(track.volumeRange.first, track.volumeRange.second, distancePerc);
                        volumeLimits = track.volumeRange;
                        interpolationSpeed = _volumeIntrpolationSpeed * (track.volumeRange.second - track.volumeRange.first);
                    }
                    else
                    {
                        if (track.volume > track.volumeRange.first)
                        {
                            interpolationSpeed = _volumeIntrpolationSpeed * (track.volumeRange.second - track.volumeRange.first);
                        }
                        else
                        {
                            interpolationSpeed = _finalFalloffVolumeIntrpolationSpeed * track.volumeRange.first;
                        }
                    }
                }

                float distanceToTarget = targetVolume - track.volume;
                float delta = Clamp(Sign(distanceToTarget) * interpolationSpeed * dt, -Abs(distanceToTarget), Abs(distanceToTarget));
                track.volume = Clamp(track.volume + delta, volumeLimits.first, volumeLimits.second);

                track.instance->SetVolume(track.volume * _masterVolume);
            }
        }
    }

    void EnumerateMusicTrackMapPreloads(PreloadSet& preloads, const Audio::MusicTrackMap& tracks)
    {
        for (const auto& track : tracks)
        {
            preloads.insert(track.second.Path);
        }
    }

    void EnumerateMusicSwellMapPreloads(PreloadSet& preloads, const Audio::MusicSwellMap& tracks)
    {
        for (const auto& track : tracks)
        {
            preloads.insert(track.second.Path);
        }
    }
}
