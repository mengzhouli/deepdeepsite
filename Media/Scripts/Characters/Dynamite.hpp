#pragma once

#include "Characters/Explosive.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Dynamite : public Explosive
        {
        public:
            Dynamite(const CharacterParameters& parameters, CharacterID owner, float duration, float radius, float dmg);

            virtual float OnPreInteractedWith(Character* interactor) override;
        };
    }

    template <>
    void EnumeratePreloads<Character::Dynamite>(PreloadSet& preloads);
}
