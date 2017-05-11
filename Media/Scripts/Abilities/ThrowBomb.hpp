#pragma once

#include "Animation/SkeletonInstance.hpp"

#include "Abilities/BasicAbility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class ThrowBomb : public BasicAbility
        {
        public:
            ThrowBomb(const AbilityParameters& parameters);

            void ClearState() override;
            virtual void SetPrimaryTarget(const Vector2f& pos) override;
            virtual bool IsInValidState() const override;
            virtual Character::Action GenerateAction() const override;
            virtual void ExecuteAction(const Character::Action& action) override;
            virtual void CancelAction() override;
            virtual bool IsExecutingAction() const override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;
            void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            void computeTrajectory(Vector2f& outLaunchPos, Vector2f& outVelocity, Vector2f& outImpulse, bool useHandPos);

            bool _executing;

            bool _validPosition;
            Vector2f _bombTarget;

            float _fuseLength;
            float _radius;
            float _dmg;

            bool _drawSpline = false;
            Splinef _trajectorySpline;

            bool _bombAttached;
            Animation::SkeletonInstance* _bomb;
        };
    }

    template <>
    void EnumeratePreloads<Ability::ThrowBomb>(PreloadSet& preloads);
}
