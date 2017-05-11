#pragma once

#include "Content/Preload.hpp"
#include "Content/ContentManager.hpp"
#include "Geometry/Chain.hpp"
#include "NonCopyable.hpp"
#include "Camera.hpp"
#include "Audio/SoundManager.hpp"

namespace Dwarf
{
    namespace Audio
    {
        class LavaSound : public NonCopyable
        {
        public:
            LavaSound(Content::ContentManager* cm, SoundManager* manager, const Chainf& path, float minDistance, float maxDistance);
            ~LavaSound();

            void Play(float rampUpTime);

            void Update(double totalTime, float dt, const Camera& camera);

        private:
            bool _playing;
            float _timer;
            float _totalTime;
            
            float _minDist;
            float _maxDist;

            SoundManager* _soundManager;
            const Sound* _sound;
            std::shared_ptr<ManagedSoundInstance> _soundInstance;
            const Chainf _path;
        };
    }

    template <>
    void EnumeratePreloads<Audio::LavaSound>(PreloadSet& preloads);
}
