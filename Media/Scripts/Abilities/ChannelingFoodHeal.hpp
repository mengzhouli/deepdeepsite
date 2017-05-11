#pragma once

#include "Animation/SkeletonInstance.hpp"
#include "Abilities/BasicAbility.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class ChannelingFoodHeal : public BasicAbility
        {
        public:
            ChannelingFoodHeal(const AbilityParameters& parameters);

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
            bool VerifyPotPosition(const Vector2f& pos, Vector2f& placePos);
            void setDrawPot(bool drawPot);
            void setDrawFood(bool drawFood);
            void setDrawPlacementPot(bool drawPot, bool drawRange);

            bool _executing = false;

            bool _drawPot = false;
            Vector2f _potPosition;
            Animation::SkeletonInstance* _cookPot = nullptr;

            bool _cooking = false;
            float _cookingTimer = 0.0f;

            std::vector<Character::CharacterID> _targets;

            bool _throwing = false;
            bool _drawFood = false;
            uint32_t _curFoodIdx = 0;
            std::vector<Animation::SkeletonInstance*> _foods;

            bool _validPosition = false;
            Vector2f _placementPotPosition;
            bool _drawPlacementPot = false;
            bool _drawPlacementPotRange = false;
            Animation::SkeletonInstance* _placementPot = nullptr;

            Audio::SoundSet _cookingSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _curCookingSound;
        };
    }

    template <>
    void EnumeratePreloads<Ability::ChannelingFoodHeal>(PreloadSet& preloads);
}
