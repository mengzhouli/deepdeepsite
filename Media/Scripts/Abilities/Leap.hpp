#pragma once

#include "Abilities/BasicAbility.hpp"
#include "Particles/ParticleSystemInstance.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class Leap : public BasicAbility
        {
        public:
            Leap(const AbilityParameters& parameters, float range, float damage);
            Leap(const AbilityParameters& parameters);

            virtual void ClearState() override;
            virtual void SetPrimaryTarget(const Vector2f& pos) override;
            virtual bool IsInValidState() const override;
            virtual Character::Action GenerateAction() const override;
            virtual void ExecuteAction(const Character::Action& action) override;
            virtual void CancelAction() override;
            virtual bool IsExecutingAction() const override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;
            virtual void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            bool validateTarget(Vector2f& target, Splinef& spline) const;

            Vector2f _startPos;
            float _ownerHeight;
            float _timer;
            float _duration;
            bool _executing;

            float _windUpTimer;
            float _landTimer;

            const float _speed;
            const float _range;
            const float _damageRadius;
            const Character::Damage _damage;

            bool _draw;
            Splinef _jumpSpline;

            bool _targetValid;
            Vector2f _target;

            Audio::SoundSet _landSounds;
            Particles::ParticleSystemInstance* _landParticles;
        };
    }

    template <>
    void EnumeratePreloads<Ability::Leap>(PreloadSet& preloads);
}
