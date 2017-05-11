#pragma once

#include "Characters/BasicCharacter.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Barricade : public BasicCharacter
        {
        public:
            Barricade(const CharacterParameters& parameters, float maxHealth);

        protected:
            virtual void OnPostRecieveDamage(Character* attacker, const Vector2f& position, const ResolvedDamage& dmg) override;

            virtual void OnStateChange(CharacterState newState) override;

        private:
            Animation::AnimationSet _damagedAnimations;
        };
    }

    template <>
    void EnumeratePreloads<Character::Barricade>(PreloadSet& preloads);
}
