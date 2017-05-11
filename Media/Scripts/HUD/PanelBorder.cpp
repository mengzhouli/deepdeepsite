#include "HUD/PanelBorder.hpp"

namespace Dwarf
{
    namespace HUD
    {
        TilingBorderPanelDrawable::TilingBorderPanelDrawable(const Graphics::HUDMaterialSet* matset, const std::string& cornerTopLeft, const std::string& cornerTopRight, const std::string& cornerBottomLeft, const std::string& cornerBottomRight, const std::string& edgeleft, const std::string& edgeRight, const std::string& edgeTop, const std::string& edgeBottom) : _cornerTopLeft(nullptr)
            , _cornerTopRight(nullptr)
            , _cornerBottomLeft(nullptr)
            , _cornerBottomRight(nullptr)
            , _edgeLeft(nullptr)
            , _edgeRight(nullptr)
            , _edgeTop(nullptr)
            , _edgeBottom(nullptr)
        {
            _cornerTopLeft = matset->GetMaterial(cornerTopLeft);
            _cornerTopRight = matset->GetMaterial(cornerTopRight);
            _cornerBottomLeft = matset->GetMaterial(cornerBottomLeft);
            _cornerBottomRight = matset->GetMaterial(cornerBottomRight);
            _edgeLeft = matset->GetMaterial(edgeleft);
            _edgeRight = matset->GetMaterial(edgeRight);
            _edgeTop = matset->GetMaterial(edgeTop);
            _edgeBottom = matset->GetMaterial(edgeBottom);
        }

        PaddingFunction TilingBorderPanelDrawable::GetPaddingFunction() const
        {
            return [this](const Vector2f& size, float scale)
            {
                float maxEdgeWidth = Max(Max(_edgeLeft->GetSize().X, _edgeRight->GetSize().X), Max(_edgeTop->GetSize().Y, _edgeBottom->GetSize().Y));
                return Max(maxEdgeWidth - 1.0f, 0.0f) * scale;
            };
        }

        Vector2f TilingBorderPanelDrawable::GetSize(float scale, const Vector2f& maxSize) const
        {
            return Vector2f::Min(Vector2f(_cornerTopLeft->GetSize()) * 2.0f * scale, maxSize);
        }

