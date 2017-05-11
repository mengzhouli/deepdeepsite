#pragma once

#include "Character/Character.hpp"
#include "Graphics/Geometry/RectangleDrawable.hpp"
#include "Physics/RopeCollision.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Graphics/PolygonMaterial.hpp"
#include "Level/LevelInstance.hpp"

#include "Drawables/BridgeDrawable.hpp"
#include "Drawables/RopeDrawable.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Bridge : public Character
        {
        public:
            Bridge(const CharacterParameters& parameters, const Vector2f& a, const Vector2f& b, float buildTime);

            bool IsBuilt() const;
            bool IsBuilding() const;
            void SetBuilding(bool building);

            const Vector2f& GetSideA() const;
            const Vector2f& GetSideB() const;

            virtual void OnSpawn() override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        protected:
            virtual ~Bridge();

        private:
            void updateBuilding(float dt);

            float _totalBuildTime;
            float _remainingBuildTime;
            bool _buildFinished;
            bool _building;

            Pathfinding::PathItemID _pathId;

            Vector2f _a;
            Vector2f _b;
            Graphics::BridgeDrawable* _bridge;
        };

        class Bridge2 : public Character, public Graphics::IDrawable
        {
        public:
            Bridge2(const CharacterParameters& parameters, const Vector2f& a, const Vector2f& b, float buildPerc, const Item::Resources& cost);

            const App::Cursor* GetInteractCursor() const override;
            const HUD::Panel* GetTooltip() const override;

            float GetBuildPercentage() const;
            bool IsBuilt() const;

            float GetGoldCostPerSecond(float buildSpeedMultiplier) const;
            void Build(const Item::Resources& payment, float speedMultiplier);

            const Rectanglef& GetBounds() const override;

            bool Intersects(const Vector2f& pt) const override;

            void Draw(Graphics::SpriteRenderer* spriteRenderer) const override;
            const Rectanglef& GetDrawBounds() const override;

        protected:
            virtual ~Bridge2();

            void OnSpawn() override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            bool CanBeInteractedWith(const Character* interactor) const;

            Vector2f GetInteractionMoveTarget(const Character* interactor) const override;

            Vector2f GetLineOfSightPoint() const override;

        private:
            ResourcePointer<const App::Cursor> _buildCursor;

            mutable bool _tooltipDirty = true;
            mutable ResourcePointer<const HUD::Panel> _tooltip;

            float _goldCostPerSecond;

            Item::Resources _totalCost;
            Item::Resources _remainingCost;

            Content::ContentManager* _contentManager;

            float _buildPerc;
            float _buildPercPaidFor = 0.0f;
            float _buildSpeedMultiplier = 1.0f;

            struct bridgeSide
            {
                Vector2f position;

                ResourcePointer<Animation::SkeletonInstance> endPiece;

                Chainf bridgeChain;
                ResourcePointer<Physics::RopeCollision> bridgeCollision;
                std::vector<ResourcePointer<Animation::SkeletonInstance>> bridgeSlats;

                Chainf ropeChain;
                ResourcePointer<Physics::RopeCollision> ropeCollision;
                ResourcePointer<Graphics::RopeDrawable> ropeDrawable;

                std::vector<ResourcePointer<Graphics::RopeDrawable>> linkRopes;
            };

            bridgeSide _a;
            bridgeSide _b;

            Pathfinding::PathItemID _pathId = Pathfinding::PathItemID_Invalid;

            Rectanglef _bounds;
        };
    }

    template <>
    void EnumeratePreloads<Character::Bridge>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::Bridge2>(PreloadSet& preloads);
}
