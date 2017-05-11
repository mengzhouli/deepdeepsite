#pragma once

#include "Characters/Explosive.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Bomb : public Explosive
        {
        public:
            Bomb(const CharacterParameters& parameters, const std::string& skel, const std::string& matset, const std::string& fuseName, const std::string& exposionOrigin,
                 CharacterID owner, float duration, float radius, float dmg);

        protected:
            void OnUpdate(double totalTime, float dt) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

        private:
            Audio::SoundSet _bounceSounds;
        };

        class IronBomb : public Bomb
        {
        public:
            IronBomb(const CharacterParameters& parameters, CharacterID owner, float duration, float radius, float dmg);
        };
    }

    template <>
    void EnumeratePreloads<Character::Bomb>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::IronBomb>(PreloadSet& preloads);
}
