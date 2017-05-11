#pragma once

#include "Abilities/BasicAbility.hpp"

#include "Characters/Bridge.hpp"
#include "Drawables/BridgeDrawable.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class BuildBridge : public BasicAbility
        {
        public:
            BuildBridge(const AbilityParameters& parameters);

            virtual void ClearState() override;
            virtual void SetPrimaryTarget(const Vector2f& pos) override;
            virtual void SetSecondaryTarget(const Vector2f& pos) override;
            virtual bool IsInValidState() const override;
            virtual Character::Action GenerateAction() const override;
            virtual void ExecuteAction(const Character::Action& action) override;
            virtual void CancelAction() override;
            virtual bool IsExecutingAction() const override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        protected:
            virtual ~BuildBridge();

        private:
            bool VerifyBridgeStart(const Vector2f& pos, Vector2f& outPos, Vector2f& outMovePos) const;
            bool VerifyBridgeEnd(const Vector2f& pos, Vector2f& outPos) const;
            bool VerifyBridge(const Vector2f& start, const Vector2f& end, const Chainf& bridge) const;

            float _minLength;
            float _maxLength;
            float _bridgeBuildTime;

            const float _ledgeSearchRadius = 500.0f;

            Vector2f _startPos;
            Vector2f _startMovePos;
            Vector2f _endPos;

            bool _startValid;
            bool _endValid;
            bool _bridgeValid;

            Character::Bridge* _bridge;

            bool _drawBridge;
            Graphics::BridgeDrawable* _bridgeDrawable;

            bool _executing;
        };
    }

    template <>
    void EnumeratePreloads<Ability::BuildBridge>(PreloadSet& preloads);
}
