#pragma once

#include "Abilities/BasicAbility.hpp"
#include "Particles/ParticleSystemInstance.hpp"
#include "SoundSet.hpp"


namespace Dwarf
{
    namespace Ability
    {
        class GroundSlam : public BasicAbility
        {
        public:
            GroundSlam(const AbilityParameters& parameters);

            virtual void ClearState() override;
            virtual bool IsInValidState() const override;
            virtual Character::Action GenerateAction() const override;
            virtual void ExecuteAction(const Character::Action& action) override;
            virtual void CancelAction() override;
            virtual bool IsExecutingAction() const override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Vector2f _startPos;
            float _ownerHeight;

            bool _hit;
            float _hitTimmer;
            float _actionTimer;

            bool _executing;

            const Character::Damage _damage;

            Audio::SoundSet _landSounds;
            Particles::ParticleSystemInstance* _landParticles;
        };
    }

    template <>
    void EnumeratePreloads<Ability::GroundSlam>(PreloadSet& preloads);
}