        void TilingBorderPanelDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const
        {
            Vector2f downDir = Vector2f::Normalize(bounds.BottomLeft() - bounds.TopLeft());
            Vector2f rightDir = Vector2f::Normalize(bounds.TopRight() - bounds.TopLeft());

            {
                // Bottom Edge
                float bottomCornerSizeDif = (_cornerBottomRight->GetSize().X - _cornerBottomLeft->GetSize().X) * scale;
                Vector2f bottomSideCenter = bounds.Middle() + (downDir * (bounds.ExtentY - (_edgeBottom->GetSize().Y * 0.5f * scale))) - (rightDir * bottomCornerSizeDif * 0.5f);
                float bottomEdgeLength = (bounds.ExtentX * 2.0f) - ((_cornerBottomRight->GetSize().X + _cornerBottomLeft->GetSize().X) * scale);
                Rectanglef bottomSideRect(bottomSideCenter - Vector2f(bottomEdgeLength * 0.5f, _edgeBottom->GetSize().Y * 0.5f * scale), Vector2f(bottomEdgeLength, _edgeBottom->GetSize().Y * scale));
                spriteRenderer->DrawSprite(_edgeBottom->GetTexture(), bottomSideRect, _edgeBottom->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                // Top Edge
                float topCornerSizeDif = (_cornerTopRight->GetSize().X - _cornerTopLeft->GetSize().X) * scale;
                Vector2f topSideCenter = bounds.Middle() - (downDir * (bounds.ExtentY - (_edgeTop->GetSize().Y * 0.5f * scale))) - (rightDir * topCornerSizeDif * 0.5f);
                float topEdgeLength = (bounds.ExtentX * 2.0f) - ((_cornerTopRight->GetSize().X + _cornerTopLeft->GetSize().X) * scale);
                Rectanglef topSideRect(topSideCenter - Vector2f(topEdgeLength * 0.5f, _edgeTop->GetSize().Y * 0.5f * scale), Vector2f(topEdgeLength, _edgeTop->GetSize().Y * scale));
                spriteRenderer->DrawSprite(_edgeTop->GetTexture(), topSideRect, _edgeTop->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                // Left Edge
                float leftCornerSizeDif = (_cornerBottomLeft->GetSize().Y - _cornerTopLeft->GetSize().Y) * scale;
                Vector2f leftSideCenter = bounds.Middle() - (rightDir * (bounds.ExtentX - (_edgeLeft->GetSize().X * 0.5f * scale))) - (downDir * leftCornerSizeDif * 0.5f);
                float leftEdgeLength = (bounds.ExtentY * 2.0f) - ((_cornerBottomLeft->GetSize().Y - _cornerTopLeft->GetSize().Y) * scale);
                Rectanglef leftSideRect(leftSideCenter - Vector2f(_edgeLeft->GetSize().X * 0.5f * scale, leftEdgeLength * 0.5f), Vector2f(_edgeLeft->GetSize().X * scale, leftEdgeLength));
                spriteRenderer->DrawSprite(_edgeLeft->GetTexture(), leftSideRect, _edgeLeft->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                // Right Edge
                float rightCornerSizeDif = (_cornerBottomRight->GetSize().Y - _cornerTopRight->GetSize().Y) * scale;
                Vector2f rightSideCenter = bounds.Middle() + (rightDir * (bounds.ExtentX - (_edgeRight->GetSize().X * 0.5f * scale))) - (downDir * rightCornerSizeDif * 0.5f);
                float rightEdgeLength = (bounds.ExtentY * 2.0f) - ((_cornerBottomRight->GetSize().Y - _cornerTopRight->GetSize().Y) * scale);
                Rectanglef rightSideRect(rightSideCenter - Vector2f(_edgeRight->GetSize().X * 0.5f * scale, rightEdgeLength * 0.5f), Vector2f(_edgeRight->GetSize().X * scale, rightEdgeLength));
                spriteRenderer->DrawSprite(_edgeRight->GetTexture(), rightSideRect, _edgeRight->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                // Top Left
                Vector2f topLeftCornerCenter = bounds.TopLeft() + (downDir * (_cornerTopLeft->GetSize().Y * 0.5f * scale)) + (rightDir * (_cornerTopLeft->GetSize().X * 0.5f * scale));
                Rectanglef topLeftCornerRect(topLeftCornerCenter - Vector2f(_cornerTopLeft->GetSize()) * 0.5f * scale, Vector2f(_cornerTopLeft->GetSize() * scale));
                spriteRenderer->DrawSprite(_cornerTopLeft->GetTexture(), topLeftCornerRect, _cornerTopLeft->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                // Bottom Left
                Vector2f bottomLeftCornerCenter = bounds.BottomLeft() - (downDir * (_cornerBottomLeft->GetSize().Y * 0.5f * scale)) + (rightDir * (_cornerBottomLeft->GetSize().X * 0.5f * scale));
                Rectanglef bottomLeftCornerRect(bottomLeftCornerCenter - Vector2f(_cornerBottomLeft->GetSize()) * 0.5f * scale, Vector2f(_cornerBottomLeft->GetSize() * scale));
                spriteRenderer->DrawSprite(_cornerBottomLeft->GetTexture(), bottomLeftCornerRect, _cornerBottomLeft->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                // Top Right
                Vector2f topRightCornerCenter = bounds.TopRight() + (downDir * (_cornerTopRight->GetSize().Y * 0.5f * scale)) - (rightDir * (_cornerTopRight->GetSize().X * 0.5f * scale));
                Rectanglef topRightCornerRect(topRightCornerCenter - Vector2f(_cornerTopRight->GetSize()) * 0.5f * scale, Vector2f(_cornerTopRight->GetSize() * scale));
                spriteRenderer->DrawSprite(_cornerTopRight->GetTexture(), topRightCornerRect, _cornerTopRight->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                // Bottom Right
                Vector2f bottomRightCornerCenter = bounds.BottomRight() - (downDir * (_cornerBottomRight->GetSize().Y * 0.5f * scale)) - (rightDir * (_cornerBottomRight->GetSize().X * 0.5f * scale));
                Rectanglef bottomRightCornerRect(bottomRightCornerCenter - Vector2f(_cornerBottomRight->GetSize()) * 0.5f * scale, Vector2f(_cornerBottomRight->GetSize() * scale));
                spriteRenderer->DrawSprite(_cornerBottomRight->GetTexture(), bottomRightCornerRect, _cornerBottomRight->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }
        }

        TilingBorderPanelDrawable::~TilingBorderPanelDrawable()
        {

        }

        StretchingBorderPanelDrawable::StretchingBorderPanelDrawable(const Graphics::HUDMaterialSet* matset, const std::string& borderMaterial, const std::string& innerBorderRect) : _drawable(nullptr)
            , _edgeThickness(0)
        {
            assert(matset);
            const Graphics::HUDMaterial* outterArea = matset->GetMaterial(borderMaterial);
            const Graphics::HUDMaterial* innerArea = matset->GetMaterial(innerBorderRect);

            _drawable = MakeResource<HUDMaterialPanelDrawable>(outterArea);

            Vector2f sizeDiff = Vector2f(outterArea->GetSize()) - Vector2f(innerArea->GetSize());
            _edgeThickness = Vector2f::Max(sizeDiff * 0.5f, Vector2f::Zero) / Vector2f(outterArea->GetSize());
        }

        PaddingFunction StretchingBorderPanelDrawable::GetPaddingFunction() const
        {
            return [this](const Vector2f& size, float scale)
            {
                return size * _edgeThickness;
            };
        }

        Vector2f StretchingBorderPanelDrawable::GetSize(float scale, const Vector2f& maxSize) const
        {
            return _drawable->GetSize(scale, maxSize);
        }

        void StretchingBorderPanelDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const
        {
            _drawable->Draw(spriteRenderer, color, bounds, scale);
        }

        StretchingBorderPanelDrawable::~StretchingBorderPanelDrawable()
        {
        }

        VerticalStretchingBorderPanelDrawable::VerticalStretchingBorderPanelDrawable(const Graphics::HUDMaterialSet* matset,
                                                                                    const std::string& topMaterial,
                                                                                    const std::string& middleMaterial,
                                                                                    const std::string& bottomMaterial,
                                                                                    const Vector2f& padding,
                                                                                    float scale)
            : _topEdge(matset->GetMaterial(topMaterial))
            , _middle(matset->GetMaterial(middleMaterial))
            , _bottomEdge(matset->GetMaterial(bottomMaterial))
            , _padding(padding)
            , _scale(scale)
        {
        }

        PaddingFunction VerticalStretchingBorderPanelDrawable::GetPaddingFunction() const
        {
            return [this](const Vector2f& size, float scale)
            {
                return _padding * getDrawScale(size.X);
            };
        }

        Vector2f VerticalStretchingBorderPanelDrawable::GetSize(float scale, const Vector2f& maxSize) const
        {
            return Vector2f(_topEdge->GetPixelArea().W, _topEdge->GetPixelArea().H + _bottomEdge->GetPixelArea().H) * _scale;
        }

        void VerticalStretchingBorderPanelDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const
        {
            Vector2f downDir = Vector2f::Normalize(bounds.BottomLeft() - bounds.TopLeft());
            Vector2f rightDir = Vector2f::Normalize(bounds.TopRight() - bounds.TopLeft());

            float drawScale = getDrawScale(bounds.Width());

            Vector2f topSize = _topEdge->GetPixelArea().Size * drawScale;
            Vector2f botSize = _bottomEdge->GetPixelArea().Size * drawScale;
            float middleHeight = bounds.Height() - botSize.Y - topSize.Y;

            if (middleHeight > 0)
            {
                middleHeight += 1.0f;
                Vector2f middle = bounds.Center - (downDir * bounds.ExtentY) + (downDir * topSize.Y);
                Vector2f middleSize(bounds.Width(), middleHeight);
                Rectanglef middleRect(middle - Vector2f(middleSize.X * 0.5f, 0.0f), middleSize);
                spriteRenderer->DrawSprite(_middle->GetTexture(), middleRect, _middle->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                Vector2f topDrawPos = bounds.Center - (downDir * bounds.ExtentY);
                Rectanglef topRect(topDrawPos - Vector2f(topSize.X * 0.5f, 0.0f), topSize);
                spriteRenderer->DrawSprite(_topEdge->GetTexture(), topRect, _topEdge->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }

            {
                Vector2f botDrawPos = bounds.Center + (downDir * bounds.ExtentY) - (downDir * botSize.Y);
                Rectanglef botRect(botDrawPos - Vector2f(botSize.X * 0.5f, 0.0f), botSize);
                spriteRenderer->DrawSprite(_bottomEdge->GetTexture(), botRect, _bottomEdge->GetSubRect(), color, bounds.R, Vector2f(0.5f));
            }
        }

        float VerticalStretchingBorderPanelDrawable::getDrawScale(float width) const
        {
            return width / _topEdge->GetPixelArea().W;
        }
    }
}
