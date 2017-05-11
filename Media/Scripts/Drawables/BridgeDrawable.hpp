#pragma once

#include "Content/IHasContent.hpp"
#include "Graphics/IDrawable.hpp"
#include "Geometry/Chain.hpp"
#include "Geometry/RotatedRectangle.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Level/LevelInstance.hpp"

#include "Drawables/RopeDrawable.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class BridgeDrawable : public IDrawable, public Content::IHasContent, public Resource
        {
        public:
            BridgeDrawable();

            void SetEndpoints(const Vector2f& a, const Vector2f& b);
            void SetColor(const Color& color);
            void SetBuildPercent(float buildPerc);

            const Chainf& GetPathChain() const;

            void LoadContent(Content::ContentManager* contentManager);
            void UnloadContent();

            void Update(double totalTime, float dt);

            const Rectanglef& GetDrawBounds() const;
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

        private:
            void UpdateBridgeSegments();

            void UpdateLinkColors();

            Vector2f _leftSide;
            Vector2f _rightSide;
            float _buildPerc;
            int32_t _buildDirection;
            Color _color;

            Content::ContentManager* _contentManager;

            Chainf _pathChain;

            bool _contentLoaded;

            const Animation::Skeleton* _linkSkeleton;
            const Graphics::PolygonMaterialSet* _linkMaterialSet;
            std::vector<Animation::SkeletonInstance*> _links;

            Animation::SkeletonInstance* _leftEnd;
            Animation::SkeletonInstance* _rightEnd;

            RopeDrawable* _rope;

            Rectanglef _bounds;
        };
    }

    template <>
    void EnumeratePreloads<Graphics::BridgeDrawable>(PreloadSet& preloads);
}
