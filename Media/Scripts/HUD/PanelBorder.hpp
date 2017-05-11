#pragma once

#include "HUD/Panel.hpp"
#include "Graphics/MaterialSet.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class BorderPanelDrawable : public PanelDrawable
        {
        public:
            virtual PaddingFunction GetPaddingFunction() const = 0;
        };

        class TilingBorderPanelDrawable : public BorderPanelDrawable
        {
        public:
            TilingBorderPanelDrawable(const Graphics::HUDMaterialSet* matset, const std::string& cornerTopLeft, const std::string& cornerTopRight,
                                       const std::string& cornerBottomLeft, const std::string& cornerBottomRight, const std::string& edgeleft,
                                       const std::string& edgeRight, const std::string& edgeTop, const std::string& edgeBottom);

            virtual PaddingFunction GetPaddingFunction() const override;

            virtual Vector2f GetSize(float scale, const Vector2f& maxSize) const override;

            virtual void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const override;

        protected:
            virtual ~TilingBorderPanelDrawable();

        private:
            ResourcePointer<const Graphics::HUDMaterial> _cornerTopLeft;
            ResourcePointer<const Graphics::HUDMaterial> _cornerTopRight;
            ResourcePointer<const Graphics::HUDMaterial> _cornerBottomLeft;
            ResourcePointer<const Graphics::HUDMaterial> _cornerBottomRight;
            ResourcePointer<const Graphics::HUDMaterial> _edgeLeft;
            ResourcePointer<const Graphics::HUDMaterial> _edgeRight;
            ResourcePointer<const Graphics::HUDMaterial> _edgeTop;
            ResourcePointer<const Graphics::HUDMaterial> _edgeBottom;
        };

        class StretchingBorderPanelDrawable : public BorderPanelDrawable
        {
        public:
            StretchingBorderPanelDrawable(const Graphics::HUDMaterialSet* matset, const std::string& borderMaterial, const std::string& innerBorderRect);

            virtual PaddingFunction GetPaddingFunction() const override;

            virtual Vector2f GetSize(float scale, const Vector2f& maxSize) const override;

            virtual void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const;

        protected:
            virtual ~StretchingBorderPanelDrawable();

        private:
            ResourcePointer<HUDMaterialPanelDrawable> _drawable;
            Vector2f _edgeThickness;
        };

        class VerticalStretchingBorderPanelDrawable : public BorderPanelDrawable
        {
        public:
            VerticalStretchingBorderPanelDrawable(const Graphics::HUDMaterialSet* matset, const std::string& topMaterial, const std::string& middleMaterial,
                                                  const std::string& bottomMaterial, const Vector2f& padding, float scale);

            PaddingFunction GetPaddingFunction() const override;

            Vector2f GetSize(float scale, const Vector2f& maxSize) const override;

            void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const;

        private:
            float getDrawScale(float width) const;

            ResourcePointer<const Graphics::HUDMaterial> _topEdge;
            ResourcePointer<const Graphics::HUDMaterial> _middle;
            ResourcePointer<const Graphics::HUDMaterial> _bottomEdge;
            Vector2f _padding;
            float _scale;
        };
    }
}
