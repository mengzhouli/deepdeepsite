#pragma once

#include "Abilities/BasicAbility.hpp"
#include "SoundSet.hpp"

#include "Particles/ParticleSystemInstance.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class SackSmash : public BasicAbility
        {
        public:
            SackSmash(const AbilityParameters& parameters);

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
            void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            bool _placementValid = false;
            Vector2f _target;

            bool _executing = false;
            Vector2f _executionTarget;

            bool _smashing = false;
            float _smashTimer = -1.0f;
            float _wooshTimer = -1.0f;
            float _hitTimer = -1.0f;

            Audio::SoundSet _sackWooshSounds;
            Audio::SoundSet _sackHitSounds;

            ResourcePointer<Particles::ParticleSystemInstance> _dustParticles;
        };
    }

    template <>
    void EnumeratePreloads<Ability::SackSmash>(PreloadSet& preloads);
}
