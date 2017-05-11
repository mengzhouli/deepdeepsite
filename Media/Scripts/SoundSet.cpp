#include "SoundSet.hpp"
#include "Random.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Audio
    {
        SoundSet::SoundSet(const SoundPathVector& sounds)
        {
            AddSounds(sounds);
        }

        void SoundSet::AddSound(const std::string& soundPath, float weight)
        {
            // Scan the animations to make sure if this sound exists already and update
            // it if it does exist
            for (uint32_t i = 0; i < _weights.size(); i++)
            {
                if (_weights[i].path == soundPath)
                {
                    // Update the weight
                    float curWeight = _weights[i].weight;
                    if (i > 0)
                    {
                        curWeight -= _weights[i - 1].weight;
                    }

                    float weightDif = weight - curWeight;
                    _weights[i].weight = weight;
                    if (i > 0)
                    {
                        _weights[i].weight += _weights[i - 1].weight;
                    }

                    for (uint32_t j = i + 1; j < _weights.size(); j++)
                    {
                        _weights[j].weight += weightDif;
                    }

                    return;
                }
            }

            // Insert
            float finalWeight = (_weights.size() > 0) ? _weights.back().weight + weight : weight;

            Item newItem;
            newItem.path = soundPath;
            newItem.weight = finalWeight;

            _weights.push_back(newItem);
        }

        void SoundSet::AddSound(const SoundPathWeightPair& sound)
        {
            AddSound(sound.first, sound.second);
        }

        void SoundSet::AddSounds(const SoundPathVector& sounds)
        {
            for (const auto& sound : sounds)
            {
                AddSound(sound.first, sound.second);
            }
        }

        const Sound* SoundSet::GetNextSound() const
        {
            if (!_weights.empty())
            {
                float randVal = Random::RandomBetween(0.0f, _weights.back().weight);
                for (const auto& sound : _weights)
                {
                    if (randVal <= sound.weight)
                    {
                        assert(sound.sound != nullptr);
                        return sound.sound;
                    }
                }
            }

            assert(_weights.empty());
            return nullptr;
        }

        void SoundSet::LoadContent(Content::ContentManager* contentManager)
        {
            for (auto& sound : _weights)
            {
                sound.sound = EmplaceResource(contentManager->Load<Sound>(sound.path));
            }
        }

        void SoundSet::UnloadContent()
        {
            for (auto& sound : _weights)
            {
                sound.sound.Reset();
            }
        }

        uint32_t SoundSet::Count() const
        {
            return _weights.size();
        }

        void SoundSet::Clear()
        {
            _weights.clear();
        }
    }

    void EnumerateSoundVectorPreloads(PreloadSet& preloads, const Audio::SoundPathVector& sounds)
    {
        for (const auto& sound : sounds)
        {
            preloads.insert(sound.first);
        }
    }
}
