#pragma once

#include "Characters/BasicCharacter.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Gobbo : public BasicCharacter
        {
        public:
            Gobbo(const CharacterParameters& parameters);
        };

        class GobboThrower : public Gobbo
        {
        public:
            GobboThrower(const CharacterParameters& parameters);
        };

        class GobboBowman : public Gobbo
        {
        public:
            GobboBowman(const CharacterParameters& parameters);
        };

        class GobboBomber : public Gobbo
        {
        public:
            GobboBomber(const CharacterParameters& parameters);

        protected:
            virtual void OnSpawn() override;

            virtual void OnStateChange(CharacterState state) override;

            virtual void OnUpdate(double totalTime, float dt) override;

        private:
            CharacterID _bomb;
        };

        class GobboChief : public Gobbo
        {
        public:
            GobboChief(const CharacterParameters& parameters);
        };

        class GobboTower : public BasicCharacter
        {
        public:
            GobboTower(const CharacterParameters& parameters);

        protected:
            void OnStateChange(CharacterState newState) override;

            void OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg) override;

        private:
            Animation::AnimationSet _onRecieveDamageAnimations;
            std::vector<std::vector<std::string>> _remainingBreakableMaterials;
        };

        class GobboBramble : public BasicCharacter
        {
        public:
            GobboBramble(const CharacterParameters& parameters);

        protected:
            virtual void OnSpawn() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnStateChange(CharacterState newState) override;

        private:
            float _dmgTick;

            Animation::AnimationSet _attackAnimations;
        };
    }

    template <>
    void EnumeratePreloads<Character::Gobbo>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GobboThrower>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GobboBowman>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GobboBomber>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GobboChief>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GobboTower>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GobboBramble>(PreloadSet& preloads);
}
