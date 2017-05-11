#pragma once

#include "Audio/Sound.hpp"
#include "Content/IHasContent.hpp"
#include "Content/Preload.hpp"

namespace Dwarf
{
    namespace Audio
    {
        typedef std::pair<std::string, float> SoundPathWeightPair;
        typedef std::vector<SoundPathWeightPair> SoundPathVector;

        class SoundSet final
        {
        public:
            SoundSet() = default;
            SoundSet(const SoundPathVector& sounds);

            void AddSound(const std::string& soundPath, float weight);
            void AddSound(const SoundPathWeightPair& sound);
            void AddSounds(const SoundPathVector& sounds);

            const Sound* GetNextSound() const;

            uint32_t Count() const;

            void Clear();

            void LoadContent(Content::ContentManager* contentManager);
            void UnloadContent();

        private:
            struct Item
            {
                std::string path = "";
                ResourcePointer<const Sound> sound;
                float weight = 0.0f;
            };
            std::vector<Item> _weights;
        };
    }

    void EnumerateSoundVectorPreloads(PreloadSet& preloads, const Audio::SoundPathVector& sounds);
}
