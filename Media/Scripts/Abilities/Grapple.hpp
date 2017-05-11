#pragma once

#include "Abilities/BasicAbility.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class GrappleRopeDrawable;
    }

    namespace Ability
    {
        class Grapple : public BasicAbility
        {
        public:
            Grapple(const AbilityParameters& parameters);
            Grapple(const AbilityParameters& parameters, bool drawPlacementRope);

            virtual void ClearState() override;
            virtual void SetPrimaryTarget(const Vector2f& pos) override;
            virtual bool IsInValidState() const override;
            virtual Character::Action GenerateAction() const override;
            virtual void ExecuteAction(const Character::Action& action) override;
            virtual void CancelAction() override;
            virtual bool IsExecutingAction() const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;
            virtual void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            bool VerifyLedgePosition(const Vector2f& inputPos, Vector2f& ledgePos, Vector2f& groundPos);

            float _grappleRange;
            Vector2f _selectedLedgePos;
            bool _ledgeIsValid;

            const bool _drawPlacementRopeEnabled;

            bool _draw;
            Graphics::GrappleRopeDrawable* _drawable;

            Audio::SoundSet _grappleShootSounds;
        };
    }

    template <>
    void EnumeratePreloads<Ability::Grapple>(PreloadSet& preloads);
}
