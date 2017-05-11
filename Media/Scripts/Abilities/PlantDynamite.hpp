#pragma once

#include "Animation/SkeletonInstance.hpp"

#include "Abilities/BasicAbility.hpp"

#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class PlantDynamite : public BasicAbility
        {
        public:
            PlantDynamite(const AbilityParameters& parameters);
            PlantDynamite(const AbilityParameters& parameters, float fuseLength, float dmg, float radius, bool drawPlacementKeg);

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
            void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            bool VerifyDynamitePosition(const Vector2f& pos, Vector2f& placePos);

            void setDrawPlacementDynamite(bool drawPlacement);
            void setDrawActionDynamite(bool drawAction);

            bool _executing;

            bool _drawPlacementDynamiteEnabled;
            bool _drawPlacementDynamite;
            bool _validPosition;

            float _fuseLength;
            float _radius;
            float _dmg;

            Animation::SkeletonInstance* _placementDynamite;

            bool _drawActionDynamite = false;
            float _dynamiteDropAnimationTimer = -1.0f;
            float _dynamiteSpawnTimer = -1.0f;
            float _dynamiteDropTimer = -1.0f;
            Animation::SkeletonInstance* _actionDynamite = nullptr;

            Audio::SoundSet _placementSounds;
        };
    }

    template <>
    void EnumeratePreloads<Ability::PlantDynamite>(PreloadSet& preloads);
}
