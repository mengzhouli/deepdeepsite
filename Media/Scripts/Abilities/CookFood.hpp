#pragma once

#include "Animation/SkeletonInstance.hpp"
#include "Abilities/BasicAbility.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class CookFood : public BasicAbility
        {
        public:
            CookFood(const AbilityParameters& parameters, const std::string& nameCode, const std::string& foodMatSet);

            virtual void ClearState() override;
            virtual void SetPrimaryTarget(const Vector2f& pos) override;
            virtual bool IsInValidState() const override;
            virtual Character::Action GenerateAction() const override;

            virtual void ExecuteAction(const Character::Action& action) override;
            virtual void CancelAction() override;
            virtual bool IsExecutingAction() const override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            virtual Character::CharacterConstructor<Character::Character> GetFoodConstructor() const = 0;

        private:
            bool VerifyFoodPosition(const Vector2f& pos, Vector2f& placePos);

            void setDrawCookpot(bool drawCookpot);

            bool _executing;

            bool _drawFood;
            bool _drawCookpot;
            bool _validPosition;
            Vector2f _foodPosition;

            bool _cooking;
            float _cookingTimer;

            std::string _foodMatsetPath;
            Animation::SkeletonInstance* _food;

            Character::CharacterID _prevFood = 0;

            Animation::SkeletonInstance* _cookPot;

            Audio::SoundSet _cookingSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _curCookingSound;
        };

        class CookCritFood : public CookFood
        {
        public:
            CookCritFood(const AbilityParameters& parameters);

            Character::CharacterConstructor<Character::Character> GetFoodConstructor() const override;

        private:
            float _critChance;
        };

        class CookAttackSpeedFood : public CookFood
        {
        public:
            CookAttackSpeedFood(const AbilityParameters& parameters);

            Character::CharacterConstructor<Character::Character> GetFoodConstructor() const override;

        private:
            float _attackSpeedBonus;
        };

        class CookBeef : public CookFood
        {
        public:
            CookBeef(const AbilityParameters& parameters);

            virtual Character::CharacterConstructor<Character::Character> GetFoodConstructor() const override;

        private:
            float _beefHealthBonus;
            float _beefBonusDuration;
        };

        class CookChicken : public CookFood
        {
        public:
            CookChicken(const AbilityParameters& parameters);

            virtual Character::CharacterConstructor<Character::Character> GetFoodConstructor() const override;

        private:
            float _chickenRegen;
            float _chickenDuration;
        };

        class CookHam : public CookFood
        {
        public:
            CookHam(const AbilityParameters& parameters);

            virtual Character::CharacterConstructor<Character::Character> GetFoodConstructor() const override;

        private:
            float _hamHealAmmount;
        };
    }

    template <>
    void EnumeratePreloads<Ability::CookFood>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Ability::CookCritFood>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Ability::CookAttackSpeedFood>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Ability::CookBeef>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Ability::CookChicken>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Ability::CookHam>(PreloadSet& preloads);
}
