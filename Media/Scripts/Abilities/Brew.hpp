#pragma once

#include "Animation/SkeletonInstance.hpp"
#include "Abilities/BasicAbility.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class Brew : public BasicAbility
        {
        public:
            Brew(const AbilityParameters& parameters, const std::string& nameCode, const std::string& skeletonPath, const std::string& foodMatSet);

            void ClearState() override;
            void SetPrimaryTarget(const Vector2f& pos) override;
            bool IsInValidState() const override;
            Character::Action GenerateAction() const override;

            void ExecuteAction(const Character::Action& action) override;
            void CancelAction() override;
            bool IsExecutingAction() const override;

            void OnUpdate(double totalTime, float dt) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            bool VerifyKegPosition(const Vector2f& pos, Vector2f& placePos);

            void setDrawPlacementKeg(bool drawPlacement);
            void setDrawActionKeg(bool drawActionKeg);

            virtual Character::CharacterConstructor<Character::Character> GetKegConstructor() const = 0;

            bool _executing = false;

            std::string _skeletonPath = "";
            std::string _matsetPath = "";

            bool _drawPlacementKeg = false;
            bool _placementValid = false;
            Animation::SkeletonInstance* _placementKeg = nullptr;

            bool _drawActionKeg = false;
            float _kegDropAnimationTimer = -1.0f;
            float _kegSpawnTimer = -1.0f;
            float _kegDropTimer = -1.0f;
            Animation::SkeletonInstance* _actionKeg = nullptr;

            Audio::SoundSet _spawnSounds;

            Character::CharacterID _prevKeg = 0;
        };

        class BrewBeer : public Brew
        {
        public:
            BrewBeer(const AbilityParameters& parameters);

        private:
            Character::CharacterConstructor<Character::Character> GetKegConstructor() const override;

            float _damageReduction;
        };

        class BrewWine : public Brew
        {
        public:
            BrewWine(const AbilityParameters& parameters);

        private:
            Character::CharacterConstructor<Character::Character> GetKegConstructor() const override;

            float _regen;
        };
    }

    template <>
    void EnumeratePreloads<Ability::Brew>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Ability::BrewBeer>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Ability::BrewWine>(PreloadSet& preloads);
}
