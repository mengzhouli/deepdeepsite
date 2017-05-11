#pragma once

#include "Audio/Sound.hpp"
#include "Audio/SoundInstance.hpp"
#include "Audio/SoundManager.hpp"
#include "Content/IHasContent.hpp"
#include "IUpdateable.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Audio
    {
        class AmbientSoundManager : public NonCopyable, public IUpdateable, public Content::IHasContent
        {
        public:
            AmbientSoundManager(SoundManager* soundManager);

            void AddSounds(EnvironmentType environment, const SoundPathVector& sounds, float volume);

            void SetVolume(float volume);
            float GetVolume() const;

            void LoadContent(Content::ContentManager* contentManager) override;
            void UnloadContent() override;

            void Update(double totalTime, float dt) override;

        private:
            SoundManager* _soundManager = nullptr;

            float _volume = 0.05f;

            struct environment
            {
                SoundSet sounds;
                float volume;
                std::shared_ptr<ManagedSoundInstance> curSound;
            };

            std::map<EnvironmentType, environment> _environments;
        };
    }
}
