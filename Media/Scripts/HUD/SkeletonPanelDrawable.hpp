#pragma once

#include "HUD/Panel.hpp"
#include "Graphics/MaterialSet.hpp"
#include "Animation/SkeletonInstance.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class SkeletonPanelDrawable : public PanelDrawable
        {
        public:
            SkeletonPanelDrawable(const Animation::Skeleton* skeleton);
            virtual ~SkeletonPanelDrawable();

            void MergeMaterialSet(const Graphics::PolygonMaterialSet* matset);

            void SetPose(const std::string& anim, float time);
            void SetRotation(const Rotatorf& rotation);
            void SetScale(float scale);
            void SetColor(const Color& color);
            void SetSize(const Vector2f& size);

            Vector2f GetSize(float scale, const Vector2f& maxSize) const override;
            void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const override;

        private:
            float _skeletonScale = 1.0f;
            Rotatorf _skeletonRotation = 0.0f;
            Color _skeletonColor = Color::FromBytes(255, 255, 255, 255);
            Vector2f _skeletonSize = Vector2f::Zero;

            Animation::SkeletonInstance* _skeleton = nullptr;
        };
    }
}
