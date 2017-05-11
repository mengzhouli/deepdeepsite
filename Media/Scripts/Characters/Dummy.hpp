#pragma once

#include "Characters/BasicCharacter.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Dummy : public BasicCharacter
        {
        public:
            Dummy(const CharacterParameters& parameters);

            virtual void OnPostRecieveDamage(Character* attacker, const Vector2f& position, const ResolvedDamage& dmg) override;

        private:
            Animation::AnimationSet _damagedAnimations;
        };
    }

    template <>
    void EnumeratePreloads<Character::Dummy>(PreloadSet& preloads);
}
