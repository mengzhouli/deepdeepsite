#pragma once

#include "Abilities/BasicAbility.hpp"

#include "Animation/SkeletonInstance.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class BuildBarricade : public BasicAbility
        {
        public:
            BuildBarricade(const AbilityParameters& parameters);

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

        private:
            bool VerifyBerricadePosition(const Vector2f& pos, Vector2f& placePos);

            bool _executing;

            bool _drawBarricade;
            bool _validPosition;
            Vector2f _barricadePosition;

            float _barricadeHealth;

            Animation::SkeletonInstance* _barricade;
        };
    }

    template <>
    void EnumeratePreloads<Ability::BuildBarricade>(PreloadSet& preloads);
}
