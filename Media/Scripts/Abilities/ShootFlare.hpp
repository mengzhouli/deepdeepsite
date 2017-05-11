#pragma once

#include "Abilities/BasicAbility.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class ShootFlare : public BasicAbility
        {
        public:
            ShootFlare(const AbilityParameters& parameters);
            ShootFlare(const AbilityParameters& parameters, bool drawSpline);

            virtual void ClearState() override;
            virtual void SetPrimaryTarget(const Vector2f& pos) override;
            virtual bool IsInValidState() const override;
            virtual Character::Action GenerateAction() const override;
            virtual void ExecuteAction(const Character::Action& action) override;
            virtual void CancelAction() override;
            virtual bool IsExecutingAction() const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;
            virtual void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            const float _flareDuration;

            void computeTrajectory(Vector2f& outLaunchPos, Vector2f& outVelocity, Vector2f& outImpulse);

            bool _drawSplineEnabled;
            bool _drawSpline;
            Splinef _trajectorySpline;
            Vector2f _target;
            
            Audio::SoundSet _flareShootSounds;
        };
    }

    template <>
    void EnumeratePreloads<Ability::ShootFlare>(PreloadSet& preloads);
}
